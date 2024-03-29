/* Copyright (c) 2008, Sandia National Laboratories */

#include <lwk/liblwk.h>

const char *
pmem_type_to_string(pmem_type_t type)
{
	switch(type) {
		case PMEM_TYPE_BOOTMEM:     return "BOOTMEM";     break;
		case PMEM_TYPE_BIGPHYSAREA: return "BIGPHYSAREA"; break;
		case PMEM_TYPE_INITRD:      return "INITRD";      break;
		case PMEM_TYPE_INIT_TASK:   return "INIT_TASK";   break;
		case PMEM_TYPE_KMEM:        return "KMEM";        break;
		case PMEM_TYPE_UMEM:        return "UMEM";        break;
	}
	return "UNKNOWN";
}

void
pmem_region_unset_all(struct pmem_region *rgn)
{
	rgn->type_is_set      = false;
	rgn->numa_node_is_set = false;
	rgn->allocated_is_set = false;
	rgn->name_is_set      = false;
}

int
pmem_alloc_umem(size_t size, size_t alignment, struct pmem_region *rgn)
{
	struct pmem_region constraint, result;

	/* Find and allocate a chunk of PMEM_TYPE_UMEM physical memory */
	pmem_region_unset_all(&constraint);
	constraint.start     = 0;
	constraint.end       = (paddr_t)(-1);
	constraint.type      = PMEM_TYPE_UMEM; constraint.type_is_set = true;
	constraint.allocated = false;          constraint.allocated_is_set = true;

	if (pmem_alloc(size, alignment, &constraint, &result))
		return -ENOMEM;

	*rgn = result;
	return 0;
}

bool
pmem_is_type(pmem_type_t type, paddr_t start, size_t extent)
{
	struct pmem_region query, result;
	int status;

	pmem_region_unset_all(&query);
	query.start = start;
	query.end   = start + extent;
	query.type  = type; query.type_is_set = true; 
	result.end  = 0;

	while ((status = pmem_query(&query, &result)) == 0) {
		if (result.start != query.start)
			return false;
		if (result.end == query.end)
			break;
		query.start = result.end;
	}
	return (status) ? false : true;
}
