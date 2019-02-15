#include "CastorUtilsUniqueTest.hpp"

#include <CastorUtils/Design/Unique.hpp>

using namespace castor;

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
		CT_CHECK_THROW( std::make_unique< TestUnique >() );
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
