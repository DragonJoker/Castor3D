#include "Castor3D/Render/Clustered/ReduceLightsAABB.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/LightCache.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/Clustered/FrustumClusters.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/Light/PointLight.hpp"
#include "Castor3D/Scene/Light/SpotLight.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/Shaders/GlslAABB.hpp"
#include "Castor3D/Shader/Shaders/GlslAppendBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslClusteredLights.hpp"
#include "Castor3D/Shader/Shaders/GlslLight.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Ubos/CameraUbo.hpp"
#include "Castor3D/Shader/Ubos/ClustersUbo.hpp"

#include <CastorUtils/Design/DataHolder.hpp>

#include <ShaderWriter/Source.hpp>

#include <RenderGraph/FramePassGroup.hpp>
#include <RenderGraph/RunnablePasses/ComputePass.hpp>

#include <limits>

#define C3D_DebugEnableWarpOptimisation 0

namespace castor3d
{
	//*********************************************************************************************

	namespace rdclgb
	{
		enum BindingPoints
		{
			eCamera,
			eClusters,
			eAllLightsAABB,
			eReducedLightsAABB,
		};

		static uint32_t constexpr NumThreads = 512u;
		static float constexpr FltMax = std::numeric_limits< float >::max();

		static ShaderPtr createShader( RenderDevice const & device
			, ClustersConfig const & config
			, bool first )
		{
			sdw::ComputeWriter writer{ &device.renderSystem.getEngine()->getShaderAllocator() };

			// Inputs
			C3D_Camera( writer
				, eCamera
				, 0u );
			C3D_Clusters( writer
				, eClusters
				, 0u
				, &config );
			C3D_AllLightsAABBEx( writer
				, eAllLightsAABB
				, 0u
				, first );
			C3D_ReducedLightsAABB( writer
				, eReducedLightsAABB
				, 0u );

			sdw::PushConstantBuffer pcb{ writer, "C3D_DispatchData", "c3d_dispatchData" };
			auto c3d_numThreadGroups = pcb.declMember< sdw::UInt >( "numThreadGroups" );
			auto c3d_reduceNumElements = pcb.declMember< sdw::UInt >( "reduceNumElements" );
			pcb.end();

			auto gsAABBMin = writer.declSharedVariable< sdw::Vec4 >( "gsAABBMin", NumThreads );
			auto gsAABBMax = writer.declSharedVariable< sdw::Vec4 >( "gsAABBMax", NumThreads );

			// Perform log-step reduction in group shared memory.
			auto logStepReduction = [&]( sdw::UInt const & groupIndex
				, sdw::UInt const & groupID )
			{
				// If we can assume that NUM_THREADS is a power of 2, we can compute
				// the reduction index by performing a bit shift. This is equivalent to 
				// halving the number of values (and threads) that must perform the reduction
				// operation.
				auto reduceIndex = NumThreads >> 1u;

				if ( config.enableReduceWarpOptimisation )
				{
					while ( reduceIndex > 32u )
					{
						IF( writer, groupIndex < reduceIndex )
						{
							gsAABBMin[groupIndex] = min( gsAABBMin[groupIndex], gsAABBMin[groupIndex + reduceIndex] );
							gsAABBMax[groupIndex] = max( gsAABBMax[groupIndex], gsAABBMax[groupIndex + reduceIndex] );
						}
						FI;

						// Sync group shared memory writes.
						shader::groupMemoryBarrierWithGroupSync( writer );

						// Halve the number of threads that participate in the reduction.
						reduceIndex >>= 1u;
					}

					// Within a warp (of 32 threads), instructions are warp-synchronous
					// and the GroupMemoryBarrierWithGroupSync() is no longer needed to ensure
					// the previous writes to groups shared memory have completed.
					// Source: DirectCompute Optimizations and Best Practices (2010), Eric Young.
					// Source: The CUDA Handbook (2013), Nicholas Wilt
					IF( writer, groupIndex < 32_u )
					{
						while ( reduceIndex > 0u )
						{
							// To avoid out-of-bounds memory access, the number of threads in the 
							// group must be at least 2x the reduce index. For example, the 
							// thread at index 31 will access elements 31 and 63 so the size of the thread group
							// must be at least 64.
							if ( NumThreads >= ( reduceIndex << 1u ) )
							{
								gsAABBMin[groupIndex] = min( gsAABBMin[groupIndex], gsAABBMin[groupIndex + reduceIndex] );
								gsAABBMax[groupIndex] = max( gsAABBMax[groupIndex], gsAABBMax[groupIndex + reduceIndex] );
							}

							reduceIndex >>= 1u;
						}

						IF( writer, groupIndex == 0_u )
						{
							c3d_reducedLightsAABB[groupID] = shader::AABB{ gsAABBMin[groupIndex], gsAABBMax[groupIndex] };
						}
						FI;
					}
					FI;
				}
				else
				{
					while ( reduceIndex > 0u )
					{
						IF( writer, groupIndex < reduceIndex )
						{
							gsAABBMin[groupIndex] = min( gsAABBMin[groupIndex], gsAABBMin[groupIndex + reduceIndex] );
							gsAABBMax[groupIndex] = max( gsAABBMax[groupIndex], gsAABBMax[groupIndex + reduceIndex] );
						}
						FI;

						// Sync group shared memory writes.
						shader::groupMemoryBarrierWithGroupSync( writer );

						// Halve the number of threads that participate in the reduction.
						reduceIndex >>= 1u;
					}

					IF( writer, groupIndex == 0_u )
					{
						c3d_reducedLightsAABB[groupID] = shader::AABB{ gsAABBMin[groupIndex], gsAABBMax[groupIndex] };
					}
					FI;
				}
			};

			writer.implementMainT< sdw::VoidT >( NumThreads
				, [&]( sdw::ComputeIn in )
				{
					auto groupIndex = in.localInvocationIndex;
					auto groupID = in.workGroupID.x();
					auto threadIndex = in.globalInvocationID.x();

					auto aabbMin = writer.declLocale( "aabbMin"
						, vec4( sdw::Float{ FltMax }, FltMax, FltMax, 1.0f ) );
					auto aabbMax = writer.declLocale( "aabbMax"
						, vec4( sdw::Float{ -FltMax }, -FltMax, -FltMax, 1.0f ) );

					IF( writer, groupIndex == 0_u )
					{
						FOR( writer, sdw::UInt, n, 0_u, n < NumThreads, ++n )
						{
							gsAABBMin[n] = aabbMin;
							gsAABBMax[n] = aabbMax;
						}
						ROF;
					}
					FI;

					shader::groupMemoryBarrierWithGroupSync( writer );

					if ( first )
					{
						// The 1st pass of the reduction operates on the light buffers.
						// First compute point lights AABB.
						IF( writer, threadIndex < c3d_clustersData.pointLightCount() )
						{
							auto aabb = c3d_allLightsAABB[threadIndex];

							aabbMin = min( aabbMin, aabb.min() );
							aabbMax = max( aabbMax, aabb.max() );
						}
						FI;

						// Next, expand AABB for spot lights.
						IF( writer, threadIndex < c3d_clustersData.spotLightCount() )
						{
							auto aabb = c3d_allLightsAABB[c3d_clustersData.pointLightCount() + threadIndex];

							aabbMin = min( aabbMin, aabb.min() );
							aabbMax = max( aabbMax, aabb.max() );
						}
						FI;

						gsAABBMin[groupIndex] = aabbMin;
						gsAABBMax[groupIndex] = aabbMax;

						// Sync group shared memory writes.
						shader::groupMemoryBarrierWithGroupSync( writer );

						// Perform log-step reduction to allow each thread group in the dispatch
						// to reduce to a single element.
						logStepReduction( groupIndex, groupID );
					}
					else
					{
						// The subsequent passes of the reduction operate on the global AABB computed 
						// in previous pass.
						// This step is repeated until we are reduced to a single thread group.
						FOR( writer, sdw::UInt, i, groupIndex, i < c3d_reduceNumElements, i += NumThreads * c3d_numThreadGroups )
						{
							aabbMin = min( aabbMin, c3d_reducedLightsAABB[i].min() );
							aabbMax = max( aabbMax, c3d_reducedLightsAABB[i].max() );
						}
						ROF;

						gsAABBMin[groupIndex] = aabbMin;
						gsAABBMax[groupIndex] = aabbMax;

						// Sync group shared memory writes.
						shader::groupMemoryBarrierWithGroupSync( writer );

						// Perform log-step reduction to allow each thread group in the dispatch
						// to reduce to a single element. If there was only a single thread group
						// in this dispatch, then this will reduce to a single element.
						logStepReduction( groupIndex, groupID );
					}

					IF( writer, groupIndex == 0_u )
					{
						auto lightsMin = writer.declLocale( "lightsMin"
							, gsAABBMin[groupIndex] );
						auto lightsMax = writer.declLocale( "lightsMax"
							, gsAABBMax[groupIndex] );
						auto clustersLightsData = writer.declLocale< sdw::Vec4 >( "clustersLightsData" );
						auto lightsAABBRange = writer.declLocale< sdw::Vec4 >( "lightsAABBRange" );
						c3d_clustersData.computeGlobalLightsData( lightsMin
							, lightsMax
							, c3d_cameraData.nearPlane()
							, c3d_cameraData.farPlane()
							, clustersLightsData
							, lightsAABBRange );
						c3d_clustersLightsData = clustersLightsData;
						c3d_lightsAABBRange = lightsAABBRange;
					}
					FI;
				} );
			return writer.getBuilder().releaseShader();
		}

