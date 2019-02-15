#include "CastorUtilsChangeTrackedTest.hpp"

#include <Design/ChangeTracked.hpp>

#include <numeric>

using namespace castor;

namespace Testing
{
	CastorUtilsChangeTrackedTest::CastorUtilsChangeTrackedTest()
		: TestCase{ "CastorUtilsChangeTrackedTest" }
	{
	}

	CastorUtilsChangeTrackedTest::~CastorUtilsChangeTrackedTest()
	{
	}

	void CastorUtilsChangeTrackedTest::doRegisterTests()
	{
		doRegisterTest( "BasicChangeTrackedTest", std::bind( &CastorUtilsChangeTrackedTest::BasicTest, this ) );
	}

	void CastorUtilsChangeTrackedTest::BasicTest()
	{
		auto tracked = makeChangeTracked( 1 );
		CT_CHECK( tracked.isDirty() );
		CT_EQUAL( tracked.value(), 1 );
		CT_EQUAL( int( tracked ), 1 );
		tracked.reset();
		CT_CHECK( !tracked.isDirty() );
		tracked = 2;
		CT_CHECK( tracked.isDirty() );
		CT_EQUAL( tracked.value(), 2 );
		CT_EQUAL( int( tracked ), 2 );
		tracked.reset();

		auto tracked2 = tracked;
		CT_CHECK( tracked2.isDirty() );
		CT_EQUAL( tracked2.value(), 2 );
		CT_EQUAL( int( tracked2 ), 2 );
		tracked = 3;
		CT_EQUAL( tracked2.value(), 3 );
		CT_NEQUAL( tracked2, tracked );
	}
}
