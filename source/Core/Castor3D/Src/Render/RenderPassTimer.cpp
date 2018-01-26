#include "RenderPassTimer.hpp"

#include "Engine.hpp"
#include "Render/RenderLoop.hpp"
#include "Render/RenderSystem.hpp"
#include "Miscellaneous/GpuQuery.hpp"

using namespace castor;

namespace castor3d
{
	RenderPassTimer::RenderPassTimer( Engine & engine
		, String const & category
		, String const & name )
		: Named{ name }
		, m_engine{ engine }
		, m_category{ category }
		, m_timerQuery
		{
			{
				engine.getRenderSystem()->createQuery( QueryType::eTime ),
				engine.getRenderSystem()->createQuery( QueryType::eTimeElapsed )
			}
		}
		, m_cpuTime{ 0_ns }
		, m_gpuTime{ 0_ns }
	{
		m_timerQuery[0]->initialise();
		m_timerQuery[1]->initialise();
		m_timerQuery[1 - m_queryIndex]->begin();
		m_timerQuery[1 - m_queryIndex]->end();
		engine.getRenderLoop().registerTimer( *this );
	}

	RenderPassTimer::~RenderPassTimer()
	{
		if ( m_engine.hasRenderLoop() )
		{
			m_engine.getRenderLoop().unregisterTimer( *this );
		}

		m_timerQuery[0]->cleanup();
		m_timerQuery[1]->cleanup();
		m_timerQuery[0].reset();
		m_timerQuery[1].reset();
	}

	void RenderPassTimer::start()
	{
		m_cpuTimer.getElapsed();
		m_timerQuery[m_queryIndex]->begin();
	}

	void RenderPassTimer::stop()
	{
		m_cpuTime = m_cpuTimer.getElapsed();
		m_timerQuery[m_queryIndex]->end();
		m_queryIndex = 1 - m_queryIndex;
		uint64_t time = 0;
		m_timerQuery[m_queryIndex]->getInfos( QueryInfo::eResult, time );
		m_gpuTime = Nanoseconds( time );
	}

	void RenderPassTimer::reset()
	{
		m_cpuTime = 0_ns;
		m_gpuTime = 0_ns;
	}
}
