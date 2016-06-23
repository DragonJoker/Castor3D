#include "CastorUtilsArrayViewTest.hpp"

#include <ArrayView.hpp>

#include <numeric>

using namespace Castor;

namespace Testing
{
	CastorUtilsArrayViewTest::CastorUtilsArrayViewTest()
		: TestCase{ "CastorUtilsArrayViewTest" }
	{
	}

	CastorUtilsArrayViewTest::~CastorUtilsArrayViewTest()
	{
	}

	void CastorUtilsArrayViewTest::DoRegisterTests()
	{
		DoRegisterTest( "BasicArrayViewTest", std::bind( &CastorUtilsArrayViewTest::BasicTest, this ) );
	}

	void CastorUtilsArrayViewTest::BasicTest()
	{
		{
			size_t const l_size = 8;
			int * l_tmp = new int[l_size];
			ArrayView< int > l_view1{ l_tmp, l_size };
			CT_CHECK( l_view1.size() == l_size );
			CT_CHECK( !l_view1.empty() );
			CT_CHECK( l_view1.begin() == l_tmp );
			CT_CHECK( l_view1.cbegin() == l_view1.begin() );
			CT_CHECK( l_view1.end() == l_tmp + l_size );
			CT_CHECK( l_view1.cend() == l_view1.end() );
			CT_CHECK( l_view1.end() == l_view1.begin() + l_view1.size() );
			CT_CHECK( l_view1.end() == l_view1.begin() + l_view1.size() );
			std::iota( l_view1.begin(), l_view1.end(), 0 );
			size_t l_index{ 0u };

			for ( auto l_value : l_view1 )
			{
				CT_CHECK( l_value == l_tmp[l_index++] );
			}

			CT_CHECK( *l_view1.rbegin() == l_tmp[--l_index] );
			delete[] l_tmp;
		}
		{
			static size_t const l_size = 8;
			int l_tmp[l_size];
			ArrayView< int > l_view1{ l_tmp };
			CT_CHECK( l_view1.size() == l_size );
			CT_CHECK( !l_view1.empty() );
			CT_CHECK( l_view1.begin() == l_tmp );
			CT_CHECK( l_view1.cbegin() == l_view1.begin() );
			CT_CHECK( l_view1.end() == l_tmp + l_size );
			CT_CHECK( l_view1.cend() == l_view1.end() );
			CT_CHECK( l_view1.end() == l_view1.begin() + l_view1.size() );
			CT_CHECK( l_view1.end() == l_view1.begin() + l_view1.size() );
			std::iota( l_view1.begin(), l_view1.end(), 0 );
			size_t l_index{ 0u };

			for ( auto l_value : l_view1 )
			{
				CT_CHECK( l_value == l_tmp[l_index++] );
			}

			CT_CHECK( *l_view1.rbegin() == l_tmp[--l_index] );
		}
		{
			size_t const l_size = 0;
			int * l_tmp = new int[l_size + 1];
			ArrayView< int > l_view1{ l_tmp, l_size };
			CT_CHECK( l_view1.size() == l_size );
			CT_CHECK( l_view1.empty() );
			CT_CHECK( l_view1.begin() == l_tmp );
			CT_CHECK( l_view1.cbegin() == l_view1.begin() );
			CT_CHECK( l_view1.end() == l_tmp + l_size );
			CT_CHECK( l_view1.cend() == l_view1.end() );
			CT_CHECK( l_view1.end() == l_view1.begin() + l_view1.size() );
			CT_CHECK( l_view1.end() == l_view1.begin() + l_view1.size() );
			CT_CHECK( l_view1.end() == l_view1.begin() );
			delete[] l_tmp;
		}
	}
}
