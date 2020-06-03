/*!  Dynamic memory allocator - first fit */

#define _FF_SIMPLE3_C_
#include <lib/ff_simple3.h>
#include <lib/ff_simple.h>
#include <kernel/kprint.h>

#ifndef ASSERT
#include ASSERT_H
#endif

/*!
 * Initialize dynamic memory manager
 * \param mem_segm Memory pool start address
 * \param size Memory pool size
 * \return memory pool descriptor
*/
void *ffs3_init ( void *mem_segm, size_t size )
{
		
	size_t start, end;
	ffs3_mpool_t *mpool;

	ASSERT ( mem_segm && size > sizeof (ffs3_hdr_t) * 3 );

	kprintf("Initializing memory subsytem at %x\n", mem_segm);

	/* align all on 'size_t' (if already not aligned) */
	start = (size_t) mem_segm;
	end = start + size;
	ALIGN_FW ( start );

	mpool = (void *) start;		/* place mm descriptor here */
	start += sizeof (ffs3_mpool_t);
	ALIGN ( end );

	// 3 segments
	size_t seg_size = (end - start) / 3 - sizeof(size_t); // So the chunks
		//don't overlap if there is misalignment!
	size_t start1 = start;
	size_t start2 = seg_size + start1;
	size_t start3 = 2*seg_size + start1;

	mpool = (ffs3_mpool_t*) mpool;
	mpool->first = ffs_init((void*)start1, seg_size);
	mpool->second = ffs_init((void*)start2, seg_size);
	mpool->third = ffs_init((void*)start3, seg_size);

	return (void*)mpool;
}

/*!
 * Get free chunk with required size (or slightly bigger)
 * \param mpool Memory pool to be used (if NULL default pool is used)
 * \param size Requested chunk size
 * \return Block address, NULL if can't find adequate free chunk
 */
void *ffs3_alloc ( ffs3_mpool_t *mpool, size_t size )
{
	if(size <= 64) {
		kprintf("allocating small chunk\n");
		return ffs_alloc(mpool->first, size);
	} else if (size > 64 && size <= 512) {
		kprintf("allocating medium chunk\n");
		return ffs_alloc(mpool->second, size);
	} else {
		kprintf("allocating large chunk\n");
		return ffs_alloc(mpool->third, size);
	}
}

/*!
 * Free memory chunk
 * \param mpool Memory pool to be used (if NULL default pool is used)
 * \param chunk Chunk location (starting address)
 * \return 0 if successful, -1 otherwise
 */
int ffs3_free ( ffs3_mpool_t *mpool, void *chunk_to_be_freed )
{
	if(chunk_to_be_freed >= (void*)(mpool->first) &&
			chunk_to_be_freed < (void*)(mpool->second)) {
				kprintf("freeing small chunk\n");
				return ffs_free(mpool->first, chunk_to_be_freed);
			}

	else if(chunk_to_be_freed >= (void*) (mpool->second) &&
			chunk_to_be_freed < (void*)(mpool->third)) {
				kprintf("freeing medium chunk\n");
				return ffs_free(mpool->second, chunk_to_be_freed);
			}
	else {
		kprintf("freeing large chunk\n");
		return ffs_free(mpool->third, chunk_to_be_freed);
	}
}