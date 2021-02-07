#include "Castor3D/Render/RenderPassTimer.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Render/RenderLoop.hpp"
#include "Castor3D/Render/RenderSystem.hpp"

#include <ashespp/Command/CommandBuffer.hpp>
#include <ashespp/Core/Device.hpp>
#include <ashespp/Miscellaneous/QueryPool.hpp>

CU_ImplementCUSmartPtr( castor3d, RenderPassTimer )

#define C3D_DebugQueryLock 0

namespace castor3d
{
	//*********************************************************************************************

	RenderPassTimerBlock::RenderPassTimerBlock( RenderPassTimer & timer )
		: m_timer{ &timer }
	{
	}

	RenderPassTimerBlock::RenderPassTimerBlock( RenderPassTimerBlock && rhs )noexcept
		: m_timer{ rhs.m_timer }
	{
		rhs.m_timer = nullptr;
	}

	RenderPassTimerBlock & RenderPassTimerBlock::operator=( RenderPassTimerBlock && rhs )noexcept
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

	RenderPassTimer::RenderPassTimer( RenderDevice const & device
		, castor::String const & category
		, castor::String const & name
		, uint32_t passesCount )
		: castor::Named{ name }
		, m_engine{ *device.renderSystem.getEngine() }
		, m_device{ device }
		, m_passesCount{ passesCount }
		, m_category{ category }
		, m_timerQuery{ device->createQueryPool( name
			, VK_QUERY_TYPE_TIMESTAMP
			, 2u * passesCount
			, 0u ) }
		, m_cpuTime{ 0_ns }
		, m_gpuTime{ 0_ns }
		, m_startedPasses( size_t( m_passesCount ), { false, false } )
	{
		m_engine.getRenderLoop().registerTimer( *this );
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

	void RenderPassTimer::notifyPassRender( uint32_t passIndex
		, bool subtractGpuFromCpu )
	{
		auto & started = m_startedPasses[passIndex];
		started.first = true;
		started.second = subtractGpuFromCpu;
	}

	void RenderPassTimer::stop()
	{
		m_cpuTime += m_cpuTimer.getElapsed();
		m_cpuTime -= m_subtracteGpuTime;
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
		static float const period = float( m_device->getTimestampPeriod() );
		m_gpuTime = 0_ns;
		m_subtracteGpuTime = 0_ns;

		for ( uint32_t i = 0; i < m_passesCount; ++i )
		{
			auto & started = m_startedPasses[i];

			if ( started.first )
			{
#if C3D_DebugQueryLock
				ashes::UInt64Array valueswa{ 0u, 0u, 0u, 0u };
				bool ok{ false };
				uint32_t count = 0u;

				while ( count < 1000u && !ok )
				{
					try
					{
						m_timerQuery->getResults( i * 2u
							, 2u
							, 0u
							, VK_QUERY_RESULT_WITH_AVAILABILITY_BIT
							, valueswa );
						ok = true;
					}
					catch ( ashes::Exception & exc )
					{
						if ( exc.getResult() != VK_NOT_READY )
						{
							std::cerr << exc.what() << std::endl;
						}
					}

					std::this_thread::sleep_for( 1_ms );
					++count;
				}

				if ( count == 1000u )
				{
					std::cerr << "Couldn't retrieve RenderPassTimer [" << getCategory() << "] - ["  << getName() << "] results." << std::endl;
				}
				else
				{
#else
				try
				{
#endif
					ashes::UInt64Array values{ 0u, 0u };
					m_timerQuery->getResults( i * 2u
						, 2u
						, 0u
						, VK_QUERY_RESULT_WAIT_BIT
						, values );

					auto gpuTime = castor::Nanoseconds{ uint64_t( ( values[1] - values[0] ) / period ) };
					m_gpuTime += gpuTime;

					if ( started.second )
					{
						m_subtracteGpuTime += gpuTime;
					}
				}
#if !C3D_DebugQueryLock
				catch ( ashes::Exception & exc )
				{
					std::cerr << exc.what() << std::endl;
				}
#endif

				started.first = false;
				started.second = false;
			}
		}
	}

	void RenderPassTimer::updateCount( uint32_t count )
	{
		if ( m_passesCount != count )
		{
			m_passesCount = count;
			m_timerQuery = m_device->createQueryPool( VK_QUERY_TYPE_TIMESTAMP
				, 2u * m_passesCount
				, 0u );
			m_startedPasses.resize( m_passesCount );
		}
	}

	//*********************************************************************************************
}
