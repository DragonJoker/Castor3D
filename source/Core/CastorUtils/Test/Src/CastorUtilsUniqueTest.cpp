#include "CastorUtilsUniqueTest.hpp"

#include <Design/Unique.hpp>

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

	void CastorUtilsUniqueTest::DoRegisterTests()
	{
		DoRegisterTest( "NoInstanceTest", std::bind( &CastorUtilsUniqueTest::NoInstanceTest, this ) );
		DoRegisterTest( "OneInstanceTest", std::bind( &CastorUtilsUniqueTest::OneInstanceTest, this ) );
		DoRegisterTest( "MultipleInstanceTest", std::bind( &CastorUtilsUniqueTest::MultipleInstanceTest, this ) );
		DoRegisterTest( "MemoryTest", std::bind( &CastorUtilsUniqueTest::MemoryTest, this ) );
	}

	void CastorUtilsUniqueTest::NoInstanceTest()
	{
		CT_CHECK_THROW( TestUnique::GetInstance() );
	}

	void CastorUtilsUniqueTest::OneInstanceTest()
	{
		{
			TestUnique l_tmp;
			CT_CHECK_NOTHROW( TestUnique::GetInstance() );
		}
		CT_CHECK_THROW( TestUnique::GetInstance() );
	}

	void CastorUtilsUniqueTest::MultipleInstanceTest()
	{
		TestUnique l_tmp1;
		CT_CHECK_THROW( std::make_unique< TestUnique >() );
	}

	void CastorUtilsUniqueTest::MemoryTest()
	{
		{
			TestUniqueFirst l_src;
			l_src.m_int = 0xFFCCBBAA;
			l_src.m_ptr = &l_src;
			auto & l_ref = TestUniqueFirst::GetInstance();
			CT_CHECK( l_src.m_int == l_ref.m_int );
			CT_CHECK( l_src.m_ptr == l_ref.m_ptr );
			CT_CHECK( l_src.m_ptr == &l_ref );
		}
		{
			TestUniqueMiddle l_src;
			l_src.m_int = 0x99887766;
			l_src.m_ptr = &l_src;
			auto & l_ref = TestUniqueMiddle::GetInstance();
			CT_CHECK( l_src.m_int == l_ref.m_int );
			CT_CHECK( l_src.m_ptr == l_ref.m_ptr );
			CT_CHECK( l_src.m_ptr == &l_ref );
		}
		{
			TestUniqueLast l_src;
			l_src.m_int = 0x55443322;
			l_src.m_ptr = &l_src;
			auto & l_ref = TestUniqueLast::GetInstance();
			CT_CHECK( l_src.m_int == l_ref.m_int );
			CT_CHECK( l_src.m_ptr == l_ref.m_ptr );
			CT_CHECK( l_src.m_ptr == &l_ref );
		}
	}

	//*********************************************************************************************
}
