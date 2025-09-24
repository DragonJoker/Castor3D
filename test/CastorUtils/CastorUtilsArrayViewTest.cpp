#include "CastorUtilsArrayViewTest.hpp"

#include <CastorUtils/Design/ArrayView.hpp>

#include <numeric>

namespace Testing
{
	CastorUtilsArrayViewTest::CastorUtilsArrayViewTest()
		: TestCase{ "CastorUtilsArrayViewTest" }
	{
	}

	void CastorUtilsArrayViewTest::doRegisterTests()
	{
		doRegisterTest( "BasicArrayViewTest", [this](){ BasicTest(); } );
	}

	void CastorUtilsArrayViewTest::BasicTest()
	{
		{
			CT_ON("	Check build from dynamically allocated buffer" );
			uint64_t const size = 8;
			c3d::Vector< int > tmp;
			tmp.resize( size );
			auto view1 = c3d::makeArrayView( tmp.data(), size );
			CT_CHECK( view1.size() == size );
			CT_CHECK( !view1.empty() );
			CT_CHECK( view1.begin() == tmp.data() );
			CT_CHECK( view1.cbegin() == view1.begin() );
			CT_CHECK( view1.end() == tmp.data() + size );
			CT_CHECK( view1.cend() == view1.end() );
			CT_CHECK( view1.end() == view1.begin() + view1.size() );
			CT_CHECK( view1.end() == view1.begin() + view1.size() );
			std::iota( view1.begin(), view1.end(), 0 );
			size_t index{ 0u };

			for ( auto value : view1 )
			{
				CT_CHECK( value == tmp[index++] );
			}

			CT_CHECK( *view1.rbegin() == tmp[--index] );
		}
		{
			CT_ON("	Check build from statically allocated buffer" );
			static size_t const size = 8;
			int tmp[size];
			auto view1 = c3d::makeArrayView( tmp );
			CT_CHECK( view1.size() == size );
			CT_CHECK( !view1.empty() );
			CT_CHECK( view1.begin() == tmp );
			CT_CHECK( view1.cbegin() == view1.begin() );
			CT_CHECK( view1.end() == tmp + size );
			CT_CHECK( view1.cend() == view1.end() );
			CT_CHECK( view1.end() == view1.begin() + view1.size() );
			CT_CHECK( view1.end() == view1.begin() + view1.size() );
			std::iota( view1.begin(), view1.end(), 0 );
			size_t index{ 0u };

			for ( auto value : view1 )
			{
				CT_CHECK( value == tmp[index++] );
			}

			CT_CHECK( *view1.rbegin() == tmp[--index] );
		}
		{
			CT_ON("	Check build buffer part" );
			uint32_t const size = 0;
			c3d::Vector< int > tmp;
			tmp.resize( size + 1 );
			auto view1 = c3d::makeArrayView( tmp.data(), size );
			CT_CHECK( view1.size() == size );
			CT_CHECK( view1.empty() );
			CT_CHECK( view1.begin() == tmp.data() );
			CT_CHECK( view1.cbegin() == view1.begin() );
			CT_CHECK( view1.end() == tmp.data() + size );
			CT_CHECK( view1.cend() == view1.end() );
			CT_CHECK( view1.end() == view1.begin() + view1.size() );
			CT_CHECK( view1.end() == view1.begin() + view1.size() );
			CT_CHECK( view1.end() == view1.begin() );
		}
	}
}
