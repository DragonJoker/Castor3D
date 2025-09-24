#include "CastorUtilsUniqueTest.hpp"

#include <CastorUtils/Design/Unique.hpp>

namespace Testing
{
	//*********************************************************************************************

	namespace
	{
		class TestUnique
			: public c3d::Unique< TestUnique >
		{
		public:
			TestUnique()
				: c3d::Unique< TestUnique >( this )
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
			: public c3d::Unique< TestUniqueFirst >
			, public IntVal
			, public PtrVal
		{
		public:
			TestUniqueFirst()
				: c3d::Unique< TestUniqueFirst >( this )
			{
			}
		};

		class TestUniqueMiddle
			: public IntVal
			, public c3d::Unique< TestUniqueMiddle >
			, public PtrVal
		{
		public:
			TestUniqueMiddle()
				: c3d::Unique< TestUniqueMiddle >( this )
			{
			}
		};

		class TestUniqueLast
			: public IntVal
			, public PtrVal
			, public c3d::Unique< TestUniqueLast >
		{
		public:
			TestUniqueLast()
				: c3d::Unique< TestUniqueLast >( this )
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
		doRegisterTest( "MultipleInstanceTest", [this](){ MultipleInstanceTest(); } );
		doRegisterTest( "MemoryTest", [this](){ MemoryTest(); } );
	}

	void CastorUtilsUniqueTest::MultipleInstanceTest()
	{
		TestUnique tmp1;
		CT_CHECK_THROW( c3d::makeRawUnique< TestUnique >() );
	}

	void CastorUtilsUniqueTest::MemoryTest()
	{
		{
			TestUniqueFirst src;
			src.m_int = 0xFFCCBBAA;
			src.m_ptr = &src;
			auto const & ref = src;
			CT_CHECK( src.m_int == ref.m_int );
			CT_CHECK( src.m_ptr == ref.m_ptr );
			CT_CHECK( src.m_ptr == &ref );
		}
		{
			TestUniqueMiddle src;
			src.m_int = 0x99887766;
			src.m_ptr = &src;
			auto const & ref = src;
			CT_CHECK( src.m_int == ref.m_int );
			CT_CHECK( src.m_ptr == ref.m_ptr );
			CT_CHECK( src.m_ptr == &ref );
		}
		{
			TestUniqueLast src;
			src.m_int = 0x55443322;
			src.m_ptr = &src;
			auto const & ref = src;
			CT_CHECK( src.m_int == ref.m_int );
			CT_CHECK( src.m_ptr == ref.m_ptr );
			CT_CHECK( src.m_ptr == &ref );
		}
	}

	//*********************************************************************************************
}
