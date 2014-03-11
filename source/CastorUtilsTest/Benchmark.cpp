#include "CastorUtilsTest/Benchmark.hpp"

using namespace CastorUtilsTest;
using namespace Castor;

//*************************************************************************************************

BenchCase :: BenchCase( std::string const & p_strName )
	:	m_strName			( p_strName	)
	,	m_dCumulativeTimes	( 0.0		)
	,	m_uiTotalExecutions	( 0			)
{
}

BenchCase :: ~BenchCase()
{
}

void BenchCase :: Bench()
{
	m_preciseTimer.TimeS();
	DoBench();
	m_dCumulativeTimes += m_preciseTimer.TimeS();
	m_uiTotalExecutions++;
}

//*************************************************************************************************

TestCase :: TestCase( std::string const & p_strName )
	:	m_strName	( p_strName	)
{
}

TestCase :: ~TestCase()
{
}

//*************************************************************************************************

std::vector< std::pair< uint64_t, BenchCaseSPtr > > BenchFactory :: m_arrayBenchs;
std::vector< TestCaseSPtr > BenchFactory :: m_arrayTests;

BenchFactory :: BenchFactory()
{
}

BenchFactory :: ~BenchFactory()
{
	m_arrayBenchs.clear();
	m_arrayTests.clear();
}

void BenchFactory :: Register( uint64_t p_ui64Calls, BenchCaseSPtr p_pBench )
{
	m_arrayBenchs.push_back( std::make_pair( p_ui64Calls, p_pBench ) );
}

void BenchFactory :: Register( TestCaseSPtr p_pCode )
{
	m_arrayTests.push_back( p_pCode );
}

void BenchFactory :: ExecuteBenchs()
{
	Logger::LogMessage( cuT(				"*********************************************************************************************" ) );

	if( m_arrayBenchs.size() )
	{
		Logger::LogMessage( cuT(			"Benchmarks - Begin" ) );
		Logger::LogMessage(	cuT(			"**************************************************" ) );

		std::for_each( m_arrayBenchs.begin(), m_arrayBenchs.end(), []( std::pair< uint64_t, BenchCaseSPtr > p_pair )
		{
			try
			{
				for( uint64_t i = 0; i < p_pair.first; i++ )
				{
					p_pair.second->Bench();
				}

				Logger::LogMessage(			"*	Bench ended for : %s",			p_pair.second->GetName().c_str() );
				Logger::LogMessage(			"*		- Executed %d times",		p_pair.first );
				Logger::LogMessage(	cuT(	"*		- Total time : %.4fs" ),	p_pair.second->GetCumulativeTime() );
				Logger::LogMessage(	cuT(	"*		- Average time : %.4fms" ),	(1000.0 * p_pair.second->GetCumulativeTime() / p_pair.second->GetTotalExecutions() ) );
				Logger::LogMessage(	cuT(	"**************************************************" ) );
			}
			catch( ... )
			{
				Logger::LogMessage(			"*	Bench %s failed",				p_pair.second->GetName().c_str() );
				Logger::LogMessage(	cuT(	"**************************************************" ) );
			}
		} );

		Logger::LogMessage( cuT(			"Benchmarks - End" ) );
	}
	else
	{
		Logger::LogMessage( cuT(			"No bench" ) );
	}

	Logger::LogMessage( cuT(				"*********************************************************************************************" ) );
}

void BenchFactory :: BenchsSummary()
{
	Logger::LogMessage(	cuT(				"**************************************************" ) );

	std::for_each( m_arrayBenchs.begin(), m_arrayBenchs.end(), []( std::pair< uint64_t, BenchCaseSPtr > p_pair )
	{
		Logger::LogMessage(					"*	%s global results :",			p_pair.second->GetName().c_str() );
		Logger::LogMessage(					"*		- Executed %d times",		p_pair.second->GetTotalExecutions() );
		Logger::LogMessage(	cuT(			"*		- Total time : %.4fs" ),	p_pair.second->GetCumulativeTime() );
		Logger::LogMessage(	cuT(			"*		- Average time : %.4fms" ),	(1000.0 * p_pair.second->GetCumulativeTime() / p_pair.second->GetTotalExecutions() ) );
		Logger::LogMessage(	cuT(			"**************************************************" ) );
	} );
}

void BenchFactory :: ExecuteTests()
{
	uint32_t l_errCount = 0;
	uint32_t l_testCount = 0;
	Logger::LogMessage( cuT(				"*********************************************************************************************" ) );

	if( m_arrayTests.size() )
	{
		Logger::LogMessage( cuT(			"Tests - Begin" ) );
		Logger::LogMessage( cuT(			"**************************************************" ) );

		std::for_each( m_arrayTests.begin(), m_arrayTests.end(), [&]( TestCaseSPtr p_pCode )
		{
			try
			{
				p_pCode->Execute( l_errCount, l_testCount );
			}
			catch( std::exception & exc )
			{
				l_errCount++;
				Logger::LogMessage(			"*	Test %s execution failed (%s)",	p_pCode->GetName().c_str(), exc.what() );
			}
			catch( ... )
			{
				l_errCount++;
				Logger::LogMessage(			"*	Test %s execution failed (Unknown reason)",	p_pCode->GetName().c_str() );
			}

			Logger::LogMessage(	cuT(		"**************************************************" ) );
		} );

		Logger::LogMessage( cuT(			"Tests - End, %d errors detected out of %d tests" ), l_errCount, l_testCount );
	}
	else
	{
		Logger::LogMessage( cuT(			"No test" ) );
	}

	Logger::LogMessage( cuT(				"*********************************************************************************************" ) );
}

//*************************************************************************************************

bool CastorUtilsTest::RegisterBench( uint64_t p_ui64Calls, BenchCaseSPtr p_pBenchmark )
{
	BenchFactory::Register( p_ui64Calls, p_pBenchmark );
	return true;
}

bool CastorUtilsTest::RegisterTest( TestCaseSPtr p_pCode )
{
	BenchFactory::Register( p_pCode );
	return true;
}

//*************************************************************************************************