		static u32 computeThreadGroupsCount( LightCache const & cache )
		{
			auto pointLightsCount = cache.getLightsBufferCount( LightType::ePoint );
			auto spoLightsCount = cache.getLightsBufferCount( LightType::eSpot );
			auto maxLightsCount = std::max( pointLightsCount, spoLightsCount );

			// Don't dispatch more than 512 thread groups. The reduction algorithm depends on the
			// number of thread groups to be no more than 512. The buffer which stores the reduced AABB is sized
			// for a maximum of 512 thread groups.
			return std::min( 512u, castor::divRoundUp( maxLightsCount, 512u ) );
		}

		class FirstFramePass
			: public crg::ComputePass
		{
		public:
			FirstFramePass( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph
				, RenderDevice const & device
				, FrustumClusters const & clusters
				, crg::cp::Config config )
				: crg::ComputePass{ framePass
					, context
					, graph
					, crg::ru::Config{ 4u }
					, config
						.isEnabled( IsEnabledCallback( [this](){ return doIsEnabled(); } ) )
						.getPassIndex( GetPassIndexCallback( [this]() { return doGetPassIndex(); } ) )
						.programCreator( { 4u, [this]( uint32_t passIndex ){ return doCreateProgram( passIndex ); } } )
						.recordInto( RunnablePass::RecordCallback( [this]( crg::RecordContext & ctx, VkCommandBuffer cmd, uint32_t idx ){ doSubRecordInto( ctx, cmd, idx ); } ) )
						.end( RecordCallback{ [this]( crg::RecordContext & ctx, VkCommandBuffer cb, uint32_t idx ) { doPostRecord( ctx, cb, idx ); } } )
						.pushConstants( VkPushConstantRange{ VK_SHADER_STAGE_COMPUTE_BIT, 0u, 8u } )
						.getGroupCountX( crg::cp::GetGroupCountCallback( [this]() { return doGetGroupsCountX(); } ) ) }
				, m_device{ device }
				, m_clusters{ clusters }
				, m_lightCache{ m_clusters.getCamera().getScene()->getLightCache() }
			{
			}

		private:
			struct ProgramData
			{
				ShaderModule module;
				ashes::PipelineShaderStageCreateInfoArray stages;
			};

		private:
			RenderDevice const & m_device;
			FrustumClusters const & m_clusters;
			LightCache const & m_lightCache;
			std::map< uint32_t, ProgramData > m_programs;
			uint32_t m_dispatchCount{};

		private:
			crg::VkPipelineShaderStageCreateInfoArray doCreateProgram( uint32_t passIndex )
			{
				auto ires = m_programs.emplace( passIndex, ProgramData{} );

				if ( ires.second )
				{
					auto & program = ires.first->second;
					program.module = ShaderModule{ VK_SHADER_STAGE_COMPUTE_BIT, "ReduceLightsAABB/First", createShader( m_device, m_clusters.getConfig(), true ) };
					program.stages = ashes::PipelineShaderStageCreateInfoArray{ makeShaderState( m_device, program.module ) };
				}

				return ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( ires.first->second.stages );
			}

			bool doIsEnabled()const
			{
				return m_clusters.getConfig().enabled
					&& m_clusters.needsLightsUpdate();
			}

			uint32_t doGetPassIndex()
			{
				uint32_t result{ ( m_clusters.getConfig().limitClustersToLightsAABB ? 1u : 0u )
					+ ( m_clusters.getConfig().enableReduceWarpOptimisation ? 2u : 0u ) };
				uint32_t count{ computeThreadGroupsCount( m_lightCache ) };

				if ( m_dispatchCount && count != m_dispatchCount )
				{
					setToReset( result );
				}

				return result;
			}

			void doSubRecordInto( crg::RecordContext & context
				, VkCommandBuffer commandBuffer
				, uint32_t index )
			{
				struct
				{
					uint32_t numThreadGroups{};
					uint32_t reduceNumElements{};
				} dispatchData;

				// In the first pass, the number of lights determines the number of
				// elements to be reduced.
				m_dispatchCount = computeThreadGroupsCount( m_lightCache );
				dispatchData.numThreadGroups = m_dispatchCount;
				m_context.vkCmdPushConstants( commandBuffer, getPipelineLayout(), VK_SHADER_STAGE_COMPUTE_BIT, 0u, 8u, &dispatchData );
			}

			void doPostRecord( crg::RecordContext & context
				, VkCommandBuffer commandBuffer
				, uint32_t index )
			{
				if ( computeThreadGroupsCount( m_lightCache ) <= 1u )
				{
					auto & attach = m_pass.buffers.back();
					auto & buffer = attach.buffer;

					auto currentState = context.getAccessState( buffer.buffer.buffer( index )
						, buffer.range );
					context.memoryBarrier( commandBuffer
						, buffer.buffer.buffer( index )
						, buffer.range
						, currentState.access
						, currentState.pipelineStage
						, { VK_ACCESS_SHADER_READ_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT } );
				}
			}

			uint32_t doGetGroupsCountX()const
			{
				return computeThreadGroupsCount( m_lightCache );
			}
		};

