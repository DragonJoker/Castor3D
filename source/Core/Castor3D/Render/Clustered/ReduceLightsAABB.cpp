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

namespace c3d
{
	//*********************************************************************************************

	namespace rdclgb
	{
		enum class Bindings
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
				, Bindings::eCamera
				, 0u );
			C3D_Clusters( writer
				, Bindings::eClusters
				, 0u );
			C3D_AllLightsAABBEx( writer
				, Bindings::eAllLightsAABB
				, 0u
				, first );
			C3D_ReducedLightsAABB( writer
				, Bindings::eReducedLightsAABB
				, 0u );

			sdw::PushConstantBuffer pcb{ writer, "C3D_DispatchData", "c3d_dispatchData" };
			auto c3d_numThreadGroups = pcb.declMember< sdw::UInt >( "numThreadGroups" );
			auto c3d_reduceNumElements = pcb.declMember< sdw::UInt >( "reduceNumElements" );
			pcb.end();

			auto gsAABBMin = writer.declSharedVariable< sdw::Vec4 >( "gsAABBMin", NumThreads );
			auto gsAABBMax = writer.declSharedVariable< sdw::Vec4 >( "gsAABBMax", NumThreads );

			// Perform log-step reduction in group shared memory.
			auto logStepReduction = [&writer, &c3d_reducedLightsAABB, &gsAABBMin, &gsAABBMax
				, &config]( sdw::UInt const & groupIndex
					, sdw::UInt const & groupID )
			{
				// If we can assume that NumThreads is a power of 2, we can compute
				// the reduction index by performing a bit shift. This is equivalent to 
				// halving the number of values (and threads) that must perform the reduction
				// operation.
				auto reduceIndex = NumThreads >> 1u;

				if ( config.enableReduceWarpOptimisation )
				{
					while ( reduceIndex > 32u )
					{
						sdwIF( writer, groupIndex < reduceIndex )
						{
							gsAABBMin[groupIndex] = min( gsAABBMin[groupIndex], gsAABBMin[groupIndex + reduceIndex] );
							gsAABBMax[groupIndex] = max( gsAABBMax[groupIndex], gsAABBMax[groupIndex + reduceIndex] );
						}
						sdwFI

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
					sdwIF( writer, groupIndex < 32_u )
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

						sdwIF( writer, groupIndex == 0_u )
						{
							c3d_reducedLightsAABB[groupID] = shader::AABB{ gsAABBMin[groupIndex], gsAABBMax[groupIndex] };
						}
						sdwFI
					}
					sdwFI
				}
				else
				{
					while ( reduceIndex > 0u )
					{
						sdwIF( writer, groupIndex < reduceIndex )
						{
							gsAABBMin[groupIndex] = min( gsAABBMin[groupIndex], gsAABBMin[groupIndex + reduceIndex] );
							gsAABBMax[groupIndex] = max( gsAABBMax[groupIndex], gsAABBMax[groupIndex + reduceIndex] );
						}
						sdwFI

						// Sync group shared memory writes.
						shader::groupMemoryBarrierWithGroupSync( writer );

						// Halve the number of threads that participate in the reduction.
						reduceIndex >>= 1u;
					}

					sdwIF( writer, groupIndex == 0_u )
					{
						c3d_reducedLightsAABB[groupID] = shader::AABB{ gsAABBMin[groupIndex], gsAABBMax[groupIndex] };
					}
					sdwFI
				}
			};

