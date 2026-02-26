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
#include <RenderGraph/RunnablePasses/ComputePass.hpp>

#include <ShaderWriter/Source.hpp>

namespace c3d
{
	//*********************************************************************************************

	namespace dspclst
	{
		enum class Bindings
		{
			eCamera,
			eLights,
			eClusters,
			eAllLightsAABB,
			eClustersAABB,
			ePointLightBVH,
			eSpotLightBVH,
			ePointLightIndices,
			eSpotLightIndices,
			ePointLightIndex,
			ePointLightCluster,
			eSpotLightIndex,
			eSpotLightCluster,
		};

		static ShaderPtr createShader( RenderDevice const & device )
		{
			uint32_t NumThreads = 32u;

			sdw::ComputeWriter writer{ &c3d::getEngine( device ).getShaderAllocator() };

			// Inputs
			C3D_Camera( writer
				, Bindings::eCamera
				, 0u );
			shader::LightsBuffer lights{ writer
				, uint32_t( Bindings::eLights )
				, 0u };
			C3D_Clusters( writer
				, Bindings::eClusters
				, 0u );
			C3D_AllLightsAABB( writer
				, Bindings::eAllLightsAABB
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
			C3D_PointLightClusterIndex( writer
				, Bindings::ePointLightIndex
				, 0u );
			C3D_PointLightClusterGrid( writer
				, Bindings::ePointLightCluster
				, 0u );
			C3D_SpotLightClusterIndex( writer
				, Bindings::eSpotLightIndex
				, 0u );
			C3D_SpotLightClusterGrid( writer
				, Bindings::eSpotLightCluster
				, 0u );

			auto gsProcessedLightsCount = writer.declSharedVariable< sdw::UInt >( "gsProcessedLightsCount" );
			auto gsLightStartOffset = writer.declSharedVariable< sdw::UInt >( "gsLightStartOffset" );
			auto gsClusterIndex1D = writer.declSharedVariable< sdw::UInt32 >( "gsClusterIndex1D" );
			auto gsClusterAABB = writer.declSharedVariable< shader::AABB >( "gsClusterAABB" );

			shader::BVHBaseT< shader::AABB > bvh{ writer };
			shader::LightsBVHT< shader::AABB > lightsBvh{ writer, c3d_allLightsAABB, MaxLightsPerCluster };

			auto processPointLights = writer.implementFunction< sdw::Void >( "c3d_processPointLights"
				, [&gsProcessedLightsCount, &gsLightStartOffset, gsClusterIndex1D
					, c3d_pointLightClusterGrid, &c3d_pointLightClusterIndex, &c3d_pointLightClusterListCount
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
						c3d_pointLightClusterIndex[gsLightStartOffset + i] = lightsBvh.getTraversedLightID( i );
					}
					sdwROF
				}
				, sdw::InUInt{ writer, "groupIndex" } );

			auto processSpotLights = writer.implementFunction< sdw::Void >( "c3d_processSpotLights"
				, [&gsProcessedLightsCount, &gsLightStartOffset, gsClusterIndex1D
					, c3d_spotLightClusterGrid, &c3d_spotLightClusterIndex, &c3d_spotLightClusterListCount
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
						c3d_spotLightClusterIndex[gsLightStartOffset + i] = lightsBvh.getTraversedLightID( i );
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
				, crg::cp::Config config )
				: crg::ComputePass{framePass
					, context
					, graph
					, crg::ru::Config{ 2u }
					, config
						.isEnabled( IsEnabledCallback( [this, &clusters]() { return doIsEnabled( clusters ); } ) )
						.getPassIndex( RunnablePass::GetPassIndexCallback( [this, &clusters](){ return doGetPassIndex( clusters ); } ) )
						.programCreator( { 2u, [this]( uint32_t passIndex ){ return doCreateProgram( passIndex ); } } ) }
				, m_device{ device }
				, m_config{ clusters.getConfig() }
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
			uint32_t doGetPassIndex( FrustumClusters const & clusters )const
			{
				u32 result = {};

				auto const & lightCache = clusters.getScene().getLightCache();
				auto pointLightsCount = lightCache.getLightsBufferCount( LightType::ePoint );
				auto spoLightsCount = lightCache.getLightsBufferCount( LightType::eSpot );
				auto totalValues = std::max( pointLightsCount, spoLightsCount );
				if ( auto numChunks = getLightsMortonCodeChunkCount( totalValues );
					numChunks > 1u )
					result += ( ( numChunks - 1u ) % 2u );

				return result;
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

			bool doIsEnabled( FrustumClusters const & clusters )const
			{
				return clusters.getScene().hasClusteredLights();
			}

		private:
			RenderDevice const & m_device;
			ClustersConfig const & m_config;
			Map< uint32_t, ProgramData > m_programs;
		};
	}

	//*********************************************************************************************

	void createAssignLightsToClustersPass( crg::FramePassGroup & graph
		, RenderDevice const & device
		, FrustumClusters & clusters
		, CameraUbo const & clustersCameraUbo
		, BufferBase const & allLightsAABB
		, BufferBase const & clustersAABB
		, BufferBase const & pointLightBVH
		, BufferBase const & spotLightBVH
		, ClustersLightSortAttachs const & outputSortAttachs
		, BufferBase & pointLightClusterIndex
		, BufferBase & spotLightClusterIndex
		, BufferBase & pointLightClusterGrid
		, BufferBase & spotLightClusterGrid )
	{
		auto const & lights = clusters.getScene().getLightCache();

		auto & pass = graph.createPass( "AssignLightsToClusters"
			, [&clusters, &device]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runGraph )
			{
				auto result = makeRawUnique< dspclst::FramePass >( framePass
					, context
					, runGraph
					, device
					, clusters
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
		pass.addInputStorageT( *outputSortAttachs.pointLightIndices, dspclst::Bindings::ePointLightIndices );
		pass.addInputStorageT( *outputSortAttachs.spotLightIndices, dspclst::Bindings::eSpotLightIndices );

		pointLightClusterIndex.setLastAttach( pass.addClearableOutputStorageBufferT( pointLightClusterIndex.bufferViewId, dspclst::Bindings::ePointLightIndex ) );
		pointLightClusterGrid.setLastAttach( pass.addClearableOutputStorageBufferT( pointLightClusterGrid.bufferViewId, dspclst::Bindings::ePointLightCluster ) );
		spotLightClusterIndex.setLastAttach( pass.addClearableOutputStorageBufferT( spotLightClusterIndex.bufferViewId, dspclst::Bindings::eSpotLightIndex ) );
		spotLightClusterGrid.setLastAttach( pass.addClearableOutputStorageBufferT( spotLightClusterGrid.bufferViewId, dspclst::Bindings::eSpotLightCluster ) );
	}

	//*********************************************************************************************
}
