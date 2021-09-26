#include "CastorUtilsSignalTest.hpp"

#include <CastorUtils/Design/Signal.hpp>
#include <CastorUtils/Exception/Exception.hpp>

#include <random>

using castor::Signal;

namespace Testing
{
	CastorUtilsSignalTest::CastorUtilsSignalTest()
		: TestCase( "CastorUtilsSignalTest" )
	{
	}

	void CastorUtilsSignalTest::doRegisterTests()
	{
		doRegisterTest( "Creation", std::bind( &CastorUtilsSignalTest::Creation, this ) );
		doRegisterTest( "Assignment", std::bind( &CastorUtilsSignalTest::Assignment, this ) );
		doRegisterTest( "MultipleSignalConnectionAssignment", std::bind( &CastorUtilsSignalTest::MultipleSignalConnectionAssignment, this ) );
	}

	void CastorUtilsSignalTest::Creation()
	{
		Signal < std::function< void() > > signal;
		auto connection( signal.connect( [](){ CU_Exception( "coucou" ); } ) );
		CT_CHECK_THROW( signal() );
	}

	void CastorUtilsSignalTest::Assignment()
	{
		Signal < std::function< void() > > signal;
		auto connection = signal.connect( [](){ CU_Exception( "coucou" ); } );
		connection = signal.connect( [](){ CU_Exception( "coin" ); } );
		try
		{
			signal();
			CT_FAILURE( "Signal should have raised an exception" );
		}
		catch ( castor::Exception & exc )
		{
			CT_EQUAL( exc.getDescription(), cuT( "coin" ) );
		}
		connection.disconnect();
	}

	void CastorUtilsSignalTest::MultipleSignalConnectionAssignment()
	{
		Signal < std::function< void() > > signal1;
		Signal < std::function< void() > > signal2;
		auto conn1 = signal1.connect( [](){ CU_Exception( "coucou" ); } );
		auto conn2 = signal2.connect( [](){ CU_Exception( "coin" ); } );
		CT_CHECK_THROW( signal1() );
		CT_CHECK_THROW( signal2() );
		conn1 = std::move( conn2 );
		CT_CHECK_NOTHROW( signal1() );
		CT_CHECK_THROW( signal2() );
		conn1 = signal1.connect( [](){ CU_Exception( "coucou" ); } );
		CT_CHECK_NOTHROW( signal2() );
		CT_CHECK_THROW( signal1() );
		conn1 = signal2.connect( [](){ CU_Exception( "coin" ); } );
		CT_CHECK_NOTHROW( signal1() );
		CT_CHECK_THROW( signal2() );
		conn2 = signal1.connect( [](){ CU_Exception( "coucou" ); } );
		CT_CHECK_THROW( signal2() );
		CT_CHECK_THROW( signal1() );
	}
}
