#include "Castor3D/Render/Clustered/AssignLightsToClusters.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/LightCache.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/Clustered/ClustersConfig.hpp"
#include "Castor3D/Render/Clustered/FrustumClusters.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/Light/SpotLight.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/Shaders/GlslAABB.hpp"
#include "Castor3D/Shader/Shaders/GlslAppendBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslBVHBase.hpp"
#include "Castor3D/Shader/Shaders/GlslClusteredLights.hpp"
#include "Castor3D/Shader/Shaders/GlslLight.hpp"
#include "Castor3D/Shader/Shaders/GlslLightsBVH.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Ubos/CameraUbo.hpp"
#include "Castor3D/Shader/Ubos/ClustersUbo.hpp"

#include <RenderGraph/FramePassGroup.hpp>
#include <RenderGraph/RunnableGraph.hpp>
#include <RenderGraph/RunnablePasses/ComputePass.hpp>

#include <ShaderWriter/Source.hpp>

namespace c3d
{
	//*********************************************************************************************

	namespace dspclst
	{
		enum class Bindings
		{
			eLights,
			eAllLightsAABB,
			eCamera = eAllLightsAABB + MaxLightsAABBPageCount,
			eClusters,
			eClustersAABB,
			ePointLightBVH,
			eSpotLightBVH,
			ePointLightIndices,
			eSpotLightIndices,
			eCounters,
			ePointLightClusterGrid,
			eSpotLightClusterGrid,
			ePointLightClusterIndex,
			eSpotLightClusterIndex = ePointLightClusterIndex + MaxClusterLightsIndicesPageCount,
		};

