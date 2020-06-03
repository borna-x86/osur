/*! Dynamic memory allocator - first fit (simple)
 *
 * In this implementation double linked list are used.
 * Free list is not sorted. Search is started from first element until chunk
 * with adequate size is found (same or greater than required).
 * When chunk is freed, first join is tried with left and right neighbor chunk
 * (by address). If not joined, chunk is marked as free and put at list start.
 */

#pragma once


#include <types/basic.h>
#include <lib/ff_simple.h>

#ifndef _FF_SIMPLE3_C_

typedef void ffs3_mpool_t;

/*! interface */
void *ffs3_init ( void *mem_segm, size_t size );
void *ffs3_alloc ( ffs3_mpool_t *mpool, size_t size );
int ffs3_free ( ffs3_mpool_t *mpool, void *chunk_to_be_freed );

/*! rest is only for first_fit.c */
#else /* _FF_SIMPLE_C_ */

/* free chunk header (in use chunk header is just 'size') */
typedef struct _ffs3_hdr_t_
{
	size_t		     size;
			     /* chunk size, including head and tail headers */
	struct _ffs3_hdr_t_  *prev;
			     /* previous free in list */
	struct _ffs3_hdr_t_  *next;
			     /* next free in list */
}
ffs3_hdr_t;

/* chunk tail (and header for in use chunks) */
typedef struct _ffs3_tail_t_
{
	size_t  size;
		/* chunk size, including head and tail headers */
}
ffs3_tail_t;

typedef struct _ffs3_mpool_t_
{
	ffs_mpool_t *first;
	ffs_mpool_t *second;
	ffs_mpool_t *third;
}
ffs3_mpool_t;

 #define ALIGN_VAL	( (size_t) sizeof(size_t) )
 #define ALIGN_MASK	( ~( ALIGN_VAL - 1 ) )
 #define ALIGN(P)	\
	do { (P) = ALIGN_MASK & ( (size_t) (P) ); } while(0)
 #define ALIGN_FW(P)	\
 	do { (P) = ALIGN_MASK & (((size_t) (P)) + (ALIGN_VAL - 1)) ; } while(0)

void *ffs3_init ( void *mem_segm, size_t size );
void *ffs3_alloc ( ffs3_mpool_t *mpool, size_t size );
int ffs3_free ( ffs3_mpool_t *mpool, void *chunk_to_be_freed );

#endif /* _FF_SIMPLE_C_ */
