#include "PoolException.hpp"

#include "MemoryDataTyper.hpp"

namespace Castor
{
	char const * const Error< ePOOL_ERROR_TYPE_COMMON_OUT_OF_MEMORY >::Text = "Pool is out of memory";
	char const * const Error< ePOOL_ERROR_TYPE_COMMON_POOL_IS_FULL >::Text = "The object does not come from the pool (pool is full)";
	char const * const Error< ePOOL_ERROR_TYPE_COMMON_MEMORY_LEAKS_DETECTED >::Text = "Memory leaks detected";
	char const * const Error< ePOOL_ERROR_TYPE_COMMON_NOT_FROM_RANGE >::Text = "Object does not come from memory range";
	char const * const Error< ePOOL_ERROR_TYPE_MARKED_LEAK_ADDRESS >::Text = "At address";
	char const * const Error< ePOOL_ERROR_TYPE_MARKED_DOUBLE_DELETE >::Text = "Object was already deleted by the pool";
	char const * const Error< ePOOL_ERROR_TYPE_MARKED_NOT_FROM_POOL >::Text = "Object was not allocated by the pool";
	char const * const Error< ePOOL_ERROR_TYPE_GROWING_NOT_FROM_RANGES >::Text = "Object does not come from the pool memory ranges";
	char const * const Error< ePOOL_ERROR_TYPE_STL_ALLOCATOR_UNIQUE >::Text = "Unique instance already created";

	template<> char const * const MemoryDataNamer< eMEMDATA_TYPE_FIXED >::Name = "FixedSizeMemoryData";
	template<> char const * const MemoryDataNamer< eMEMDATA_TYPE_FIXED_MARKED >::Name = "FixedSizeMarkedMemoryData";
	template<> char const * const MemoryDataNamer< eMEMDATA_TYPE_FIXED_GROWING >::Name = "FixedGrowingSizeMemoryData";
	template<> char const * const MemoryDataNamer< eMEMDATA_TYPE_FIXED_GROWING_MARKED >::Name = "FixedGrowingSizeMarkedMemoryData";
}
