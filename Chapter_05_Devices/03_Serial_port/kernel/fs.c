#define _K_FS_C_

#include <kernel/errno.h>
#include <kernel/memory.h>
#include <types/io.h>
#include <lib/list.h>
#include <lib/string.h>
#include "fs.h"
#include "device.h"
#include "time.h"
#include "memory.h"

static kdevice_t *disk;
#define DISK_WRITE(buffer, blocks, first_block) \
k_device_send(buffer, (first_block << 16) | blocks, 0, disk);

#define DISK_READ(buffer, blocks, first_block) \
k_device_recv(buffer, (first_block << 16) | blocks, 0, disk);

static struct fs_table *ft;
static size_t ft_size;
static struct kfile_desc *last_check;
static list_t open_files;

int k_fs_init(char *disk_device, size_t bsize, size_t blocks)
{
	disk = k_device_open ( disk_device, 0 );
	assert(disk);

	//initialize disk
	ft_size = sizeof(struct fs_table) + blocks;
	ft_size = (ft_size + bsize - 1) / bsize;
	ft = kmalloc(ft_size * bsize);
	memset(ft, 0, ft_size * bsize);
	ft->file_system_type = FS_TYPE;
	strcpy(ft->partition_name, disk_device);
	ft->block_size = bsize;
	ft->blocks = blocks;
	ft->max_files = MAXFILESONDISK;

	int i;
	for (i = ft_size; i < ft->blocks; i++)
		ft->free[i] = 1;

	DISK_WRITE(ft, ft_size, 0);

	list_init(&open_files);

	last_check = NULL;

	return 0;
}

int k_fs_is_file_open(descriptor_t *desc)
{
	kobject_t *kobj;

	kobj = desc->ptr;
	if (kobj && list_find(&kobjects, &kobj->list) &&
		(kobj->flags & KTYPE_FILE) != 0)
	{
		struct kfile_desc *fd = kobj->kobject;
		if (fd && fd->id == desc->id &&
			list_find(&open_files, &fd->list))
		{
			last_check = fd;
			return 0;
		}
	}

	return -1;
}

int k_fs_open_file(char *pathname, int flags, mode_t mode, descriptor_t *desc)
{
	struct fs_node *tfd = NULL;
	char *fname = &pathname[5];

	//check for conflicting flags
	//if (	((flags & O_RDONLY) && (flags & O_WRONLY)) ||
	//	((flags & O_RDONLY) && (flags & O_RDWR))   ||
	//	((flags & O_WRONLY) && (flags & O_RDWR))
	//) {
     //   kprintf("ret wring flags");
	//	return -EINVAL;}

	//check if file already open
	struct kfile_desc *fd = list_get(&open_files, FIRST);
	while (fd != NULL) {
		if (strcmp(fd->tfd->node_name, fname) == 0) {
			//already open!
			//if its open for reading and O_RDONLY is set in flags
			if ((fd->flags & O_RDONLY) == (flags & O_RDONLY))
				tfd = fd->tfd;//fine, save pointer to descriptor
			else {
				return -EADDRINUSE; //not fine
                
                }

		}
		fd = list_get_next(&fd->list);
	}

	if (!tfd) {
		//not already open; check if such file exists in file table
		int i;
		for (i = 0; i < ft->max_files; i++) {
			if (strcmp(ft->fd[i].node_name, fname) == 0) {
				tfd = &ft->fd[i];
				break;
			}
		}
	}

	if (!tfd) {
		//file doesn't exitst
		if ((flags & (O_CREAT | O_WRONLY)) == 0)
			return -EINVAL;

		//create fs_node in fs_table
		//1. find unused descriptor
		int i;
		for (i = 0; i < ft->max_files; i++) {
			if (ft->fd[i].node_name[0] == 0) {
				tfd = &ft->fd[i];
				break;
			}
		}
		if (!tfd)
			return -ENFILE;

		//2. initialize descriptor
		strcpy(tfd->node_name, fname);
		tfd->id = i;
		timespec_t t;
		kclock_gettime (CLOCK_REALTIME, &t);
		tfd->tc = tfd->ta = tfd->tm = t;
		tfd->size = 0;
		tfd->blocks = 0;
	}

	//create kobject and a new struct kfile_desc
	kobject_t *kobj = kmalloc_kobject(sizeof(struct kfile_desc));
	kobj->flags = KTYPE_FILE;
	fd = kobj->kobject;
	fd->tfd = tfd;
	fd->flags = flags;
	fd->fp = 0;
	fd->id = k_new_id ();
	list_append(&open_files, fd, &fd->list);

	//fill desc
	desc->id = fd->id;
	desc->ptr = kobj;

	return 0;
}

