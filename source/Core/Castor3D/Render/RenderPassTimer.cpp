#include "Castor3D/Render/RenderPassTimer.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Render/RenderLoop.hpp"
#include "Castor3D/Render/RenderSystem.hpp"

#include <ashespp/Core/Device.hpp>
#include <ashespp/Miscellaneous/QueryPool.hpp>

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
		, m_timerQuery{ getCurrentRenderDevice( engine )->createQueryPool( VK_QUERY_TYPE_TIMESTAMP
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

	void RenderPassTimer::beginPass( ashes::CommandBuffer const & cmd
		, uint32_t passIndex )const
	{
		CU_Require( passIndex < m_passesCount );
		cmd.resetQueryPool( *m_timerQuery
			, passIndex * 2u
			, 2u );
		cmd.writeTimestamp( VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT
			, *m_timerQuery
			, passIndex * 2u + 0u );
	}

	void RenderPassTimer::endPass( ashes::CommandBuffer const & cmd
		, uint32_t passIndex )const
	{
		CU_Require( passIndex < m_passesCount );
		cmd.writeTimestamp( VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT
			, *m_timerQuery
			, passIndex * 2u + 1u );
	}

	void RenderPassTimer::retrieveGpuTime()
	{
		static float const period = float( getCurrentRenderDevice( m_engine )->getTimestampPeriod() );
		m_gpuTime = 0_ns;

		for ( uint32_t i = 0; i < m_passesCount; ++i )
		{
			if ( m_startedPasses[i] )
			{
				try
				{
					ashes::UInt64Array values{ 0u, 0u, 0u, 0u };
					m_timerQuery->getResults( i * 2u
						, 2u
						, 0u
						, VK_QUERY_RESULT_WAIT_BIT
						, values );

					if ( values[1] && values[3] )
					{
						m_gpuTime += Nanoseconds{ uint64_t( ( values[2] - values[0] ) / period ) };

					}
				}
				catch ( ashes::Exception & exc )
				{
					std::cerr << exc.what() << std::endl;
				}

				m_startedPasses[i] = false;
			}
		}
	}

	void RenderPassTimer::updateCount( uint32_t count )
	{
		if ( m_passesCount != count )
		{
			m_passesCount = count;
			m_timerQuery = getCurrentRenderDevice( m_engine )->createQueryPool( VK_QUERY_TYPE_TIMESTAMP
				, 2u * m_passesCount
				, 0u );
			m_startedPasses.resize( m_passesCount );
		}
	}

	//*********************************************************************************************
}
