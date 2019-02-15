#include "PoolException.hpp"

#include "MemoryDataTyper.hpp"

namespace castor
{
	char const * const Error< PoolErrorType::eCommonOutOfMemory >::Text = "Pool is out of memory";
	char const * const Error< PoolErrorType::eCommonPoolIsFull >::Text = "The object does not come from the pool (pool is full)";
	char const * const Error< PoolErrorType::eCommonMemoryLeaksDetected >::Text = "Memory leaks detected";
	char const * const Error< PoolErrorType::eCommonNotFromRange >::Text = "Object does not come from memory range";
	char const * const Error< PoolErrorType::eMarkedLeakAddress >::Text = "At address";
	char const * const Error< PoolErrorType::eMarkedDoubleDelete >::Text = "Object was already deleted by the pool";
	char const * const Error< PoolErrorType::eMarkedNotFromPool >::Text = "Object was not allocated by the pool";
	char const * const Error< PoolErrorType::eGrowingNotFromRanges >::Text = "Object does not come from the pool memory ranges";
	char const * const Error< PoolErrorType::eSTLAllocatorUnique >::Text = "Unique instance already created";

	template<> char const * const MemoryDataNamer< MemoryDataType::eFixed >::Name = "FixedSizeMemoryData";
	template<> char const * const MemoryDataNamer< MemoryDataType::eMarked >::Name = "FixedSizeMarkedMemoryData";
	template<> char const * const MemoryDataNamer< MemoryDataType::eFixedGrowing >::Name = "FixedGrowingSizeMemoryData";
	template<> char const * const MemoryDataNamer< MemoryDataType::eFixedGrowingMarked >::Name = "FixedGrowingSizeMarkedMemoryData";
}
