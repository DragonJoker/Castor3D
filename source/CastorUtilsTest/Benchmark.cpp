#include "CastorUtilsTest/Benchmark.hpp"

using namespace CastorUtilsTest;
using namespace Castor;

//*************************************************************************************************

BenchmarkBase :: BenchmarkBase( std::string const & p_strName )
	:	m_strName			( p_strName	)
	,	m_dCumulativeTimes	( 0.0		)
	,	m_uiTotalExecutions	( 0			)
{
}

BenchmarkBase :: ~BenchmarkBase()
{
}

void BenchmarkBase :: Bench()
{
	m_preciseTimer.TimeS();
	DoBench();
	m_dCumulativeTimes += m_preciseTimer.TimeS();
	m_uiTotalExecutions++;
}

//*************************************************************************************************

CodeBase :: CodeBase( std::string const & p_strName )
	:	m_strName	( p_strName	)
{
}

CodeBase :: ~CodeBase()
{
}

//*************************************************************************************************

std::vector< std::pair< uint64_t, BenchmarkBaseSPtr > > BenchFactory :: m_arrayBenchs;
std::vector< CodeBaseSPtr > BenchFactory :: m_arrayCodes;

BenchFactory :: BenchFactory()
{
}

BenchFactory :: ~BenchFactory()
{
	m_arrayBenchs.clear();
	m_arrayCodes.clear();
}

void BenchFactory :: Register( uint64_t p_ui64Calls, BenchmarkBaseSPtr p_pBench )
{
	m_arrayBenchs.push_back( std::make_pair( p_ui64Calls, p_pBench ) );
}

void BenchFactory :: Register( CodeBaseSPtr p_pCode )
{
	m_arrayCodes.push_back( p_pCode );
}

void BenchFactory :: ExecuteBenchs()
{
	Logger::LogMessage( cuT( "*********************************************************************************************" ) );

	if( m_arrayBenchs.size() )
	{
		Logger::LogMessage( cuT( "Benchmarks - Begin" ) );
		Logger::LogMessage(	cuT( "**************************************************" ) );

		std::for_each( m_arrayBenchs.begin(), m_arrayBenchs.end(), []( std::pair< uint64_t, BenchmarkBaseSPtr > p_pair )
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

		Logger::LogMessage( cuT( "Benchmarks - End" ) );
	}
	else
	{
		Logger::LogMessage( cuT( "No bench" ) );
	}

	Logger::LogMessage( cuT( "*********************************************************************************************" ) );
}

void BenchFactory :: BenchsSummary()
{
	Logger::LogMessage(	cuT( "**************************************************" ) );

	std::for_each( m_arrayBenchs.begin(), m_arrayBenchs.end(), []( std::pair< uint64_t, BenchmarkBaseSPtr > p_pair )
	{
		Logger::LogMessage(			"*	%s global results :",			p_pair.second->GetName().c_str() );
		Logger::LogMessage(			"*		- Executed %d times",		p_pair.second->GetTotalExecutions() );
		Logger::LogMessage(	cuT(	"*		- Total time : %.4fs" ),	p_pair.second->GetCumulativeTime() );
		Logger::LogMessage(	cuT(	"*		- Average time : %.4fms" ),	(1000.0 * p_pair.second->GetCumulativeTime() / p_pair.second->GetTotalExecutions() ) );
		Logger::LogMessage(	cuT(	"**************************************************" ) );
	} );
}

void BenchFactory :: ExecuteCodes()
{
	Logger::LogMessage( cuT( "*********************************************************************************************" ) );

	if( m_arrayCodes.size() )
	{
		Logger::LogMessage( cuT( "Codes - Begin" ) );
		Logger::LogMessage(	cuT( "**************************************************" ) );

		std::for_each( m_arrayCodes.begin(), m_arrayCodes.end(), []( CodeBaseSPtr p_pCode )
		{
			try
			{
				p_pCode->Execute();
				Logger::LogMessage(	cuT(	"**************************************************" ) );
			}
			catch( ... )
			{
				Logger::LogMessage(			"*	Code %s execution failed",	p_pCode->GetName().c_str() );
				Logger::LogMessage(	cuT(	"**************************************************" ) );
			}
		} );

		Logger::LogMessage( cuT( "Codes - End" ) );
	}
	else
	{
		Logger::LogMessage( cuT( "No code" ) );
	}

	Logger::LogMessage( cuT( "*********************************************************************************************" ) );
}

//*************************************************************************************************

bool CastorUtilsTest::RegisterBenchmark( uint64_t p_ui64Calls, BenchmarkBaseSPtr p_pBenchmark )
{
	BenchFactory::Register( p_ui64Calls, p_pBenchmark );
	return true;
}

bool CastorUtilsTest::RegisterCode( CodeBaseSPtr p_pCode )
{
	BenchFactory::Register( p_pCode );
	return true;
}

//*************************************************************************************************
