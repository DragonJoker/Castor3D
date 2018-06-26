#include "RenderPassTimer.hpp"

#include "Engine.hpp"
#include "Render/RenderLoop.hpp"
#include "Render/RenderSystem.hpp"

#include <Core/Device.hpp>
#include <Miscellaneous/QueryPool.hpp>

using namespace castor;

namespace castor3d
{
	//*********************************************************************************************

	RenderPassTimerBlock::RenderPassTimerBlock( RenderPassTimer & timer )
		: m_timer{ &timer }
	{
	}

	RenderPassTimerBlock::RenderPassTimerBlock( RenderPassTimerBlock && rhs )
		: m_timer{ rhs.m_timer }
	{
		rhs.m_timer = nullptr;
	}

	RenderPassTimerBlock & RenderPassTimerBlock::operator=( RenderPassTimerBlock && rhs )
	{
		if ( this != &rhs )
		{
			m_timer = rhs.m_timer;
			rhs.m_timer = nullptr;
		}

		return *this;
	}

	RenderPassTimerBlock::~RenderPassTimerBlock()
	{
		if ( m_timer )
		{
			m_timer->stop();
		}
	}

	//*********************************************************************************************

	RenderPassTimer::RenderPassTimer( Engine & engine
		, String const & category
		, String const & name
		, uint32_t passesCount )
		: Named{ name }
		, m_engine{ engine }
		, m_passesCount{ passesCount }
		, m_category{ category }
		, m_timerQuery{ getCurrentDevice( engine ).createQueryPool( renderer::QueryType::eTimestamp
			, 2u * passesCount
			, 0u ) }
		, m_cpuTime{ 0_ns }
		, m_gpuTime{ 0_ns }
		, m_startedPasses( size_t( m_passesCount ), false )
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

	RenderPassTimerBlock RenderPassTimer::start()
	{
		m_cpuTimer.getElapsed();
		return RenderPassTimerBlock{ *this };
	}

	void RenderPassTimer::notifyPassRender( uint32_t passIndex )
	{
		m_startedPasses[passIndex] = true;
	}

	void RenderPassTimer::stop()
	{
		m_cpuTime += m_cpuTimer.getElapsed();
	}

	void RenderPassTimer::reset()
	{
		m_cpuTime = 0_ns;
		m_gpuTime = 0_ns;
	}

	void RenderPassTimer::beginPass( renderer::CommandBuffer const & cmd
		, uint32_t passIndex )const
	{
		REQUIRE( passIndex < m_passesCount );
		cmd.resetQueryPool( *m_timerQuery
			, passIndex * 2u
			, 2u );
		cmd.writeTimestamp( renderer::PipelineStageFlag::eBottomOfPipe
			, *m_timerQuery
			, passIndex * 2u + 0u );
	}

	void RenderPassTimer::endPass( renderer::CommandBuffer const & cmd
		, uint32_t passIndex )const
	{
		REQUIRE( passIndex < m_passesCount );
		cmd.writeTimestamp( renderer::PipelineStageFlag::eBottomOfPipe
			, *m_timerQuery
			, passIndex * 2u + 1u );
	}

	void RenderPassTimer::retrieveGpuTime()
	{
		static float const period = float( getCurrentDevice( m_engine ).getTimestampPeriod() );
		m_gpuTime = Nanoseconds{};

		for ( uint32_t i = 0; i < m_passesCount; ++i )
		{
			if ( m_startedPasses[i] )
			{
				renderer::UInt64Array values{ 0u, 0u };
				m_timerQuery->getResults( i * 2u
					, 2u
					, 0u
					, renderer::QueryResultFlag::eWait
					, values );
				m_gpuTime += Nanoseconds{ uint64_t( ( values[1] - values[0] ) / period ) };
				m_startedPasses[i] = false;
			}
		}
	}

	void RenderPassTimer::updateCount( uint32_t count )
	{
		m_passesCount = count;
		m_timerQuery = getCurrentDevice( m_engine ).createQueryPool( renderer::QueryType::eTimestamp
			, 2u * m_passesCount
			, 0u );
		m_startedPasses.resize( m_passesCount );
	}

	//*********************************************************************************************
}
