#include "BenchManager.hpp"
#include "Benchmark.hpp"
#include "UnitTest.hpp"

#include <algorithm>

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
		Logger::LogInfo( cuT(	"*********************************************************************************************" ) );

		if ( m_arrayBenchs.size() )
		{
			Logger::LogInfo( cuT(	"Benchmarks - Begin" ) );
			Logger::LogInfo(	cuT(	"**************************************************" ) );
			std::for_each( m_arrayBenchs.begin(), m_arrayBenchs.end(), []( BenchCaseSPtr p_bench )
			{
				p_bench->Execute();
			} );
			Logger::LogInfo( cuT(	"Benchmarks - End" ) );
		}
		else
		{
			Logger::LogInfo( cuT(	"No bench" ) );
		}

		Logger::LogInfo( cuT(	"*********************************************************************************************" ) );
	}

	void BenchManager::BenchsSummary()
	{
		Logger::LogInfo(	cuT(	"**************************************************" ) );
		std::for_each( m_arrayBenchs.begin(), m_arrayBenchs.end(), []( BenchCaseSPtr p_bench )
		{
			Logger::LogInfo(	p_bench->GetSummary().c_str() );
		} );
	}

	uint32_t BenchManager::ExecuteTests()
	{
		uint32_t l_errCount = 0;
		uint32_t l_testCount = 0;
		Logger::LogInfo( cuT(	"*********************************************************************************************" ) );

		if ( m_arrayTests.size() )
		{
			Logger::LogInfo( cuT(	"Tests - Begin" ) );
			Logger::LogInfo( cuT(	"**************************************************" ) );
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
					Logger::LogInfo( std::stringstream() << "*	Test " << p_pCode->GetName().c_str() << " execution failed (" << exc.what() << ")" );
				}
				catch ( ... )
				{
					l_errCount++;
					Logger::LogInfo(	"*	Test " + p_pCode->GetName() + " execution failed (Unknown reason)" );
				}

				Logger::LogInfo(	cuT(	"**************************************************" ) );
			} );

			if ( l_errCount )
			{
				Logger::LogInfo( StringStream() << cuT(	"Tests - End, " ) << l_errCount << cuT( " errors detected out of " ) << l_testCount << cuT( " tests" ) );
			}
			else
			{
				Logger::LogInfo( StringStream() << cuT(	"Tests - End, no error detected out of " ) << l_testCount << cuT( " tests" ) );
			}
		}
		else
		{
			Logger::LogInfo( cuT(	"No test" ) );
		}

		Logger::LogInfo( cuT(	"*********************************************************************************************" ) );
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
