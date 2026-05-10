#include "Castor3D/Render/Clustered/ReduceLightsAABB.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/LightCache.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/Clustered/FrustumClusters.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/Shaders/GlslAABB.hpp"
#include "Castor3D/Shader/Shaders/GlslClusteredLights.hpp"
#include "Castor3D/Shader/Ubos/CameraUbo.hpp"
#include "Castor3D/Shader/Ubos/ClustersUbo.hpp"

#include <RenderGraph/FramePassGroup.hpp>
#include <RenderGraph/RunnableGraph.hpp>
#include <RenderGraph/RunnablePasses/ComputePass.hpp>

#include <ShaderWriter/Source.hpp>

#include <limits>

namespace c3d
{
	//*********************************************************************************************

	namespace rdclgb
	{
		enum class Bindings
		{
			eAllLightsAABB,
			eCamera = eAllLightsAABB + MaxLightsAABBPageCount,
			eClusters,
			eReducedLightsAABB,
		};

		static uint32_t constexpr NumThreads = 512u;
		static float constexpr FltMax = std::numeric_limits< float >::max();

		static ShaderPtr createShader( RenderDevice const & device
			, ClustersConfig const & config
			, bool bottom )
		{
			sdw::ComputeWriter writer{ &c3d::getEngine( device ).getShaderAllocator() };

			// Inputs
			shader::AllLightsAABB allLightsAABB{ writer
				, Bindings::eAllLightsAABB
				, 0u
				, bottom };
			C3D_Camera( writer
				, Bindings::eCamera
				, 0u );
			C3D_Clusters( writer
				, Bindings::eClusters
				, 0u );
			C3D_ReducedLightsAABB( writer
				, Bindings::eReducedLightsAABB
				, 0u );

			sdw::PushConstantBuffer pcb{ writer, "C3D_DispatchData", "c3d_dispatchData" };
			auto c3d_numThreadGroups = pcb.declMember< sdw::UInt >( "numThreadGroups" );
			auto c3d_numElements = pcb.declMember< sdw::UInt >( "numElements" );
			auto c3d_pageIndex = pcb.declMember< sdw::UInt >( "pageIndex" );
			pcb.end();

			auto gsAABBMin = writer.declSharedVariable< sdw::Vec4 >( "gsAABBMin", NumThreads );
			auto gsAABBMax = writer.declSharedVariable< sdw::Vec4 >( "gsAABBMax", NumThreads );

			// Perform log-step reduction in group shared memory.
			auto logStepReduction = writer.implementFunction< sdw::Void >( "logStepReduction"
				, [&writer, &c3d_reducedLightsAABB, &gsAABBMin, &gsAABBMax, &config]( sdw::UInt const & groupIndex
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
								gsAABBMin[groupIndex] = sdw::min( gsAABBMin[groupIndex], gsAABBMin[groupIndex + reduceIndex] );
								gsAABBMax[groupIndex] = sdw::max( gsAABBMax[groupIndex], gsAABBMax[groupIndex + reduceIndex] );
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
									gsAABBMin[groupIndex] = sdw::min( gsAABBMin[groupIndex], gsAABBMin[groupIndex + reduceIndex] );
									gsAABBMax[groupIndex] = sdw::max( gsAABBMax[groupIndex], gsAABBMax[groupIndex + reduceIndex] );
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
								gsAABBMin[groupIndex] = sdw::min( gsAABBMin[groupIndex], gsAABBMin[groupIndex + reduceIndex] );
								gsAABBMax[groupIndex] = sdw::max( gsAABBMax[groupIndex], gsAABBMax[groupIndex + reduceIndex] );
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
				}
				, sdw::InUInt{ writer,"groupIndex" }
				, sdw::InUInt{ writer,"groupID" } );

			writer.implementMainT< sdw::VoidT >( NumThreads
				, [&writer, &allLightsAABB, &c3d_clustersData, &c3d_reducedLightsAABB, &c3d_numThreadGroups, &c3d_clustersLightsData
					, &c3d_lightsAABBRange, &c3d_cameraData, &c3d_numElements, &c3d_pageIndex, &gsAABBMin, &gsAABBMax, &logStepReduction
					, bottom]( sdw::ComputeIn const & in )
				{
					auto const & groupIndex = in.localInvocationIndex;
					auto const & groupID = in.workGroupID.x();
					auto const & threadIndex = in.globalInvocationID.x();

					auto aabbMin = writer.declLocale( "aabbMin"
						, vec4( sdw::Float{ FltMax }, FltMax, FltMax, 1.0f ) );
					auto aabbMax = writer.declLocale( "aabbMax"
						, vec4( sdw::Float{ -FltMax }, -FltMax, -FltMax, 1.0f ) );

					if ( bottom )
					{
						// The 1st pass of the reduction operates on each page of the light AABB buffer.
						sdwIF( writer, threadIndex < c3d_numElements )
						{
							auto aabb = writer.declLocale( "aabb", allLightsAABB.loadInPage( c3d_pageIndex, threadIndex ) );
							aabbMin = sdw::min( aabbMin, aabb.min() );
							aabbMax = sdw::max( aabbMax, aabb.max() );
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
						// The subsequent passes of the reduction operate on the global AABB computed in previous pass.
						// This step is repeated until we are reduced to a single thread group.

						// Init shared memory
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

						sdwFOR( writer, sdw::UInt, i, groupIndex + groupID * NumThreads, i < c3d_numElements, i += NumThreads * c3d_numThreadGroups )
						{
							auto aabb = writer.declLocale( "aabb", c3d_reducedLightsAABB[i] );
							aabbMin = sdw::min( aabbMin, aabb.min() );
							aabbMax = sdw::max( aabbMax, aabb.max() );
						}
						sdwROF

						gsAABBMin[groupIndex] = aabbMin;
						gsAABBMax[groupIndex] = aabbMax;
						shader::groupMemoryBarrierWithGroupSync( writer );

						// Perform log-step reduction to allow each thread group in the dispatch
						// to reduce to a single element. If there was only a single thread group
						// in this dispatch, then this will reduce to a single element.
						logStepReduction( groupIndex, groupID );

						sdwIF( writer, groupIndex == 0_u && c3d_numThreadGroups == 1_u )
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
					}
				} );
			return writer.getBuilder().releaseShader();
		}

		static u32 computeThreadGroupsCount( LightCache const & cache )
		{
			auto pointLightsCount = cache.getLightsBufferCount( LightType::ePoint );
			auto spoLightsCount = cache.getLightsBufferCount( LightType::eSpot );
			auto totalLightsCount = std::max( pointLightsCount, spoLightsCount );
			return std::min( NumThreads, divRoundUp( totalLightsCount, NumThreads ) );
		}

		class BottomFramePass
			: public crg::RunnablePass
		{
		public:
			BottomFramePass( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph
				, RenderDevice const & device
				, FrustumClusters const & clusters
				, crg::Buffer & lightsAABBBuffer )
				: crg::RunnablePass{ framePass
					, context
					, graph
					, crg::RunnablePass::Callbacks{}
						.onInitialise( [this]( uint32_t index ){ doInitialise( index ); } )
						.onGetPipelineState( [](){ return crg::getPipelineState( PipelineStageFlags::eComputeShader ); } )
						.onRecord( [this]( crg::RecordContext const & recContext, VkCommandBuffer cb, uint32_t i ){ doRecordInto( recContext, cb, i ); } )
						.onGetPassIndex( [this](){ return doGetPassIndex(); } )
						.onIsEnabled( [this](){ return doIsEnabled(); } )
						.onIsComputePass( [](){ return true; } )
					, crg::ru::Config{ 2u } }
				, m_device{ device }
				, m_clusters{ clusters }
				, m_lightCache{ m_clusters.getScene().getLightCache() }
				, m_lightsAABBBuffer{ lightsAABBBuffer }
				, m_pipeline{ framePass
					, graph.getContext()
					, graph
					, crg::pp::Config{}
						.programCreator( { 2u, [this]( uint32_t passIndex ){ return doCreateProgram( passIndex ); } } )
						.pushConstants( VkPushConstantRange{ VK_SHADER_STAGE_COMPUTE_BIT, 0u, 12u } )
					, VK_PIPELINE_BIND_POINT_COMPUTE
					, 2u }
			{
			}

		private:
			struct ProgramData
			{
				ProgramData() = default;
				ShaderModule shaderModule{};
				ashes::PipelineShaderStageCreateInfoArray stages{};
			};

			struct DispatchData
			{
				uint32_t numThreadGroups{};
				uint32_t numElements{};
				uint32_t pageIndex{};
			};

		private:
			void doInitialise( uint32_t index )
			{
				m_pipeline.initialise();
				doCreatePipeline( index );
			}

			bool doIsEnabled()const
			{
				return m_clusters.getConfig().enabled
					&& m_clusters.needsLightsUpdate();
			}

			void doCreatePipeline( uint32_t index )
			{
				auto & program = m_pipeline.getProgram( index );
				VkComputePipelineCreateInfo createInfo{ VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO
					, nullptr
					, 0u
					, program.front()
					, m_pipeline.getPipelineLayout()
					, VkPipeline{}
					, 0u };
				m_pipeline.createPipeline( index, createInfo );
			}

			crg::VkPipelineShaderStageCreateInfoArray doCreateProgram( uint32_t passIndex )
			{
				auto [it, res] = m_programs.try_emplace( passIndex );

				if ( res )
				{
					auto & program = it->second;
					program.shaderModule = ShaderModule{ VK_SHADER_STAGE_COMPUTE_BIT, cuT( "ReduceLightsAABB/Bottom" ), createShader( m_device, m_clusters.getConfig(), true ) };
					program.stages = ashes::PipelineShaderStageCreateInfoArray{ makeShaderState( m_device, program.shaderModule ) };
				}

				return ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( it->second.stages );
			}

			uint32_t doGetPassIndex()const
			{
				uint32_t result{ ( m_clusters.getConfig().enableReduceWarpOptimisation ? 1u : 0u ) };
				return result;
			}

			void doRecordInto( crg::RecordContext const & context
				, VkCommandBuffer commandBuffer
				, uint32_t passIndex )
			{
				DispatchData dispatchData{};
				u32 pointLightsCount = m_lightCache.getLightsBufferCount( LightType::ePoint );
				u32 spoLightsCount = m_lightCache.getLightsBufferCount( LightType::eSpot );

				u32 totalElementCount = pointLightsCount + spoLightsCount;
				u32 threadGroupsCount = std::min( NumThreads, divRoundUp( totalElementCount, NumThreads ) );
				u32 allocatedPageCount = m_lightsAABBBuffer.getAllocatedPageCount();

				// Ask for buffer size update for next frame.
				u32 neededPageCount = divRoundUp( threadGroupsCount, NumThreads );
				if ( neededPageCount > allocatedPageCount )
					m_lightsAABBBuffer.resize( neededPageCount * m_lightsAABBBuffer.getPageSize() );

				// Adjust the thread groups and elements counts to the currently allocated page count.
				threadGroupsCount = std::min( threadGroupsCount, allocatedPageCount * NumThreads );
				totalElementCount = std::min( totalElementCount, threadGroupsCount * NumThreads );
				u32 pageCount = divRoundUp( threadGroupsCount, NumThreads );

				// Bind descriptor sets and pipeline.
				m_pipeline.recordInto( context, commandBuffer, passIndex );

				// Reduce each page of the Lights AABB buffer into a single AABB and store it in the Reduced Lights AABB buffer.
				u32 remainingElementsCount = totalElementCount;
				u32 remainingThreadGroupsCount = threadGroupsCount;
				for ( u32 pageIndex = 0; pageIndex < pageCount; ++pageIndex )
				{
					u32 currentThreadGroupsCount = std::min( remainingThreadGroupsCount, NumThreads );
					dispatchData.numElements = std::min( remainingElementsCount, LightsAABBPageSize );
					dispatchData.pageIndex = pageIndex;
					context->vkCmdPushConstants( commandBuffer, m_pipeline.getPipelineLayout(), VK_SHADER_STAGE_COMPUTE_BIT, 0u, 8u, &dispatchData );
					context->vkCmdDispatch( commandBuffer, currentThreadGroupsCount, 1u, 1u );
					remainingThreadGroupsCount -= currentThreadGroupsCount;
					remainingElementsCount -= dispatchData.numElements;
				}
				CU_Require( remainingElementsCount == 0 );
				CU_Require( remainingThreadGroupsCount == 0 );
			}

		private:
			RenderDevice const & m_device;
			FrustumClusters const & m_clusters;
			LightCache const & m_lightCache;
			crg::Buffer & m_lightsAABBBuffer;
			crg::PipelineHolder m_pipeline;
			Map< uint32_t, ProgramData > m_programs;
		};

		class TopFramePass
			: public crg::RunnablePass
		{
		public:
			TopFramePass( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph
				, RenderDevice const & device
				, FrustumClusters const & clusters
				, crg::Buffer & lightsAABBBuffer )
				: crg::RunnablePass{ framePass
					, context
					, graph
					, crg::RunnablePass::Callbacks{}
						.onInitialise( [this]( uint32_t index ){ doInitialise( index ); } )
						.onGetPipelineState( [](){ return crg::getPipelineState( PipelineStageFlags::eComputeShader ); } )
						.onRecord( [this]( crg::RecordContext const & recContext, VkCommandBuffer cb, uint32_t i ){ doRecordInto( recContext, cb, i ); } )
						.onGetPassIndex( [this](){ return doGetPassIndex(); } )
						.onIsEnabled( [this](){ return doIsEnabled(); } )
						.onIsComputePass( [](){ return true; } )
					, crg::ru::Config{ 2u } }
				, m_device{ device }
				, m_clusters{ clusters }
				, m_lightCache{ m_clusters.getScene().getLightCache() }
				, m_lightsAABBBuffer{ lightsAABBBuffer }
				, m_pipeline{ framePass
					, graph.getContext()
					, graph
					, crg::pp::Config{}
						.programCreator( { 2u, [this]( uint32_t passIndex ){ return doCreateProgram( passIndex ); } } )
						.pushConstants( VkPushConstantRange{ VK_SHADER_STAGE_COMPUTE_BIT, 0u, 12u } )
					, VK_PIPELINE_BIND_POINT_COMPUTE
					, 2u }
			{
			}

		private:
			struct ProgramData
			{
				ProgramData() = default;
				ShaderModule shaderModule{};
				ashes::PipelineShaderStageCreateInfoArray stages{};
			};

			struct DispatchData
			{
				uint32_t numThreadGroups{};
				uint32_t numElements{};
				uint32_t pageIndex{};
			};

		private:
			void doInitialise( uint32_t index )
			{
				m_pipeline.initialise();
				doCreatePipeline( index );
			}

			bool doIsEnabled()const
			{
				return m_clusters.getConfig().enabled
					&& m_clusters.needsLightsUpdate();
			}

			void doCreatePipeline( uint32_t index )
			{
				auto & program = m_pipeline.getProgram( index );
				VkComputePipelineCreateInfo createInfo{ VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO
					, nullptr
					, 0u
					, program.front()
					, m_pipeline.getPipelineLayout()
					, VkPipeline{}
					, 0u };
				m_pipeline.createPipeline( index, createInfo );
			}

			crg::VkPipelineShaderStageCreateInfoArray doCreateProgram( uint32_t passIndex )
			{
				auto [it, res] = m_programs.try_emplace( passIndex );

				if ( res )
				{
					auto & program = it->second;
					program.shaderModule = ShaderModule{ VK_SHADER_STAGE_COMPUTE_BIT, cuT( "ReduceLightsAABB/Top" ), createShader( m_device, m_clusters.getConfig(), false ) };
					program.stages = ashes::PipelineShaderStageCreateInfoArray{ makeShaderState( m_device, program.shaderModule ) };
				}

				return ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( it->second.stages );
			}

			uint32_t doGetPassIndex()const
			{
				return ( m_clusters.getConfig().enableReduceWarpOptimisation ? 1u : 0u );
			}

			void doRecordInto( crg::RecordContext const & context
				, VkCommandBuffer commandBuffer
				, uint32_t passIndex )
			{
				DispatchData dispatchData{};
				u32 pointLightsCount = m_lightCache.getLightsBufferCount( LightType::ePoint );
				u32 spoLightsCount = m_lightCache.getLightsBufferCount( LightType::eSpot );

				u32 totalElementCount = pointLightsCount + spoLightsCount;
				u32 threadGroupsCount = std::min( NumThreads, divRoundUp( totalElementCount, NumThreads ) );
				u32 allocatedPageCount = m_lightsAABBBuffer.getAllocatedPageCount();

				// Ask for buffer size update for next frame.
				u32 neededPageCount = divRoundUp( threadGroupsCount, NumThreads );
				if ( neededPageCount > allocatedPageCount )
					m_lightsAABBBuffer.resize( neededPageCount * m_lightsAABBBuffer.getPageSize() );

				// Adjust the thread groups and elements counts to the currently allocated page count.
				threadGroupsCount = std::min( threadGroupsCount, allocatedPageCount * NumThreads );
				totalElementCount = std::min( totalElementCount, threadGroupsCount * NumThreads );
				u32 pageCount = divRoundUp( threadGroupsCount, NumThreads );

				// Bind descriptor sets and pipeline.
				m_pipeline.recordInto( context, commandBuffer, passIndex );

				// Reduce the results of each page into a single AABB
				{
					dispatchData.numThreadGroups = pageCount;
					dispatchData.numElements = threadGroupsCount;
					context->vkCmdPushConstants( commandBuffer, m_pipeline.getPipelineLayout(), VK_SHADER_STAGE_COMPUTE_BIT, 0u, 8u, &dispatchData );
					context->vkCmdDispatch( commandBuffer, pageCount, 1u, 1u );
				}

				// If there are multiple pages, we need an additional reduction passto reduce each page AABB into a single AABB.
				if ( pageCount > 1u )
				{
					dispatchData.numThreadGroups = 1u;
					dispatchData.numElements = pageCount;
					context->vkCmdPushConstants( commandBuffer, m_pipeline.getPipelineLayout(), VK_SHADER_STAGE_COMPUTE_BIT, 0u, 8u, &dispatchData );
					context->vkCmdDispatch( commandBuffer, 1u, 1u, 1u );
				}
			}

		private:
			RenderDevice const & m_device;
			FrustumClusters const & m_clusters;
			LightCache const & m_lightCache;
			crg::Buffer & m_lightsAABBBuffer;
			crg::PipelineHolder m_pipeline;
			Map< uint32_t, ProgramData > m_programs;
		};
	}