		class SecondFramePass
			: public crg::ComputePass
		{
		public:
			SecondFramePass( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph
				, RenderDevice const & device
				, FrustumClusters const & clusters
				, crg::cp::Config config )
				: crg::ComputePass{ framePass
					, context
					, graph
					, crg::ru::Config{ 4u }
					, config
						.isEnabled( IsEnabledCallback( [this](){ return doIsEnabled(); } ) )
						.getPassIndex( GetPassIndexCallback( [this]() { return doGetPassIndex(); } ) )
						.programCreator( { 4u, [this]( uint32_t passIndex ){ return doCreateProgram( passIndex ); } } )
						.recordInto( RunnablePass::RecordCallback( [this]( crg::RecordContext & ctx, VkCommandBuffer cmd, uint32_t idx ){ doSubRecordInto( ctx, cmd, idx ); } ) )
						.end( RecordCallback{ [this]( crg::RecordContext & ctx, VkCommandBuffer cb, uint32_t idx ) { doPostRecord( ctx, cb, idx ); } } )
						.pushConstants( VkPushConstantRange{ VK_SHADER_STAGE_COMPUTE_BIT, 0u, 8u } ) }
				, m_device{ device }
				, m_clusters{ clusters }
				, m_lightCache{ m_clusters.getCamera().getScene()->getLightCache() }
			{
			}

		private:
			struct ProgramData
			{
				ShaderModule module;
				ashes::PipelineShaderStageCreateInfoArray stages;
			};

		private:
			RenderDevice const & m_device;
			FrustumClusters const & m_clusters;
			LightCache const & m_lightCache;
			std::map< uint32_t, ProgramData > m_programs;

		private:
			crg::VkPipelineShaderStageCreateInfoArray doCreateProgram( uint32_t passIndex )
			{
				auto ires = m_programs.emplace( passIndex, ProgramData{} );

				if ( ires.second )
				{
					auto & program = ires.first->second;
					program.module = ShaderModule{ VK_SHADER_STAGE_COMPUTE_BIT, "ReduceLightsAABB/Second", createShader( m_device, m_clusters.getConfig(), false ) };
					program.stages = ashes::PipelineShaderStageCreateInfoArray{ makeShaderState( m_device, program.module ) };
				}

				return ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( ires.first->second.stages );
			}

			uint32_t doGetPassIndex()
			{
				return ( m_clusters.getConfig().limitClustersToLightsAABB ? 1u : 0u )
					+ ( m_clusters.getConfig().enableReduceWarpOptimisation ? 2u : 0u );
			}

			bool doIsEnabled()const
			{
				return m_clusters.getConfig().enabled
					&& m_clusters.needsLightsUpdate();
			}

			void doSubRecordInto( crg::RecordContext & context
				, VkCommandBuffer commandBuffer
				, uint32_t index )
			{
				struct
				{
					uint32_t numThreadGroups{};
					uint32_t reduceNumElements{};
				} dispatchData;

				// In the second pass, the number of elements to be reduced is the 
				// number of thread groups from the first pass.
				dispatchData.reduceNumElements = computeThreadGroupsCount( m_lightCache );
				dispatchData.numThreadGroups = 1u;

				m_context.vkCmdPushConstants( commandBuffer, getPipelineLayout(), VK_SHADER_STAGE_COMPUTE_BIT, 0u, 8u, &dispatchData );
			}

			void doPostRecord( crg::RecordContext & context
				, VkCommandBuffer commandBuffer
				, uint32_t index )
			{
				auto & attach = m_pass.buffers.back();
				auto & buffer = attach.buffer;

				auto currentState = context.getAccessState( buffer.buffer.buffer( index )
					, buffer.range );
				context.memoryBarrier( commandBuffer
					, buffer.buffer.buffer( index )
					, buffer.range
					, currentState.access
					, currentState.pipelineStage
					, { VK_ACCESS_SHADER_READ_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT } );
			}
		};
	}