int k_fs_close_file(descriptor_t *desc)
{
	struct kfile_desc *fd = last_check;
	kobject_t *kobj;

	kobj = desc->ptr;
	/* - already tested!
	if (!kobj || !list_find(&kobjects, &kobj->list))
		return -EINVAL;

	fd = kobj->kobject;
	if (!fd || fd->id != desc->id)
		return -EINVAL;
	*/
	if (!list_find_and_remove(&open_files, &fd->list))
		return -EINVAL;

	kfree_kobject ( kobj );

	return 0;
}

//op = 0 => write, otherwise =>read
int k_fs_read_write(descriptor_t *desc, void *buffer, size_t size, int op)
{
	//desc already checked, use "last_check";
	struct kfile_desc *fd = last_check;

	//sanity check
	if ((op && (fd->flags & O_WRONLY)) || (!op && (fd->flags & O_RDONLY)))
		return -EPERM;

	if (op) { //read
		//read from offset "fd->fp" to "buffer" "size" bytes
        //reading more than in file
        int file_size = fd->tfd->size;
        if(fd->fp + size > file_size) {
            size = file_size - fd->fp; //So we don't read past EOF!
        }

        //don't read anything
        if(size <= 0) {
            return 0;
        }

        //we are going to read, let's assume it doesn't take more than a second
        //otherwise I'd have to do it before each return
        timespec_t t;
		kclock_gettime (CLOCK_REALTIME, &t);
        fd->tfd->ta = t;

        //read from fp to nearest block boundary (if the fp is in a block )
        int offset_into_output = 0; //where in buffer (arg of this fn) to write next
        int offset = fd->fp;

        int next_block_to_read = offset/ft->block_size;

        if ( offset % ft->block_size != 0 ) {
            int remaining_util_boundary = ft->block_size - offset % ft->block_size;
            char buffer_block[ft->block_size];
            
            //The end is near :) All the desired data is in this block
            if(remaining_util_boundary > size) {               
                DISK_READ(buffer_block, 1, fd->tfd->block[next_block_to_read]);
                memcpy(buffer, buffer_block + offset % ft->block_size, size);
                fd->fp += size;
                return size;
            } else { //Read until end of block
                DISK_READ(buffer_block, 1, fd->tfd->block[next_block_to_read]);
                memcpy(buffer, buffer_block + offset % ft->block_size, remaining_util_boundary);
                offset_into_output += remaining_util_boundary;
                next_block_to_read++;
            }
        }

        //now, we read full blocks until remaining size is less than a full block
        int remaining_size = size - offset_into_output;
        int full_blocks = remaining_size / ft->block_size;
        if(full_blocks > 0) {
            DISK_READ(buffer + offset_into_output, full_blocks, next_block_to_read);
            next_block_to_read += full_blocks;
            offset_into_output += full_blocks * (ft->block_size);
            remaining_size -= full_blocks * (ft->block_size);
        }

        //shouldn't be < 0, but still...
        if(remaining_size <= 0) {
            fd->fp += size;
            return size;
        }

        //Read final block until boundary
        char block_buffer[ft->block_size];
        DISK_READ(block_buffer, 1, next_block_to_read);
        memcpy(buffer + offset_into_output, block_buffer, remaining_size);

        fd->fp += size;
        return size;
	}
	else {
		//assume there is enough space on disk

		//write ...
		//if ...->block[x] == 0 => find free block on disk
		//when fp isn't block start, read block from disk first
		//and then replace fp+ bytes ... and then write block back

		char buf[ft->block_size];
		size_t todo = size;
		size_t maxfilesize = ft->block_size * MAXFILEBLOCKS;

        if(fd->fp + todo >= maxfilesize) {
            return -EINVAL;
        }
		
        //This is way slower (only one block at most is read), but edge cases are easier
        //& the code is cleaner
        while(todo > 0) {

            size_t block = fd->fp / ft->block_size;
            
            //can't write into desired block
            if(fd->tfd->block[block] == 0) {
                int j;
                for(j = 0; j < ft->blocks; ++j) {
                    if(ft->free[j] == 1) {
                        ft->free[j] = 0;
                        fd->tfd->blocks++;
                        fd->tfd->block[block] = j;
                    }
                }
            }
            
            size_t offset_from_block_start = fd->fp % ft->block_size;
            //always read entire block
            DISK_READ(buf, 1, block);
            size_t first_to_write = size - todo;
            //how much do we need to write?
            size_t to_write = todo > ft->block_size ? ft->block_size : todo;
            
            memcpy(buf + offset_from_block_start, buffer + first_to_write, to_write);
            DISK_WRITE(buf, 1, block);

            block++;
            fd->fp += to_write;
            fd->tfd->size = fd->fp >= fd->tfd->size ? fd->fp : fd->tfd->size;
            todo -= to_write;
        }

        

        //todo ...
        timespec_t t;
		kclock_gettime (CLOCK_REALTIME, &t);
        fd->tfd->ta = fd->tfd->tm = t;

		return size - todo;
	}

	return 0;
}
