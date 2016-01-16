// #include "CastorUtilsObjectsPoolTest.hpp"

// #include "TestObjectPool.hpp"

// #include <PoolManagedObject.hpp>
// #include <ObjectPool.hpp>

// #include <algorithm>

// namespace Testing
// {
	// namespace Memory
	// {
		// template< Castor::eMEMDATA_TYPE MemDataType >
		// using MyObjectPool = Castor::ObjectPool< Obj, MemDataType >;

		// template< Castor::eMEMDATA_TYPE MemDataType >
		// using MyAlignedObjectPool = Castor::AlignedObjectPool< Obj, MemDataType, 16 >;

		// template< Castor::eMEMDATA_TYPE MemDataType >
		// void Checks()
		// {
			// std::cout << "********************************************************************************" << std::endl << std::endl;
			// std::cout << "Memory checks" << std::endl;
			// std::cout << " Policy: " << Castor::MemoryDataNamer< MemDataType >::Name << "< Ptr >" << std::endl << std::endl;
			// {
				// MyObjectPool< MemDataType > pool( 1 );
				// Obj * bigObjs[2];

				// for ( size_t i = 0; i < 2; ++i )
				// {
					// bigObjs[i] = pool.Allocate();
				// }

				// for ( size_t i = 0; i < 2; ++i )
				// {
					// pool.Deallocate( bigObjs[i] );
				// }

				// pool.Deallocate( bigObjs[0] );
			// }
			// {
				// MyObjectPool< MemDataType > pool( 1 );
				// Obj * bigObj;
				// bigObj = pool.Allocate();
			// }
			// {
				// MyObjectPool< MemDataType > pool( 1 );
				// Obj obj;
				// pool.Deallocate( &obj );
			// }

			// std::cout << std::endl;
			// std::cout << "********************************************************************************" << std::endl;
		// }

		// template< Castor::eMEMDATA_TYPE MemDataType >
		// void AlignedChecks()
		// {
			// std::cout << "********************************************************************************" << std::endl << std::endl;
			// std::cout << "Memory checks" << std::endl;
			// std::cout << " Policy: " << Castor::MemoryDataNamer< MemDataType >::Name << "< Ptr >" << std::endl << std::endl;
			// {
				// MyAlignedObjectPool< MemDataType > pool( 1 );
				// Obj * bigObjs[2];

				// for ( size_t i = 0; i < 2; ++i )
				// {
					// bigObjs[i] = pool.Allocate();
				// }

				// for ( size_t i = 0; i < 2; ++i )
				// {
					// pool.Deallocate( bigObjs[i] );
				// }

				// pool.Deallocate( bigObjs[0] );
			// }
			// {
				// MyAlignedObjectPool< MemDataType > pool( 1 );
				// Obj * bigObj;
				// bigObj = pool.Allocate();
			// }
			// {
				// MyAlignedObjectPool< MemDataType > pool( 1 );
				// Obj obj;
				// pool.Deallocate( &obj );
			// }

			// std::cout << std::endl;
			// std::cout << "********************************************************************************" << std::endl;
		// }
	// }

	*********************************************************************************************

	// CastorUtilsObjectsPoolTest::CastorUtilsObjectsPoolTest()
		// : TestCase( "CastorUtilsObjectsPoolTest" )
	// {
	// }

	// CastorUtilsObjectsPoolTest::~CastorUtilsObjectsPoolTest()
	// {
	// }

	// void CastorUtilsObjectsPoolTest::Execute( uint32_t & p_errCount, uint32_t & p_testCount )
	// {
		// EXECUTE_TEST( CastorUtilsObjectsPoolTest, ObjectPoolTest, p_errCount, p_testCount );
		// EXECUTE_TEST( CastorUtilsObjectsPoolTest, AlignedObjectPoolTest, p_errCount, p_testCount );
		// EXECUTE_TEST( CastorUtilsObjectsPoolTest, FixedSizePerformanceTest, p_errCount, p_testCount );
		// EXECUTE_TEST( CastorUtilsObjectsPoolTest, ScatteredMemoryPerformanceTest, p_errCount, p_testCount );
		// EXECUTE_TEST( CastorUtilsObjectsPoolTest, VariableSizePerformanceTest, p_errCount, p_testCount );
		// EXECUTE_TEST( CastorUtilsObjectsPoolTest, UniquePoolTest, p_errCount, p_testCount );
	// }

	// void CastorUtilsObjectsPoolTest::ObjectPoolTest( uint32_t & p_errCount, uint32_t & p_testCount )
	// {
		// Memory::Checks< Castor::eMEMDATA_TYPE_FIXED >();
		// Memory::Checks< Castor::eMEMDATA_TYPE_FIXED_MARKED >();
		// Memory::Checks< Castor::eMEMDATA_TYPE_FIXED_GROWING >();
		// Memory::Checks< Castor::eMEMDATA_TYPE_FIXED_GROWING_MARKED >();
	// }

	// void CastorUtilsObjectsPoolTest::AlignedObjectPoolTest( uint32_t & p_errCount, uint32_t & p_testCount )
	// {
		// Memory::AlignedChecks< Castor::eMEMDATA_TYPE_FIXED >();
		// Memory::AlignedChecks< Castor::eMEMDATA_TYPE_FIXED_GROWING >();
	// }

	// void CastorUtilsObjectsPoolTest::FixedSizePerformanceTest( uint32_t & p_errCount, uint32_t & p_testCount )
	// {
		// FixedSizePerformance::Checks< PlacementNew::SFixedChecks >();
		// FixedSizePerformance::Checks< Castor::eMEMDATA_TYPE_FIXED >();
		// FixedSizePerformance::Checks< Castor::eMEMDATA_TYPE_FIXED_MARKED >();
		// FixedSizePerformance::Checks< Castor::eMEMDATA_TYPE_FIXED_GROWING >();
		// FixedSizePerformance::Checks< Castor::eMEMDATA_TYPE_FIXED_GROWING_MARKED >();
		// FixedSizePerformance::Checks< Traditional::SFixedChecks< AllocPolicies::SNewDeletePolicy > >();
		// FixedSizePerformance::Checks< Traditional::SFixedChecks< AllocPolicies::SMallocFreePolicy > >();
	// }

	// void CastorUtilsObjectsPoolTest::ScatteredMemoryPerformanceTest( uint32_t & p_errCount, uint32_t & p_testCount )
	// {
		// ScatteredMemoryPerformance::Index index;
		// ScatteredMemoryPerformance::Checks< PlacementNew::SScatteredChecks >( index );
		// ScatteredMemoryPerformance::Checks< Castor::eMEMDATA_TYPE_FIXED >( index );
		// ScatteredMemoryPerformance::Checks< Castor::eMEMDATA_TYPE_FIXED_MARKED >( index );
		// ScatteredMemoryPerformance::Checks< Castor::eMEMDATA_TYPE_FIXED_GROWING >( index );
		// ScatteredMemoryPerformance::Checks< Castor::eMEMDATA_TYPE_FIXED_GROWING_MARKED >( index );
		// ScatteredMemoryPerformance::Checks< Traditional::SScatteredChecks< AllocPolicies::SNewDeletePolicy > >( index );
		// ScatteredMemoryPerformance::Checks< Traditional::SScatteredChecks< AllocPolicies::SMallocFreePolicy > >( index );
	// }

	// void CastorUtilsObjectsPoolTest::VariableSizePerformanceTest( uint32_t & p_errCount, uint32_t & p_testCount )
	// {
		// VariableSizePerformance::Checks< PlacementNew::SVariableChecks >();
		// VariableSizePerformance::Checks< Castor::eMEMDATA_TYPE_FIXED_GROWING >();
		// VariableSizePerformance::Checks< Castor::eMEMDATA_TYPE_FIXED_GROWING_MARKED >();
		// VariableSizePerformance::Checks< Traditional::SVariableChecks< AllocPolicies::SNewDeletePolicy > >();
		// VariableSizePerformance::Checks< Traditional::SVariableChecks< AllocPolicies::SMallocFreePolicy > >();
	// }

	// void CastorUtilsObjectsPoolTest::UniquePoolTest( uint32_t & p_errCount, uint32_t & p_testCount )
	// {
		// UniqueObjectPool::Checks< Traditional::SFixedChecks< AllocPolicies::SNewDeletePolicy > >();
		// UniqueObjectPool::Checks< Traditional::SFixedChecks< AllocPolicies::SMallocFreePolicy > >();
		// UniqueObjectPool::Checks< Castor::eMEMDATA_TYPE_FIXED >();
		// UniqueObjectPool::Checks< Castor::eMEMDATA_TYPE_FIXED_GROWING >();
	// }
// }
