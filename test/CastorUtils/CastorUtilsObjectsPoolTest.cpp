#include "CastorUtilsObjectsPoolTest.hpp"

#include "TestObjectPool.hpp"

#include <Pool/PoolManagedObject.hpp>
#include <Pool/ObjectPool.hpp>

#include <algorithm>

namespace Testing
{
	namespace Memory
	{
		template< castor::MemoryDataType MemDataType >
		using MyObjectPool = castor::ObjectPool< Obj, MemDataType >;

		template< castor::MemoryDataType MemDataType >
		using MyAlignedObjectPool = castor::AlignedObjectPool< Obj, MemDataType, 16 >;

		template< castor::MemoryDataType MemDataType >
		void Checks()
		{
			std::cout << "********************************************************************************" << std::endl << std::endl;
			std::cout << "Memory checks" << std::endl;
			std::cout << " Policy: " << castor::MemoryDataNamer< MemDataType >::Name << "< Ptr >" << std::endl << std::endl;
			{
				MyObjectPool< MemDataType > pool( 1 );
				Obj * bigObjs[2];

				for ( size_t i = 0; i < 2; ++i )
				{
					bigObjs[i] = pool.allocate();
				}

				for ( size_t i = 0; i < 2; ++i )
				{
					pool.deallocate( bigObjs[i] );
				}

				pool.deallocate( bigObjs[0] );
			}
			{
				MyObjectPool< MemDataType > pool( 1 );
				Obj * bigObj;
				bigObj = pool.allocate();
			}
			{
				MyObjectPool< MemDataType > pool( 1 );
				Obj obj;
				pool.deallocate( &obj );
			}

			std::cout << std::endl;
			std::cout << "********************************************************************************" << std::endl;
		}

		template< castor::MemoryDataType MemDataType >
		void AlignedChecks()
		{
			std::cout << "********************************************************************************" << std::endl << std::endl;
			std::cout << "Memory checks" << std::endl;
			std::cout << " Policy: " << castor::MemoryDataNamer< MemDataType >::Name << "< Ptr >" << std::endl << std::endl;
			{
				MyAlignedObjectPool< MemDataType > pool( 1 );
				Obj * bigObjs[2];

				for ( size_t i = 0; i < 2; ++i )
				{
					bigObjs[i] = pool.allocate();
				}

				for ( size_t i = 0; i < 2; ++i )
				{
					pool.deallocate( bigObjs[i] );
				}

				pool.deallocate( bigObjs[0] );
			}
			{
				MyAlignedObjectPool< MemDataType > pool( 1 );
				Obj * bigObj;
				bigObj = pool.allocate();
			}
			{
				MyAlignedObjectPool< MemDataType > pool( 1 );
				Obj obj;
				pool.deallocate( &obj );
			}

			std::cout << std::endl;
			std::cout << "********************************************************************************" << std::endl;
		}
	}

	//*********************************************************************************************

	CastorUtilsObjectsPoolTest::CastorUtilsObjectsPoolTest()
		: TestCase( "CastorUtilsObjectsPoolTest" )
	{
	}

	CastorUtilsObjectsPoolTest::~CastorUtilsObjectsPoolTest()
	{
	}

	void CastorUtilsObjectsPoolTest::doRegisterTests()
	{
		doRegisterTest( "ObjectPoolTest", std::bind( &CastorUtilsObjectsPoolTest::ObjectPoolTest, this ) );
		//doRegisterTest( "AlignedObjectPoolTest", std::bind( &CastorUtilsObjectsPoolTest::AlignedObjectPoolTest, this ) );
		doRegisterTest( "FixedSizePerformanceTest", std::bind( &CastorUtilsObjectsPoolTest::FixedSizePerformanceTest, this ) );
		doRegisterTest( "ScatteredMemoryPerformanceTest", std::bind( &CastorUtilsObjectsPoolTest::ScatteredMemoryPerformanceTest, this ) );
		doRegisterTest( "VariableSizePerformanceTest", std::bind( &CastorUtilsObjectsPoolTest::VariableSizePerformanceTest, this ) );
		doRegisterTest( "UniquePoolTest", std::bind( &CastorUtilsObjectsPoolTest::UniquePoolTest, this ) );
	}

