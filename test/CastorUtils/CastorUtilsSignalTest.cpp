#include "CastorUtilsSignalTest.hpp"

#include <CastorUtils/Design/Signal.hpp>
#include <CastorUtils/Exception/Exception.hpp>

#include <random>

namespace Testing
{
	using c3d::SignalT;

	CastorUtilsSignalTest::CastorUtilsSignalTest()
		: TestCase( "CastorUtilsSignalTest" )
	{
	}

	void CastorUtilsSignalTest::doRegisterTests()
	{
		doRegisterTest( "Creation", [this](){ Creation(); } );
		doRegisterTest( "Assignment", [this](){ Assignment(); } );
		doRegisterTest( "MultipleSignalConnectionAssignment", [this](){ MultipleSignalConnectionAssignment(); } );
	}

	void CastorUtilsSignalTest::Creation()
	{
		SignalT< c3d::Function< void() > > signal;
		auto connection( signal.connect( [](){ CU_Exception( "coucou" ); } ) );
		CT_CHECK_THROW( signal() );
	}

	void CastorUtilsSignalTest::Assignment()
	{
		SignalT< c3d::Function< void() > > signal;
		auto connection = signal.connect( [](){ CU_Exception( "coucou" ); } );
		connection = signal.connect( [](){ CU_Exception( "coin" ); } );
		try
		{
			signal();
			CT_FAILURE( "Signal should have raised an exception" );
		}
		catch ( c3d::Exception & exc )
		{
			CT_EQUAL( exc.getDescription(), "coin" );
		}
		connection.disconnect();
	}

	void CastorUtilsSignalTest::MultipleSignalConnectionAssignment()
	{
		SignalT< c3d::Function< void() > > signal1;
		SignalT< c3d::Function< void() > > signal2;
		auto conn1 = signal1.connect( [](){ CU_Exception( "coucou" ); } );
		auto conn2 = signal2.connect( [](){ CU_Exception( "coin" ); } );
		CT_CHECK_THROW( signal1() );
		CT_CHECK_THROW( signal2() );
		conn1 = c3d::move( conn2 );
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