		static ShaderPtr createShader( RenderDevice const & device )
		{
			uint32_t NumThreads = 32u;

			sdw::ComputeWriter writer{ &c3d::getEngine( device ).getShaderAllocator() };

			// Inputs
			shader::LightsBuffer lights{ writer
				, uint32_t( Bindings::eLights )
				, 0u };
			shader::AllLightsAABB allLightsAABB{ writer
				, Bindings::eAllLightsAABB
				, 0u };
			C3D_Camera( writer
				, Bindings::eCamera
				, 0u );
			C3D_Clusters( writer
				, Bindings::eClusters
				, 0u );
			C3D_ClustersAABB( writer
				, Bindings::eClustersAABB
				, 0u );
			C3D_PointLightBVH( writer
				, Bindings::ePointLightBVH
				, 0u );
			C3D_SpotLightBVH( writer
				, Bindings::eSpotLightBVH
				, 0u );
			C3D_PointLightIndices( writer
				, Bindings::ePointLightIndices
				, 0u );
			C3D_SpotLightIndices( writer
				, Bindings::eSpotLightIndices
				, 0u );
			C3D_PointLightClusterGrid( writer
				, Bindings::ePointLightClusterGrid
				, 0u );
			C3D_SpotLightClusterGrid( writer
				, Bindings::eSpotLightClusterGrid
				, 0u );
			auto countersBuffer = writer.declStorageBuffer( "c3d_counters", uint32_t( Bindings::eCounters ), 0u );
			auto c3d_pointLightClusterListCount = countersBuffer.declMember< sdw::UInt >( "c3d_pointLightClusterListCount" );
			auto c3d_spotLightClusterListCount = countersBuffer.declMember< sdw::UInt >( "c3d_spotLightClusterListCount" );
			countersBuffer.end();
			shader::ClusterLightsIndices pointLightClusterIndices{ writer
				, "c3d_pointLightClusterIndices"
				, Bindings::ePointLightClusterIndex
				, 0u };
			shader::ClusterLightsIndices spotLightClusterIndices{ writer
				, "c3d_spotLightClusterIndices"
				, Bindings::eSpotLightClusterIndex
				, 0u };

			auto gsProcessedLightsCount = writer.declSharedVariable< sdw::UInt >( "gsProcessedLightsCount" );
			auto gsLightStartOffset = writer.declSharedVariable< sdw::UInt >( "gsLightStartOffset" );
			auto gsClusterIndex1D = writer.declSharedVariable< sdw::UInt32 >( "gsClusterIndex1D" );
			auto gsClusterAABB = writer.declSharedVariable< shader::AABB >( "gsClusterAABB" );

			shader::BVHBaseT< shader::AABB > bvh{ writer };
			shader::LightsBVHT< shader::AABB > lightsBvh{ writer, allLightsAABB, MaxLightsPerCluster };

			auto processPointLights = writer.implementFunction< sdw::Void >( "c3d_processPointLights"
				, [&gsProcessedLightsCount, &gsLightStartOffset, gsClusterIndex1D
					, c3d_pointLightClusterGrid, &pointLightClusterIndices, &c3d_pointLightClusterListCount
					, &writer, &lightsBvh, NumThreads]( sdw::UInt const & groupIndex )
				{
					sdwIF( writer, groupIndex == 0u )
					{
						gsProcessedLightsCount = lightsBvh.getTraversedLightCount();
						sdwIF( writer, gsProcessedLightsCount > 0_u )
						{
							gsProcessedLightsCount = min( sdw::UInt{ MaxLightsPerCluster }, gsProcessedLightsCount );
							gsLightStartOffset = sdw::atomicAdd( c3d_pointLightClusterListCount, gsProcessedLightsCount );
							c3d_pointLightClusterGrid[gsClusterIndex1D] = sdw::uvec2( gsLightStartOffset, gsProcessedLightsCount );
						}
						sdwFI
					}
					sdwFI

					shader::groupMemoryBarrierWithGroupSync( writer );

					// Now update the global light index lists with the group shared light lists.
					sdwFOR( writer, sdw::UInt, i, groupIndex, i < gsProcessedLightsCount, i += NumThreads )
					{
						pointLightClusterIndices[gsLightStartOffset + i] = lightsBvh.getTraversedLightID( i );
					}
					sdwROF
				}
				, sdw::InUInt{ writer, "groupIndex" } );

			auto processSpotLights = writer.implementFunction< sdw::Void >( "c3d_processSpotLights"
				, [&gsProcessedLightsCount, &gsLightStartOffset, gsClusterIndex1D
					, c3d_spotLightClusterGrid, &spotLightClusterIndices, &c3d_spotLightClusterListCount
					, &writer, &lightsBvh, NumThreads]( sdw::UInt const & groupIndex )
				{
					sdwIF( writer, groupIndex == 0u )
					{
						gsProcessedLightsCount = lightsBvh.getTraversedLightCount();
						sdwIF( writer, gsProcessedLightsCount > 0_u )
						{
							gsProcessedLightsCount = min( sdw::UInt{ MaxLightsPerCluster }, gsProcessedLightsCount );
							gsLightStartOffset = sdw::atomicAdd( c3d_spotLightClusterListCount, gsProcessedLightsCount );
							c3d_spotLightClusterGrid[gsClusterIndex1D] = sdw::uvec2( gsLightStartOffset, gsProcessedLightsCount );
						}
						sdwFI
					}
					sdwFI

					shader::groupMemoryBarrierWithGroupSync( writer );

					// Now update the global light index lists with the group shared light lists.
					sdwFOR( writer, sdw::UInt, i, groupIndex, i < gsProcessedLightsCount, i += NumThreads )
					{
						spotLightClusterIndices[gsLightStartOffset + i] = lightsBvh.getTraversedLightID( i );
					}
					sdwROF
				}
				, sdw::InUInt{ writer, "groupIndex" } );

			shader::Utils utils{ writer };
			writer.implementMainT< sdw::VoidT >( NumThreads
				, [&c3d_pointLightBVH, &c3d_spotLightBVH, &c3d_cameraData, &c3d_pointLightIndices, &c3d_spotLightIndices, &c3d_clustersData, &c3D_clustersAABB, &gsClusterAABB, &gsClusterIndex1D
					, &writer, &lights, &bvh, &lightsBvh, &processPointLights, &processSpotLights]( sdw::ComputeIn const & in )
				{
					auto const & groupIndex = in.localInvocationIndex;

					// Initialise traversal
					sdwIF( writer, groupIndex == 0_u )
					{
						gsClusterIndex1D = c3d_clustersData.computeClusterIndex1D( uvec3( in.workGroupID ) );
						gsClusterAABB = c3D_clustersAABB[gsClusterIndex1D];
						lightsBvh.resetTraversal();
						bvh.resetTraversal();
					}
					sdwFI
					shader::groupMemoryBarrierWithGroupSync( writer );

					// Point lights first phase
					lightsBvh.traversePointLights( c3d_pointLightIndices, c3d_pointLightBVH
						, bvh, gsClusterAABB, c3d_clustersData.pointLightLevels(), c3d_clustersData.pointLightCount(), groupIndex );

					// Point lights second phase
					processPointLights( groupIndex );
					shader::groupMemoryBarrierWithGroupSync( writer );

					// Reset the stack.
					sdwIF( writer, groupIndex == 0_u )
					{
						lightsBvh.resetTraversal();
						bvh.resetTraversal();
					}
					sdwFI
					shader::groupMemoryBarrierWithGroupSync( writer );

					// Spot lights first phase
					lightsBvh.traverseSpotLights( c3d_spotLightIndices, lights, c3d_cameraData, c3d_spotLightBVH
						, bvh, gsClusterAABB, c3d_clustersData.pointLightCount(), c3d_clustersData.spotLightLevels(), c3d_clustersData.spotLightCount(), groupIndex );

					// Spot lights second phase
					processSpotLights( groupIndex );
				} );
			return writer.getBuilder().releaseShader();
		}