			writer.implementMainT< sdw::VoidT >( NumThreads
				, [&writer, &c3d_allLightsAABB, &c3d_clustersData, &c3d_reducedLightsAABB, &c3d_numThreadGroups, &c3d_clustersLightsData
					, &c3d_lightsAABBRange, &c3d_cameraData, &c3d_reduceNumElements, &gsAABBMin, &gsAABBMax, &logStepReduction
					, first]( sdw::ComputeIn const & in )
				{
					auto const & groupIndex = in.localInvocationIndex;
					auto const & groupID = in.workGroupID.x();
					auto const & threadIndex = in.globalInvocationID.x();

					auto aabbMin = writer.declLocale( "aabbMin"
						, vec4( sdw::Float{ FltMax }, FltMax, FltMax, 1.0f ) );
					auto aabbMax = writer.declLocale( "aabbMax"
						, vec4( sdw::Float{ -FltMax }, -FltMax, -FltMax, 1.0f ) );

					sdwIF( writer, groupIndex == 0_u )
					{
						sdwFOR( writer, sdw::UInt, n, 0_u, n < NumThreads, ++n )
						{
							gsAABBMin[n] = aabbMin;
							gsAABBMax[n] = aabbMax;
						}
						sdwROF
					}
					sdwFI

					shader::groupMemoryBarrierWithGroupSync( writer );

					if ( first )
					{
						// The 1st pass of the reduction operates on the light buffers.
						// First compute point lights AABB.
						sdwIF( writer, threadIndex < c3d_clustersData.pointLightCount() )
						{
							auto aabb = c3d_allLightsAABB[threadIndex];

							aabbMin = min( aabbMin, aabb.min() );
							aabbMax = max( aabbMax, aabb.max() );
						}
						sdwFI

						// Next, expand AABB for spot lights.
						sdwIF( writer, threadIndex < c3d_clustersData.spotLightCount() )
						{
							auto aabb = c3d_allLightsAABB[c3d_clustersData.pointLightCount() + threadIndex];

							aabbMin = min( aabbMin, aabb.min() );
							aabbMax = max( aabbMax, aabb.max() );
						}
						sdwFI

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
						sdwFOR( writer, sdw::UInt, i, groupIndex, i < c3d_reduceNumElements, i += NumThreads * c3d_numThreadGroups )
						{
							aabbMin = min( aabbMin, c3d_reducedLightsAABB[i].min() );
							aabbMax = max( aabbMax, c3d_reducedLightsAABB[i].max() );
						}
						sdwROF

						gsAABBMin[groupIndex] = aabbMin;
						gsAABBMax[groupIndex] = aabbMax;

						// Sync group shared memory writes.
						shader::groupMemoryBarrierWithGroupSync( writer );

						// Perform log-step reduction to allow each thread group in the dispatch
						// to reduce to a single element. If there was only a single thread group
						// in this dispatch, then this will reduce to a single element.
						logStepReduction( groupIndex, groupID );
					}

					sdwIF( writer, groupIndex == 0_u )
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
					sdwFI
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
			return std::min( 512u, divRoundUp( maxLightsCount, 512u ) );
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
					, crg::ru::Config{ 2u }
					, config
						.isEnabled( IsEnabledCallback( [this](){ return doIsEnabled(); } ) )
						.getPassIndex( GetPassIndexCallback( [this]() { return doGetPassIndex(); } ) )
						.programCreator( { 2u, [this]( uint32_t passIndex ){ return doCreateProgram( passIndex ); } } )
						.recordInto( RunnablePass::RecordCallback( [this]( crg::RecordContext const & ctx, VkCommandBuffer cmd, uint32_t ){ doSubRecordInto( ctx, cmd ); } ) )
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
				ProgramData() = default;
				ShaderModule shaderModule{};
				ashes::PipelineShaderStageCreateInfoArray stages{};
			};

		private:
			RenderDevice const & m_device;
			FrustumClusters const & m_clusters;
			LightCache const & m_lightCache;
			Map< uint32_t, ProgramData > m_programs;
			uint32_t m_dispatchCount{};

		private:
			crg::VkPipelineShaderStageCreateInfoArray doCreateProgram( uint32_t passIndex )
			{
				auto [it, res] = m_programs.try_emplace( passIndex );

				if ( res )
				{
					auto & program = it->second;
					program.shaderModule = ShaderModule{ VK_SHADER_STAGE_COMPUTE_BIT, cuT( "ReduceLightsAABB/First" ), createShader( m_device, m_clusters.getConfig(), true ) };
					program.stages = ashes::PipelineShaderStageCreateInfoArray{ makeShaderState( m_device, program.shaderModule ) };
				}

				return ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( it->second.stages );
			}

			bool doIsEnabled()const
			{
				return m_clusters.getConfig().enabled
					&& m_clusters.needsLightsUpdate();
			}

			uint32_t doGetPassIndex()const
			{
				uint32_t result{ ( m_clusters.getConfig().enableReduceWarpOptimisation ? 1u : 0u ) };
				return result;
			}

			void doSubRecordInto( crg::RecordContext const & context
				, VkCommandBuffer commandBuffer )
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
				context->vkCmdPushConstants( commandBuffer, getPipelineLayout(), VK_SHADER_STAGE_COMPUTE_BIT, 0u, 8u, &dispatchData );
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
					, crg::ru::Config{ 2u }
					, config
						.isEnabled( IsEnabledCallback( [this](){ return doIsEnabled(); } ) )
						.getPassIndex( GetPassIndexCallback( [this]() { return doGetPassIndex(); } ) )
						.programCreator( { 2u, [this]( uint32_t passIndex ){ return doCreateProgram( passIndex ); } } )
						.recordInto( RunnablePass::RecordCallback( [this]( crg::RecordContext const & ctx, VkCommandBuffer cmd, uint32_t ){ doSubRecordInto( ctx, cmd ); } ) )
						.pushConstants( VkPushConstantRange{ VK_SHADER_STAGE_COMPUTE_BIT, 0u, 8u } ) }
				, m_device{ device }
				, m_clusters{ clusters }
				, m_lightCache{ m_clusters.getCamera().getScene()->getLightCache() }
			{
			}