	void CastorUtilsObjectsPoolTest::ObjectPoolTest()
	{
		Memory::Checks< castor::MemoryDataType::eFixed >();
		Memory::Checks< castor::MemoryDataType::eMarked >();
		Memory::Checks< castor::MemoryDataType::eFixedGrowing >();
		Memory::Checks< castor::MemoryDataType::eFixedGrowingMarked >();
	}

	void CastorUtilsObjectsPoolTest::AlignedObjectPoolTest()
	{
		Memory::AlignedChecks< castor::MemoryDataType::eFixed >();
		Memory::AlignedChecks< castor::MemoryDataType::eFixedGrowing >();
	}

	void CastorUtilsObjectsPoolTest::FixedSizePerformanceTest()
	{
		FixedSizePerformance::Checks< PlacementNew::SFixedChecks< AllocPolicies::SPlacementNewPolicy > >();
		FixedSizePerformance::Checks< castor::MemoryDataType::eFixed >();
		FixedSizePerformance::Checks< castor::MemoryDataType::eMarked >();
		FixedSizePerformance::Checks< castor::MemoryDataType::eFixedGrowing >();
		FixedSizePerformance::Checks< castor::MemoryDataType::eFixedGrowingMarked >();
		FixedSizePerformance::Checks< Traditional::SFixedChecks< AllocPolicies::SNewDeletePolicy > >();
		FixedSizePerformance::Checks< Traditional::SFixedChecks< AllocPolicies::SMallocFreePolicy > >();
	}

	void CastorUtilsObjectsPoolTest::AllDeallPerformanceTest()
	{
		AllDeallPerformance::Checks< PlacementNew::SAllDeallChecks< AllocPolicies::SPlacementNewPolicy > >();
		AllDeallPerformance::Checks< castor::MemoryDataType::eFixed >();
		AllDeallPerformance::Checks< castor::MemoryDataType::eMarked >();
		AllDeallPerformance::Checks< castor::MemoryDataType::eFixedGrowing >();
		AllDeallPerformance::Checks< castor::MemoryDataType::eFixedGrowingMarked >();
		AllDeallPerformance::Checks< Traditional::SAllDeallChecks< AllocPolicies::SNewDeletePolicy > >();
		AllDeallPerformance::Checks< Traditional::SVariableChecks< AllocPolicies::SMallocFreePolicy > >();
	}

	void CastorUtilsObjectsPoolTest::ScatteredMemoryPerformanceTest()
	{
		ScatteredMemoryPerformance::Index index;
		ScatteredMemoryPerformance::Checks< PlacementNew::SScatteredChecks< AllocPolicies::SPlacementNewPolicy > >( index );
		ScatteredMemoryPerformance::Checks< castor::MemoryDataType::eFixed >( index );
		ScatteredMemoryPerformance::Checks< castor::MemoryDataType::eMarked >( index );
		ScatteredMemoryPerformance::Checks< castor::MemoryDataType::eFixedGrowing >( index );
		ScatteredMemoryPerformance::Checks< castor::MemoryDataType::eFixedGrowingMarked >( index );
		ScatteredMemoryPerformance::Checks< Traditional::SScatteredChecks< AllocPolicies::SNewDeletePolicy > >( index );
		ScatteredMemoryPerformance::Checks< Traditional::SScatteredChecks< AllocPolicies::SMallocFreePolicy > >( index );
	}

	void CastorUtilsObjectsPoolTest::VariableSizePerformanceTest()
	{
		VariableSizePerformance::Checks< PlacementNew::SVariableChecks< AllocPolicies::SPlacementNewPolicy > >();
		VariableSizePerformance::Checks< castor::MemoryDataType::eFixedGrowing >();
		VariableSizePerformance::Checks< castor::MemoryDataType::eFixedGrowingMarked >();
		VariableSizePerformance::Checks< Traditional::SVariableChecks< AllocPolicies::SNewDeletePolicy > >();
		VariableSizePerformance::Checks< Traditional::SVariableChecks< AllocPolicies::SMallocFreePolicy > >();
	}

	void CastorUtilsObjectsPoolTest::UniquePoolTest()
	{
		UniqueObjectPool::Checks< Traditional::SFixedChecks< AllocPolicies::SNewDeletePolicy > >();
		UniqueObjectPool::Checks< Traditional::SFixedChecks< AllocPolicies::SMallocFreePolicy > >();
		UniqueObjectPool::Checks< castor::MemoryDataType::eFixed >();
		UniqueObjectPool::Checks< castor::MemoryDataType::eFixedGrowing >();
	}
}