	//*********************************************************************************************

	crg::FramePass const & createReduceLightsAABBPass( crg::FramePassGroup & graph
		, crg::FramePass const * previousPass
		, RenderDevice const & device
		, CameraUbo const & cameraUbo
		, FrustumClusters & clusters )
	{
		auto & first = graph.createPass( "ReduceLightsAABB/First"
			, [&clusters, &device]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				auto result = std::make_unique< rdclgb::FirstFramePass >( framePass
					, context
					, graph
					, device
					, clusters
					, crg::cp::Config{} );
				device.renderSystem.getEngine()->registerTimer( framePass.getFullName()
					, result->getTimer() );
				return result;
			} );
		first.addDependency( *previousPass );
		cameraUbo.createPassBinding( first, rdclgb::eCamera );
		clusters.getClustersUbo().createPassBinding( first, rdclgb::eClusters );
		createInputStoragePassBinding( first, uint32_t( rdclgb::eAllLightsAABB ), "C3D_AllLightsAABB", clusters.getAllLightsAABBBuffer(), 0u, ashes::WholeSize );
		createClearableOutputStorageBinding( first, uint32_t( rdclgb::eReducedLightsAABB ), "C3D_ReducedLightsAABB", clusters.getReducedLightsAABBBuffer(), 0u, ashes::WholeSize );

		auto & second = graph.createPass( "ReduceLightsAABB/Second"
			, [&clusters, &device]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				auto result = std::make_unique< rdclgb::SecondFramePass >( framePass
					, context
					, graph
					, device
					, clusters
					, crg::cp::Config{} );
				device.renderSystem.getEngine()->registerTimer( framePass.getFullName()
					, result->getTimer() );
				return result;
			} );
		second.addDependency( first );
		cameraUbo.createPassBinding( second, rdclgb::eCamera );
		clusters.getClustersUbo().createPassBinding( second, rdclgb::eClusters );
		createInOutStoragePassBinding( second, uint32_t( rdclgb::eReducedLightsAABB ), "C3D_ReducedLightsAABB", clusters.getReducedLightsAABBBuffer(), 0u, ashes::WholeSize );

		return second;
	}

	//*********************************************************************************************
}