		class FramePass
			: public crg::ComputePass
		{
		public:
			FramePass( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph
				, RenderDevice const & device
				, FrustumClusters const & clusters
				, crg::Buffer & pointLightClusterIndices
				, crg::Buffer & spotLightClusterIndices
				, Array< BufferT< uint32_t >, 3u > const & counters
				, crg::cp::Config config )
				: crg::ComputePass{ framePass
					, context
					, graph
					, crg::ru::Config{ 1u }
					, config
						.isEnabled( crg::IsEnabledCallback( [this, &clusters]() { return doIsEnabled( clusters ); } ) )
						.programCreator( { 1u, [this]( uint32_t passIndex ){ return doCreateProgram( passIndex ); } } )
						.end( crg::RecordCallback( [this]( crg::RecordContext & recContext, VkCommandBuffer cb, uint32_t i ){ doSubRecordInto( recContext, cb, i ); } ) ) }
				, m_device{ device }
				, m_config{ clusters.getConfig() }
				, m_pointLightClusterIndices{ pointLightClusterIndices }
				, m_spotLightClusterIndices{ spotLightClusterIndices }
				, m_counters{ counters }
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
			bool doIsEnabled( FrustumClusters const & clusters )const
			{
				return clusters.getScene().hasClusteredLights();
			}

			crg::VkPipelineShaderStageCreateInfoArray doCreateProgram( uint32_t passIndex )
			{
				auto [it, res] = m_programs.try_emplace( passIndex );

				if ( res )
				{
					auto & program = it->second;
					program.shaderModule = ShaderModule{ VK_SHADER_STAGE_COMPUTE_BIT, cuT( "AssignLightsToClusters" ), dspclst::createShader( m_device ) };
					program.stages = ashes::PipelineShaderStageCreateInfoArray{ makeShaderState( m_device, program.shaderModule ) };
				}

				return ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( it->second.stages );
			}

			void doSubRecordInto( crg::RecordContext & recordContext
				, VkCommandBuffer commandBuffer
				, uint32_t passIndex )
			{
				auto tmpCounters = getPass().getOutputs().find( uint32_t( dspclst::Bindings::eCounters ) )->second;

				auto wasBatching = recordContext.isBatching();
				if ( wasBatching )
				{
					recordContext.flushBatchBarriers( commandBuffer );
				}

				// Copy temp counters to mappable counter buffer
				crg::BufferViewId tmpCountersId = tmpCounters->buffer( passIndex );
				crg::BufferViewId countersId = m_counters[m_prevBufferIndex].bufferViewId;
				AccessState currentState = recordContext.getAccessState( tmpCountersId );
				AccessState wantedState{ AccessFlags::eTransferRead, PipelineStageFlags::eTransfer };
				recordContext.memoryBarrier( commandBuffer
					, tmpCountersId, currentState
					, wantedState );
				recordContext.memoryBarrier( commandBuffer
					, countersId
					, AccessState{ AccessFlags::eTransferWrite, PipelineStageFlags::eTransfer } );
				recordContext.copyBuffer( commandBuffer, passIndex, tmpCountersId, countersId, 0u, 0u, sizeof( u32 ) * 2u );
				recordContext.memoryBarrier( commandBuffer
					, tmpCountersId, wantedState, currentState );

				if ( wasBatching )
				{
					recordContext.beginBatchBarriers();
				}

				// Copy mappable counter buffer to RAM
				auto & vkBuffer = m_counters[m_prevBufferIndex].getBuffer();
				uint32_t * ptr{};
				recordContext->vkMapMemory( recordContext->device, vkBuffer.getStorage(), 0, VK_WHOLE_SIZE, 0, reinterpret_cast< void ** >( &ptr ) );
				if ( ptr )
				{
					auto neededPoint = ptr[0] * sizeof( u32 );
					if ( neededPoint > m_pointLightClusterIndices.getAllocatedSize() )
						m_pointLightClusterIndices.resize( neededPoint );

					auto neededSpot = ptr[0] * sizeof( u32 );
					if ( neededSpot > m_spotLightClusterIndices.getAllocatedSize() )
						m_spotLightClusterIndices.resize( neededSpot );

					recordContext->vkUnmapMemory( recordContext->device, vkBuffer.getStorage() );
				}

				// Cycle buffers, to prevent stalling GPU on buffer read.
				m_prevBufferIndex = m_bufferIndex;
				m_bufferIndex = ( m_bufferIndex + 1u ) % 3u;
			}

		private:
			RenderDevice const & m_device;
			ClustersConfig const & m_config;
			Map< uint32_t, ProgramData > m_programs;
			uint32_t m_prevBufferIndex{ 2u };
			uint32_t m_bufferIndex{ 0u };
			crg::Buffer & m_pointLightClusterIndices;
			crg::Buffer & m_spotLightClusterIndices;
			Array< BufferT< uint32_t >, 3u > const & m_counters;
		};
	}

