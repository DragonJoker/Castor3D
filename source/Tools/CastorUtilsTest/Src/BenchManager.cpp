#include "BenchManager.hpp"
#include "Benchmark.hpp"
#include "UnitTest.hpp"

using namespace Castor;

namespace Testing
{
	std::vector< BenchCaseSPtr > BenchManager::m_arrayBenchs;
	std::vector< TestCaseSPtr > BenchManager::m_arrayTests;

	BenchManager::BenchManager()
	{
	}

	BenchManager::~BenchManager()
	{
		m_arrayBenchs.clear();
		m_arrayTests.clear();
	}

	void BenchManager::Register( BenchCaseSPtr p_pBench )
	{
		m_arrayBenchs.push_back( p_pBench );
	}

	void BenchManager::Register( TestCaseSPtr p_pCode )
	{
		m_arrayTests.push_back( p_pCode );
	}

	void BenchManager::ExecuteBenchs()
	{
		Logger::LogMessage( cuT(	"*********************************************************************************************" ) );

		if ( m_arrayBenchs.size() )
		{
			Logger::LogMessage( cuT(	"Benchmarks - Begin" ) );
			Logger::LogMessage(	cuT(	"**************************************************" ) );
			std::for_each( m_arrayBenchs.begin(), m_arrayBenchs.end(), []( BenchCaseSPtr p_bench )
			{
				p_bench->Execute();
			} );
			Logger::LogMessage( cuT(	"Benchmarks - End" ) );
		}
		else
		{
			Logger::LogMessage( cuT(	"No bench" ) );
		}

		Logger::LogMessage( cuT(	"*********************************************************************************************" ) );
	}

	void BenchManager::BenchsSummary()
	{
		Logger::LogMessage(	cuT(	"**************************************************" ) );
		std::for_each( m_arrayBenchs.begin(), m_arrayBenchs.end(), []( BenchCaseSPtr p_bench )
		{
			Logger::LogMessage(	p_bench->GetSummary().c_str() );
		} );
	}

	uint32_t BenchManager::ExecuteTests()
	{
		uint32_t l_errCount = 0;
		uint32_t l_testCount = 0;
		Logger::LogMessage( cuT(	"*********************************************************************************************" ) );

		if ( m_arrayTests.size() )
		{
			Logger::LogMessage( cuT(	"Tests - Begin" ) );
			Logger::LogMessage( cuT(	"**************************************************" ) );
			std::for_each( m_arrayTests.begin(), m_arrayTests.end(), [&]( TestCaseSPtr p_pCode )
			{
				try
				{
					p_pCode->Execute( l_errCount, l_testCount );
				}
				catch ( TestFailed & exc )
				{
					Logger::LogWarning(	"*	Test " + p_pCode->GetName() + " execution failed (" + exc.what() + ") " );
				}
				catch ( std::exception & exc )
				{
					l_errCount++;
					Logger::LogMessage(	"*	Test %s execution failed (%s)",	p_pCode->GetName().c_str(), exc.what() );
				}
				catch ( ... )
				{
					l_errCount++;
					Logger::LogMessage(	"*	Test " + p_pCode->GetName() + " execution failed (Unknown reason)" );
				}

				Logger::LogMessage(	cuT(	"**************************************************" ) );
			} );

			if ( l_errCount )
			{
				Logger::LogMessage( cuT(	"Tests - End, %d errors detected out of %d tests" ), l_errCount, l_testCount );
			}
			else
			{
				Logger::LogMessage( cuT(	"Tests - End, no error detected out of %d tests" ), l_testCount );
			}
		}
		else
		{
			Logger::LogMessage( cuT(	"No test" ) );
		}

		Logger::LogMessage( cuT(	"*********************************************************************************************" ) );
		return l_errCount;
	}

	//*************************************************************************************************

	bool Register( BenchCaseSPtr p_pBenchmark )
	{
		BenchManager::Register( p_pBenchmark );
		return true;
	}

	bool Register( TestCaseSPtr p_pCode )
	{
		BenchManager::Register( p_pCode );
		return true;
	}
}
