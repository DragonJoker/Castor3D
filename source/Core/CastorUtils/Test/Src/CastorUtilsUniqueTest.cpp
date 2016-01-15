#include "CastorUtilsUniqueTest.hpp"

#include <Unique.hpp>

using namespace Castor;

namespace Testing
{
	//*********************************************************************************************

	namespace
	{
		class TestUnique
			: public Unique< TestUnique >
		{
		public:
			TestUnique()
				: Unique< TestUnique >( this )
			{
			}

			~TestUnique()
			{
			}
		};

		struct IntVal
		{
			int m_int;
		};

		struct PtrVal
		{
			void * m_ptr;
		};

		class TestUniqueFirst
			: public Unique< TestUniqueFirst >
			, public IntVal
			, public PtrVal
		{
		public:
			TestUniqueFirst()
				: Unique< TestUniqueFirst >( this )
			{
			}

			~TestUniqueFirst()
			{
			}
		};

		class TestUniqueMiddle
			: public IntVal
			, public Unique< TestUniqueMiddle >
			, public PtrVal
		{
		public:
			TestUniqueMiddle()
				: Unique< TestUniqueMiddle >( this )
			{
			}

			~TestUniqueMiddle()
			{
			}
		};

		class TestUniqueLast
			: public IntVal
			, public PtrVal
			, public Unique< TestUniqueLast >
		{
		public:
			TestUniqueLast()
				: Unique< TestUniqueLast >( this )
			{
			}

			~TestUniqueLast()
			{
			}
		};
	}

	//*********************************************************************************************

	CastorUtilsUniqueTest::CastorUtilsUniqueTest()
		:	TestCase( "CastorUtilsUniqueTest" )
	{
	}

	CastorUtilsUniqueTest::~CastorUtilsUniqueTest()
	{
	}

	void CastorUtilsUniqueTest::Execute( uint32_t & p_errCount, uint32_t & p_testCount )
	{
		EXECUTE_TEST( CastorUtilsUniqueTest, NoInstanceTest, p_errCount, p_testCount );
		EXECUTE_TEST( CastorUtilsUniqueTest, OneInstanceTest, p_errCount, p_testCount );
		EXECUTE_TEST( CastorUtilsUniqueTest, MultipleInstanceTest, p_errCount, p_testCount );
		EXECUTE_TEST( CastorUtilsUniqueTest, MemoryTest, p_errCount, p_testCount );
	}

	void CastorUtilsUniqueTest::NoInstanceTest( uint32_t & p_errCount, uint32_t & p_testCount )
	{
		TEST_CHECK_THROW( TestUnique::GetInstance() );
	}

	void CastorUtilsUniqueTest::OneInstanceTest( uint32_t & p_errCount, uint32_t & p_testCount )
	{
		{
			TestUnique l_tmp;
			TEST_CHECK_NOTHROW( TestUnique::GetInstance() );
		}
		TEST_CHECK_THROW( TestUnique::GetInstance() );
	}

	void CastorUtilsUniqueTest::MultipleInstanceTest( uint32_t & p_errCount, uint32_t & p_testCount )
	{
		TestUnique l_tmp1;
		TEST_CHECK_THROW( std::make_unique< TestUnique >() );
	}

	void CastorUtilsUniqueTest::MemoryTest( uint32_t & p_errCount, uint32_t & p_testCount )
	{
		{
			TestUniqueFirst l_src;
			l_src.m_int = 0xFFCCBBAA;
			l_src.m_ptr = &l_src;
			auto & l_ref = TestUniqueFirst::GetInstance();
			TEST_CHECK( l_src.m_int == l_ref.m_int );
			TEST_CHECK( l_src.m_ptr == l_ref.m_ptr );
			TEST_CHECK( l_src.m_ptr == &l_ref );
		}
		{
			TestUniqueMiddle l_src;
			l_src.m_int = 0x99887766;
			l_src.m_ptr = &l_src;
			auto & l_ref = TestUniqueMiddle::GetInstance();
			TEST_CHECK( l_src.m_int == l_ref.m_int );
			TEST_CHECK( l_src.m_ptr == l_ref.m_ptr );
			TEST_CHECK( l_src.m_ptr == &l_ref );
		}
		{
			TestUniqueLast l_src;
			l_src.m_int = 0x55443322;
			l_src.m_ptr = &l_src;
			auto & l_ref = TestUniqueLast::GetInstance();
			TEST_CHECK( l_src.m_int == l_ref.m_int );
			TEST_CHECK( l_src.m_ptr == l_ref.m_ptr );
			TEST_CHECK( l_src.m_ptr == &l_ref );
		}
	}

	//*********************************************************************************************
}
