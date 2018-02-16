#include "RenderPassTimer.hpp"

#include "Engine.hpp"
#include "Render/RenderLoop.hpp"
#include "Render/RenderSystem.hpp"

#include <Core/Device.hpp>
#include <Miscellaneous/QueryPool.hpp>

using namespace castor;

namespace castor3d
{
	RenderPassTimer::RenderPassTimer( Engine & engine
		, String const & category
		, String const & name )
		: Named{ name }
		, m_engine{ engine }
		, m_category{ category }
		, m_timerQuery{ engine.getRenderSystem()->getCurrentDevice()->createQueryPool( renderer::QueryType::eTimestamp, 2u, 0u ) }
		, m_cpuTime{ 0_ns }
		, m_gpuTime{ 0_ns }
	{
		engine.getRenderLoop().registerTimer( *this );
	}

	RenderPassTimer::~RenderPassTimer()
	{
		if ( m_engine.hasRenderLoop() )
		{
			m_engine.getRenderLoop().unregisterTimer( *this );
		}

		m_timerQuery.reset();
	}

	void RenderPassTimer::start()
	{
		m_cpuTimer.getElapsed();
	}

	void RenderPassTimer::stop()
	{
		m_cpuTime = m_cpuTimer.getElapsed();
		renderer::UInt32Array values{ 0u, 0u };
		m_timerQuery->getResults( 0u
			, 2u
			, 0u
			, renderer::QueryResultFlag::eWait
			, values );
		m_gpuTime = Nanoseconds{ uint64_t( ( values[1] - values[0] ) / float( m_engine.getRenderSystem()->getCurrentDevice()->getTimestampPeriod() ) ) };
	}

	void RenderPassTimer::reset()
	{
		m_cpuTime = 0_ns;
		m_gpuTime = 0_ns;
	}
}
