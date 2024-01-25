#include "CastorUtilsUniqueTest.hpp"

#include <CastorUtils/Design/Unique.hpp>

namespace Testing
{
	//*********************************************************************************************

	namespace
	{
		class TestUnique
			: public castor::Unique< TestUnique >
		{
		public:
			TestUnique()
				: castor::Unique< TestUnique >( this )
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
			: public castor::Unique< TestUniqueFirst >
			, public IntVal
			, public PtrVal
		{
		public:
			TestUniqueFirst()
				: castor::Unique< TestUniqueFirst >( this )
			{
			}
		};

		class TestUniqueMiddle
			: public IntVal
			, public castor::Unique< TestUniqueMiddle >
			, public PtrVal
		{
		public:
			TestUniqueMiddle()
				: castor::Unique< TestUniqueMiddle >( this )
			{
			}
		};

		class TestUniqueLast
			: public IntVal
			, public PtrVal
			, public castor::Unique< TestUniqueLast >
		{
		public:
			TestUniqueLast()
				: castor::Unique< TestUniqueLast >( this )
			{
			}
		};
	}

	//*********************************************************************************************

	CastorUtilsUniqueTest::CastorUtilsUniqueTest()
		:	TestCase( "CastorUtilsUniqueTest" )
	{
	}

	void CastorUtilsUniqueTest::doRegisterTests()
	{
		doRegisterTest( "NoInstanceTest", std::bind( &CastorUtilsUniqueTest::NoInstanceTest, this ) );
		doRegisterTest( "OneInstanceTest", std::bind( &CastorUtilsUniqueTest::OneInstanceTest, this ) );
		doRegisterTest( "MultipleInstanceTest", std::bind( &CastorUtilsUniqueTest::MultipleInstanceTest, this ) );
		doRegisterTest( "MemoryTest", std::bind( &CastorUtilsUniqueTest::MemoryTest, this ) );
	}

	void CastorUtilsUniqueTest::NoInstanceTest()
	{
		//CT_CHECK_THROW( TestUnique::getInstance() );
	}

	void CastorUtilsUniqueTest::OneInstanceTest()
	{
		//{
		//	TestUnique tmp;
		//	CT_CHECK_NOTHROW( TestUnique::getInstance() );
		//}
		//CT_CHECK_THROW( TestUnique::getInstance() );
	}

	void CastorUtilsUniqueTest::MultipleInstanceTest()
	{
		TestUnique tmp1;
		CT_CHECK_THROW( castor::make_unique< TestUnique >() );
	}

	void CastorUtilsUniqueTest::MemoryTest()
	{
		{
			TestUniqueFirst src;
			src.m_int = 0xFFCCBBAA;
			src.m_ptr = &src;
			auto & ref = src;
			CT_CHECK( src.m_int == ref.m_int );
			CT_CHECK( src.m_ptr == ref.m_ptr );
			CT_CHECK( src.m_ptr == &ref );
		}
		{
			TestUniqueMiddle src;
			src.m_int = 0x99887766;
			src.m_ptr = &src;
			auto & ref = src;
			CT_CHECK( src.m_int == ref.m_int );
			CT_CHECK( src.m_ptr == ref.m_ptr );
			CT_CHECK( src.m_ptr == &ref );
		}
		{
			TestUniqueLast src;
			src.m_int = 0x55443322;
			src.m_ptr = &src;
			auto & ref = src;
			CT_CHECK( src.m_int == ref.m_int );
			CT_CHECK( src.m_ptr == ref.m_ptr );
			CT_CHECK( src.m_ptr == &ref );
		}
	}

	//*********************************************************************************************
}
