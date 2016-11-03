#include "CastorUtilsObjectsPoolTest.hpp"

#include "TestObjectPool.hpp"

#include <Pool/PoolManagedObject.hpp>
#include <Pool/ObjectPool.hpp>

#include <algorithm>

namespace Testing
{
	namespace Memory
	{
		template< Castor::MemoryDataType MemDataType >
		using MyObjectPool = Castor::ObjectPool< Obj, MemDataType >;

		template< Castor::MemoryDataType MemDataType >
		using MyAlignedObjectPool = Castor::AlignedObjectPool< Obj, MemDataType, 16 >;

		template< Castor::MemoryDataType MemDataType >
		void Checks()
		{
			std::cout << "********************************************************************************" << std::endl << std::endl;
			std::cout << "Memory checks" << std::endl;
			std::cout << " Policy: " << Castor::MemoryDataNamer< MemDataType >::Name << "< Ptr >" << std::endl << std::endl;
			{
				MyObjectPool< MemDataType > pool( 1 );
				Obj * bigObjs[2];

				for ( size_t i = 0; i < 2; ++i )
				{
					bigObjs[i] = pool.Allocate();
				}

				for ( size_t i = 0; i < 2; ++i )
				{
					pool.Deallocate( bigObjs[i] );
				}

				pool.Deallocate( bigObjs[0] );
			}
			{
				MyObjectPool< MemDataType > pool( 1 );
				Obj * bigObj;
				bigObj = pool.Allocate();
			}
			{
				MyObjectPool< MemDataType > pool( 1 );
				Obj obj;
				pool.Deallocate( &obj );
			}

			std::cout << std::endl;
			std::cout << "********************************************************************************" << std::endl;
		}

