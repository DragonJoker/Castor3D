#include "RenderPassTimer.hpp"

#include "Engine.hpp"
#include "Render/RenderLoop.hpp"
#include "Render/RenderSystem.hpp"
#include "Miscellaneous/GpuQuery.hpp"

using namespace Castor;

namespace Castor3D
{
	RenderPassTimer::RenderPassTimer( Engine & engine
		, String const & name )
		: Named{ name }
		, m_engine{ engine }
		, m_timerQuery
		{
			{
				engine.GetRenderSystem()->CreateQuery( QueryType::eTimeElapsed ),
				engine.GetRenderSystem()->CreateQuery( QueryType::eTimeElapsed )
			}
		}
		, m_cpuTime{ 0_ns }
		, m_gpuTime{ 0_ns }
	{
		m_timerQuery[0]->Initialise();
		m_timerQuery[1]->Initialise();
		m_timerQuery[1 - m_queryIndex]->Begin();
		m_timerQuery[1 - m_queryIndex]->End();
		engine.GetRenderLoop().RegisterTimer( *this );
	}

	RenderPassTimer::~RenderPassTimer()
	{
		m_timerQuery[0]->Cleanup();
		m_timerQuery[1]->Cleanup();
		m_timerQuery[0].reset();
		m_timerQuery[1].reset();
	}

	void RenderPassTimer::Start()
	{
		m_cpuTimer.Time();
		m_timerQuery[m_queryIndex]->Begin();
	}

	void RenderPassTimer::Stop()
	{
		m_cpuTime = m_cpuTimer.Time();
		m_timerQuery[m_queryIndex]->End();
		m_queryIndex = 1 - m_queryIndex;
		uint64_t time = 0;
		m_timerQuery[m_queryIndex]->GetInfos( QueryInfo::eResult, time );
		m_gpuTime = Nanoseconds( time );
	}
}