	//*********************************************************************************************

	void createReduceLightsAABBPass( crg::FramePassGroup & graph
		, RenderDevice const & device
		, FrustumClusters & clusters
		, CameraUbo const & clustersCameraUbo
		, PagedBufferBase const & allLightsAABB
		, BufferBase & reducedLightsAABB )
	{
		auto & first = graph.createPass( "ReduceLightsAABB/Bottom"
			, [&clusters, &device, &allLightsAABB]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runGraph )
			{
				auto result = makeRawUnique< rdclgb::BottomFramePass >( framePass
					, context
					, runGraph
					, device
					, clusters
					, allLightsAABB.getBufferPages() );
				c3d::getEngine( device ).registerTimer( makeString( framePass.getFullName() )
					, result->getTimer() );
				return result;
			} );
		first.addInputStorageT( *allLightsAABB.getLastAttach(), rdclgb::Bindings::eAllLightsAABB );
		clustersCameraUbo.createPassBinding( first, rdclgb::Bindings::eCamera );
		clusters.getClustersUbo().createPassBinding( first, rdclgb::Bindings::eClusters );
		reducedLightsAABB.setLastAttach( first.addClearableOutputStorageBufferT( reducedLightsAABB.bufferViewId, rdclgb::Bindings::eReducedLightsAABB ) );

		auto & second = graph.createPass( "ReduceLightsAABB/Top"
			, [&clusters, &device, &allLightsAABB]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runGraph )
			{
				auto result = makeRawUnique< rdclgb::TopFramePass >( framePass
					, context
					, runGraph
					, device
					, clusters
					, allLightsAABB.getBufferPages() );
				c3d::getEngine( device ).registerTimer( makeString( framePass.getFullName() )
					, result->getTimer() );
				return result;
			} );
		clustersCameraUbo.createPassBinding( second, rdclgb::Bindings::eCamera );
		clusters.getClustersUbo().createPassBinding( second, rdclgb::Bindings::eClusters );
		reducedLightsAABB.setLastAttach( second.addInOutStorageT( *reducedLightsAABB.getLastAttach(), rdclgb::Bindings::eReducedLightsAABB ) );
	}

	//*********************************************************************************************
}