	//*********************************************************************************************

	void createAssignLightsToClustersPass( crg::FramePassGroup & graph
		, RenderDevice const & device
		, FrustumClusters & clusters
		, CameraUbo const & clustersCameraUbo
		, PagedBufferBase const & allLightsAABB
		, BufferBase const & clustersAABB
		, BufferBase const & pointLightBVH
		, BufferBase const & spotLightBVH
		, BufferBase const & pointLightIndices
		, BufferBase const & spotLightIndices
		, Array< BufferT< uint32_t >, 3u > const & counters
		, PagedBufferBase & pointLightClusterIndex
		, PagedBufferBase & spotLightClusterIndex
		, BufferBase & pointLightClusterGrid
		, BufferBase & spotLightClusterGrid
		, BufferBase & tmpCounters )
	{
		auto const & lights = clusters.getScene().getLightCache();

		auto & pass = graph.createPass( "AssignLightsToClusters"
			, [&clusters, &device, &counters, &pointLightClusterIndex, &spotLightClusterIndex]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runGraph )
			{
				auto result = makeRawUnique< dspclst::FramePass >( framePass, context, runGraph
					, device, clusters
					, pointLightClusterIndex.getBufferPages()
					, spotLightClusterIndex.getBufferPages()
					, counters
					, crg::cp::Config{}
						.getGroupCountX( crg::cp::GetGroupCountCallback( [&clusters](){ return clusters.getDimensions()->x; } ) )
						.getGroupCountY( crg::cp::GetGroupCountCallback( [&clusters](){ return clusters.getDimensions()->y; } ) )
						.getGroupCountZ( crg::cp::GetGroupCountCallback( [&clusters](){ return clusters.getDimensions()->z; } ) ) );
				c3d::getEngine( device ).registerTimer( makeString( framePass.getFullName() )
					, result->getTimer() );
				return result;
			} );
		clustersCameraUbo.createPassBinding( pass, dspclst::Bindings::eCamera );
		lights.createPassBindingT( pass, dspclst::Bindings::eLights );
		clusters.getClustersUbo().createPassBinding( pass, dspclst::Bindings::eClusters );
		pass.addInputStorageT( *allLightsAABB.getLastAttach(), dspclst::Bindings::eAllLightsAABB );
		pass.addInputStorageT( *clustersAABB.getLastAttach(), dspclst::Bindings::eClustersAABB );
		pass.addInputStorageT( *pointLightBVH.getLastAttach(), dspclst::Bindings::ePointLightBVH );
		pass.addInputStorageT( *spotLightBVH.getLastAttach(), dspclst::Bindings::eSpotLightBVH );
		pass.addInputStorageT( *pointLightIndices.getLastAttach(), dspclst::Bindings::ePointLightIndices );
		pass.addInputStorageT( *spotLightIndices.getLastAttach(), dspclst::Bindings::eSpotLightIndices );

		tmpCounters.setLastAttach( pass.addClearableOutputStorageBufferT( tmpCounters.bufferViewId, dspclst::Bindings::eCounters ) );
		pointLightClusterGrid.setLastAttach( pass.addClearableOutputStorageBufferT( pointLightClusterGrid.bufferViewId, dspclst::Bindings::ePointLightClusterGrid ) );
		spotLightClusterGrid.setLastAttach( pass.addClearableOutputStorageBufferT( spotLightClusterGrid.bufferViewId, dspclst::Bindings::eSpotLightClusterGrid ) );
		pointLightClusterIndex.setLastAttach( pass.addClearableOutputStorageBufferT( pointLightClusterIndex.bufferViewId, dspclst::Bindings::ePointLightClusterIndex ) );
		spotLightClusterIndex.setLastAttach( pass.addClearableOutputStorageBufferT( spotLightClusterIndex.bufferViewId, dspclst::Bindings::eSpotLightClusterIndex ) );
	}

	//*********************************************************************************************
}
