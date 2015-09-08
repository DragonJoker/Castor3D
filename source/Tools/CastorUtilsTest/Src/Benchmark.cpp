#include "Benchmark.hpp"

using namespace Castor;

namespace Testing
{
	BenchCase::BenchCase( std::string const & p_strName )
		:	m_strName( p_strName	)
		,	m_dCumulativeTimes( 0.0	)
		,	m_uiTotalExecutions( 0	)
	{
	}

	BenchCase::~BenchCase()
	{
	}

	void BenchCase::DoBench( std::string p_name, CallbackBench p_bench, uint64_t p_ui64Calls )
	{
		try
		{
			m_dCumulativeTimes = 0;
			m_uiTotalExecutions = 0;

			for ( uint64_t i = 0; i < p_ui64Calls; i++ )
			{
				m_preciseTimer.TimeMs();
				p_bench();
				m_dCumulativeTimes += m_preciseTimer.TimeMs();
				m_uiTotalExecutions++;
			}

			std::stringstream l_stream;
			l_stream.precision( 4 );
			l_stream << "*	" << p_name << " global results :" << std::endl;
			l_stream << "*		- Executed " << p_ui64Calls << " times" << std::endl;
			l_stream << "*		- Total time : %" << m_dCumulativeTimes << "s" << std::endl;
			l_stream << "*		- Average time : %" << ( 1000.0 * m_dCumulativeTimes / m_uiTotalExecutions ) << "ms" << std::endl;
			l_stream << "**************************************************" << std::endl;
			m_strSummary += l_stream.str();
			Logger::LogInfo( std::stringstream() << "*	Bench ended for: " << p_name.c_str() );
			Logger::LogInfo( StringStream() << cuT(	"*		- Executed " ) << p_ui64Calls << cuT( " times" ) );
			Logger::LogInfo( StringStream() << cuT(	"*		- Total time: " ) << m_dCumulativeTimes << cuT( "ms" ) );
			Logger::LogInfo( StringStream() << cuT(	"*		- Average time: " ) << ( m_dCumulativeTimes / m_uiTotalExecutions ) << cuT( "ms" ) );
			Logger::LogInfo( StringStream() << cuT(	"**************************************************" ) );
		}
		catch ( ... )
		{
			Logger::LogInfo( std::stringstream() << "*	Bench " << p_name.c_str() << " failed" );
			Logger::LogInfo( StringStream() << cuT(	"**************************************************" ) );
		}
	}
}