		private:
			struct ProgramData
			{
				ProgramData() = default;
				ShaderModule shaderModule{};
				ashes::PipelineShaderStageCreateInfoArray stages{};
			};

		private:
			RenderDevice const & m_device;
			FrustumClusters const & m_clusters;
			LightCache const & m_lightCache;
			Map< uint32_t, ProgramData > m_programs;

		private:
			crg::VkPipelineShaderStageCreateInfoArray doCreateProgram( uint32_t passIndex )
			{
				auto [it, res] = m_programs.try_emplace( passIndex );

				if ( res )
				{
					auto & program = it->second;
					program.shaderModule = ShaderModule{ VK_SHADER_STAGE_COMPUTE_BIT, cuT( "ReduceLightsAABB/Second" ), createShader( m_device, m_clusters.getConfig(), false ) };
					program.stages = ashes::PipelineShaderStageCreateInfoArray{ makeShaderState( m_device, program.shaderModule ) };
				}

				return ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( it->second.stages );
			}

			uint32_t doGetPassIndex()const
			{
				return ( m_clusters.getConfig().enableReduceWarpOptimisation ? 1u : 0u );
			}

			bool doIsEnabled()const
			{
				return m_clusters.getConfig().enabled
					&& m_clusters.needsLightsUpdate();
			}

			void doSubRecordInto( crg::RecordContext const & context
				, VkCommandBuffer commandBuffer )const
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

				context->vkCmdPushConstants( commandBuffer, getPipelineLayout(), VK_SHADER_STAGE_COMPUTE_BIT, 0u, 8u, &dispatchData );
			}
		};
	}

	//*********************************************************************************************

	void createReduceLightsAABBPass( crg::FramePassGroup & graph
		, RenderDevice const & device
		, FrustumClusters & clusters
		, CameraUbo const & clustersCameraUbo
		, BufferBase const & allLightsAABB
		, BufferBase & reducedLightsAABB )
	{
		auto & first = graph.createPass( "ReduceLightsAABB/First"
			, [&clusters, &device]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runGraph )
			{
				auto result = makeRawUnique< rdclgb::FirstFramePass >( framePass
					, context
					, runGraph
					, device
					, clusters
					, crg::cp::Config{} );
				device.renderSystem.getEngine()->registerTimer( makeString( framePass.getFullName() )
					, result->getTimer() );
				return result;
			} );
		clustersCameraUbo.createPassBinding( first, rdclgb::Bindings::eCamera );
		clusters.getClustersUbo().createPassBinding( first, rdclgb::Bindings::eClusters );
		first.addInputStorageT( *allLightsAABB.getLastAttach(), rdclgb::Bindings::eAllLightsAABB );
		reducedLightsAABB.setLastAttach( first.addClearableOutputStorageBufferT( reducedLightsAABB.bufferViewId, rdclgb::Bindings::eReducedLightsAABB ) );

		auto & second = graph.createPass( "ReduceLightsAABB/Second"
			, [&clusters, &device]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runGraph )
			{
				auto result = makeRawUnique< rdclgb::SecondFramePass >( framePass
					, context
					, runGraph
					, device
					, clusters
					, crg::cp::Config{} );
				device.renderSystem.getEngine()->registerTimer( makeString( framePass.getFullName() )
					, result->getTimer() );
				return result;
			} );
		clustersCameraUbo.createPassBinding( second, rdclgb::Bindings::eCamera );
		clusters.getClustersUbo().createPassBinding( second, rdclgb::Bindings::eClusters );
		reducedLightsAABB.setLastAttach( second.addInOutStorageT( *reducedLightsAABB.getLastAttach(), rdclgb::Bindings::eReducedLightsAABB ) );
	}

	//*********************************************************************************************
}