		template< Castor::MemoryDataType MemDataType >
		void AlignedChecks()
		{
			std::cout << "********************************************************************************" << std::endl << std::endl;
			std::cout << "Memory checks" << std::endl;
			std::cout << " Policy: " << Castor::MemoryDataNamer< MemDataType >::Name << "< Ptr >" << std::endl << std::endl;
			{
				MyAlignedObjectPool< MemDataType > pool( 1 );
				Obj * bigObjs[2];

				for ( size_t i = 0; i < 2; ++i )
				{
					bigObjs[i] = pool.Allocate();
				}

				for ( size_t i = 0; i < 2; ++i )
				{
					pool.Deallocate( bigObjs[i] );
				}

				pool.Deallocate( bigObjs[0] );
			}
			{
				MyAlignedObjectPool< MemDataType > pool( 1 );
				Obj * bigObj;
				bigObj = pool.Allocate();
			}
			{
				MyAlignedObjectPool< MemDataType > pool( 1 );
				Obj obj;
				pool.Deallocate( &obj );
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

	void CastorUtilsObjectsPoolTest::DoRegisterTests()
	{
		DoRegisterTest( "ObjectPoolTest", std::bind( &CastorUtilsObjectsPoolTest::ObjectPoolTest, this ) );
		DoRegisterTest( "AlignedObjectPoolTest", std::bind( &CastorUtilsObjectsPoolTest::AlignedObjectPoolTest, this ) );
		DoRegisterTest( "FixedSizePerformanceTest", std::bind( &CastorUtilsObjectsPoolTest::FixedSizePerformanceTest, this ) );
		DoRegisterTest( "ScatteredMemoryPerformanceTest", std::bind( &CastorUtilsObjectsPoolTest::ScatteredMemoryPerformanceTest, this ) );
		DoRegisterTest( "VariableSizePerformanceTest", std::bind( &CastorUtilsObjectsPoolTest::VariableSizePerformanceTest, this ) );
		DoRegisterTest( "UniquePoolTest", std::bind( &CastorUtilsObjectsPoolTest::UniquePoolTest, this ) );
	}

	void CastorUtilsObjectsPoolTest::ObjectPoolTest()
	{
		Memory::Checks< Castor::MemoryDataType::eFixed >();
		Memory::Checks< Castor::MemoryDataType::eMarked >();
		Memory::Checks< Castor::MemoryDataType::eFixedGrowing >();
		Memory::Checks< Castor::MemoryDataType::eFixedGrowingMarked >();
	}

	void CastorUtilsObjectsPoolTest::AlignedObjectPoolTest()
	{
		Memory::AlignedChecks< Castor::MemoryDataType::eFixed >();
		Memory::AlignedChecks< Castor::MemoryDataType::eFixedGrowing >();
	}

	void CastorUtilsObjectsPoolTest::FixedSizePerformanceTest()
	{
		FixedSizePerformance::Checks< PlacementNew::SFixedChecks< AllocPolicies::SPlacementNewPolicy > >();
		FixedSizePerformance::Checks< Castor::MemoryDataType::eFixed >();
		FixedSizePerformance::Checks< Castor::MemoryDataType::eMarked >();
		FixedSizePerformance::Checks< Castor::MemoryDataType::eFixedGrowing >();
		FixedSizePerformance::Checks< Castor::MemoryDataType::eFixedGrowingMarked >();
		FixedSizePerformance::Checks< Traditional::SFixedChecks< AllocPolicies::SNewDeletePolicy > >();
		FixedSizePerformance::Checks< Traditional::SFixedChecks< AllocPolicies::SMallocFreePolicy > >();
	}

	void CastorUtilsObjectsPoolTest::AllDeallPerformanceTest()
	{
		AllDeallPerformance::Checks< PlacementNew::SAllDeallChecks< AllocPolicies::SPlacementNewPolicy > >();
		AllDeallPerformance::Checks< Castor::MemoryDataType::eFixed >();
		AllDeallPerformance::Checks< Castor::MemoryDataType::eMarked >();
		AllDeallPerformance::Checks< Castor::MemoryDataType::eFixedGrowing >();
		AllDeallPerformance::Checks< Castor::MemoryDataType::eFixedGrowingMarked >();
		AllDeallPerformance::Checks< Traditional::SAllDeallChecks< AllocPolicies::SNewDeletePolicy > >();
		AllDeallPerformance::Checks< Traditional::SVariableChecks< AllocPolicies::SMallocFreePolicy > >();
	}

	void CastorUtilsObjectsPoolTest::ScatteredMemoryPerformanceTest()
	{
		ScatteredMemoryPerformance::Index index;
		ScatteredMemoryPerformance::Checks< PlacementNew::SScatteredChecks< AllocPolicies::SPlacementNewPolicy > >( index );
		ScatteredMemoryPerformance::Checks< Castor::MemoryDataType::eFixed >( index );
		ScatteredMemoryPerformance::Checks< Castor::MemoryDataType::eMarked >( index );
		ScatteredMemoryPerformance::Checks< Castor::MemoryDataType::eFixedGrowing >( index );
		ScatteredMemoryPerformance::Checks< Castor::MemoryDataType::eFixedGrowingMarked >( index );
		ScatteredMemoryPerformance::Checks< Traditional::SScatteredChecks< AllocPolicies::SNewDeletePolicy > >( index );
		ScatteredMemoryPerformance::Checks< Traditional::SScatteredChecks< AllocPolicies::SMallocFreePolicy > >( index );
	}

	void CastorUtilsObjectsPoolTest::VariableSizePerformanceTest()
	{
		VariableSizePerformance::Checks< PlacementNew::SVariableChecks< AllocPolicies::SPlacementNewPolicy > >();
		VariableSizePerformance::Checks< Castor::MemoryDataType::eFixedGrowing >();
		VariableSizePerformance::Checks< Castor::MemoryDataType::eFixedGrowingMarked >();
		VariableSizePerformance::Checks< Traditional::SVariableChecks< AllocPolicies::SNewDeletePolicy > >();
		VariableSizePerformance::Checks< Traditional::SVariableChecks< AllocPolicies::SMallocFreePolicy > >();
	}

	void CastorUtilsObjectsPoolTest::UniquePoolTest()
	{
		UniqueObjectPool::Checks< Traditional::SFixedChecks< AllocPolicies::SNewDeletePolicy > >();
		UniqueObjectPool::Checks< Traditional::SFixedChecks< AllocPolicies::SMallocFreePolicy > >();
		UniqueObjectPool::Checks< Castor::MemoryDataType::eFixed >();
		UniqueObjectPool::Checks< Castor::MemoryDataType::eFixedGrowing >();
	}
}
