#include "Castor3D/Render/Volumetric/AssignLightsToFroxels.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/LightCache.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/Clustered/FrustumClusters.hpp"
#include "Castor3D/Render/Volumetric/FrustumFroxels.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/Shaders/GlslBVHBase.hpp"
#include "Castor3D/Shader/Shaders/GlslClusteredLights.hpp"
#include "Castor3D/Shader/Shaders/GlslFrustum.hpp"
#include "Castor3D/Shader/Shaders/GlslLight.hpp"
#include "Castor3D/Shader/Shaders/GlslLightsBVH.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Ubos/CameraUbo.hpp"
#include "Castor3D/Shader/Ubos/ClustersUbo.hpp"
#include "Castor3D/Shader/Ubos/FroxelsUbo.hpp"

#include <RenderGraph/FramePassGroup.hpp>
#include <RenderGraph/RunnablePasses/ComputePass.hpp>

#include <ShaderWriter/Source.hpp>

namespace c3d
{
	//*********************************************************************************************

	namespace asnfxl
	{
		enum class Bindings
		{
			eCamera,
			eLights,
			eClusters,
			eFroxels,
			eAllLightsAABB,
			ePointLightBVH,
			eSpotLightBVH,
			ePointLightIndices,
			eSpotLightIndices,
			eFroxelsPointLights,
			eFroxelsSpotLights,
			eCounters,
			eIndirect,
		};

		static ShaderPtr createShader( RenderDevice const & device )
		{
			uint32_t NumThreads = 32u;

			sdw::ComputeWriter writer{ &c3d::getEngine( device ).getShaderAllocator() };
			shader::Utils utils{ writer };

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
			C3D_Froxels( writer
				, Bindings::eFroxels
				, 0u );
			C3D_AllLightsAABB( writer
				, Bindings::eAllLightsAABB
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

			auto froxelsPointLightsBuffer = writer.declStorageBuffer( "c3d_froxelsPointLightsBuffer", uint32_t( Bindings::eFroxelsPointLights ), 0u );
			auto c3d_froxelsPointLights = froxelsPointLightsBuffer.declMemberArray< sdw::U32Vec2 >( "fp" );
			froxelsPointLightsBuffer.end();

			auto froxelsSpotLightsBuffer = writer.declStorageBuffer( "c3d_froxelsSpotLightsBuffer", uint32_t( Bindings::eFroxelsSpotLights ), 0u );
			auto c3d_froxelsSpotLights = froxelsSpotLightsBuffer.declMemberArray< sdw::U32Vec2 >( "fs" );
			froxelsSpotLightsBuffer.end();

			auto countersBuffer = writer.declStorageBuffer( "c3d_countersBuffer", uint32_t( Bindings::eCounters ), 0u );
			auto c3d_counters = countersBuffer.declMemberArray< sdw::UInt >( "c" );
			countersBuffer.end();

			auto indirectDispatchBuffer = writer.declStorageBuffer( "c3d_indirectDispatchBuffer", uint32_t( Bindings::eIndirect ), 0u );
			auto c3d_indirectDispatch = indirectDispatchBuffer.declMemberArray< sdw::UInt >( "id" );
			indirectDispatchBuffer.end();

			auto gsLightStartOffset = writer.declSharedVariable< sdw::UInt >( "gsLightStartOffset" );
			auto gsProcessedLightsCount = writer.declSharedVariable< sdw::UInt >( "gsProcessedLightsCount" );
			auto gsFroxelGroupMinIndex3D = writer.declSharedVariable< sdw::U32Vec3 >( "gsFroxelGroupMinIndex3D" );
			auto gsFroxelGroupMaxIndex3D = writer.declSharedVariable< sdw::U32Vec3 >( "gsFroxelGroupMaxIndex3D" );
			auto gsFroxelGroupMinIndex1D = writer.declSharedVariable< sdw::UInt >( "gsFroxelGroupMinIndex1D" );
			auto gsFroxelGroupFrustum = writer.declSharedVariable< shader::Frustum >( "gsFroxelGroupFrustum" );

			shader::BVHBaseT< shader::Frustum > bvh{ writer };
			shader::LightsBVHT< shader::Frustum > lightsBvh{ writer, c3d_allLightsAABB, MaxLightsPerFroxel };

			auto updateIndirectDispatchBuffer = writer.implementFunction< sdw::Void >( "c3d_updateIndirectDispatchBuffer"
				, [&writer, &gsLightStartOffset, &gsProcessedLightsCount, &c3d_indirectDispatch]( sdw::UInt const & indirectDispatchOffset )
				{
					// update thread groups count if needed
					auto totalLightsCountPrevious = writer.declLocale( "totalLightsCountPrevious", gsLightStartOffset );
					auto totalLightsCountNew = writer.declLocale( "totalLightsCountNew", gsLightStartOffset + gsProcessedLightsCount );

					//
					auto additionalGroupsComputeCS = writer.declLocale( "additionalGroupsComputeCS", 0_u );
					// first write, we need to add a new group
					sdwIF( writer, totalLightsCountPrevious == 0_u )
					{
						additionalGroupsComputeCS = 1_u;
					}
					sdwFI

					// add as many groups as we go through group size limit
					additionalGroupsComputeCS += ( totalLightsCountNew / FroxelIntegrateTileSize ) - ( totalLightsCountPrevious / FroxelIntegrateTileSize );
					atomicAdd( c3d_indirectDispatch[indirectDispatchOffset + 0u], additionalGroupsComputeCS );
				}
				, sdw::InUInt{ writer, "indirectDispatchOffset" } );

			auto processPointLights = writer.implementFunction< sdw::Void >( "c3d_processPointLights"
				, [&gsProcessedLightsCount, &gsLightStartOffset, gsFroxelGroupMinIndex1D
					, &c3d_counters, &c3d_froxelsPointLights, updateIndirectDispatchBuffer
					, &writer, &lightsBvh, NumThreads]( sdw::UInt const & groupIndex )
				{
					sdwIF( writer, groupIndex == 0u )
					{
						gsProcessedLightsCount = lightsBvh.getTraversedLightCount();
						sdwIF( writer, gsProcessedLightsCount > 0_u )
						{
							updateIndirectDispatchBuffer( 0_u );
							gsProcessedLightsCount = min( sdw::UInt{ MaxLightsPerFroxel }, gsProcessedLightsCount );
							gsLightStartOffset = sdw::atomicAdd( c3d_counters[0u], gsProcessedLightsCount );
						}
						sdwFI
					}
					sdwFI

					shader::groupMemoryBarrierWithGroupSync( writer );

					// Now update the global light index lists with the group shared light lists.
					sdwFOR( writer, sdw::UInt, i, groupIndex, i < gsProcessedLightsCount, i += NumThreads )
					{
						c3d_froxelsPointLights[gsLightStartOffset + i] = sdw::uvec2( lightsBvh.getTraversedLightID( i ), gsFroxelGroupMinIndex1D );
					}
					sdwROF
				}
				, sdw::InUInt{ writer, "groupIndex" } );

			auto processSpotLights = writer.implementFunction< sdw::Void >( "c3d_processSpotLights"
				, [&gsProcessedLightsCount, &gsLightStartOffset, gsFroxelGroupMinIndex1D
					, &c3d_counters, &c3d_froxelsSpotLights, updateIndirectDispatchBuffer
					, &writer, &lightsBvh, NumThreads]( sdw::UInt const & groupIndex )
				{
					sdwIF( writer, groupIndex == 0u )
					{
						gsProcessedLightsCount = lightsBvh.getTraversedLightCount();
						sdwIF( writer, gsProcessedLightsCount > 0_u )
						{
							updateIndirectDispatchBuffer( 3_u );
							gsProcessedLightsCount = min( sdw::UInt{ MaxLightsPerFroxel }, gsProcessedLightsCount );
							gsLightStartOffset = sdw::atomicAdd( c3d_counters[1u], gsProcessedLightsCount );
						}
						sdwFI
					}
					sdwFI

					shader::groupMemoryBarrierWithGroupSync( writer );

					// Now update the global light index lists with the group shared light lists.
					sdwFOR( writer, sdw::UInt, i, groupIndex, i < gsProcessedLightsCount, i += NumThreads )
					{
						c3d_froxelsSpotLights[gsLightStartOffset + i] = sdw::uvec2( lightsBvh.getTraversedLightID( i ), gsFroxelGroupMinIndex1D );
					}
					sdwROF
				}
				, sdw::InUInt{ writer, "groupIndex" } );

			auto initSharedMemory = writer.implementFunction< sdw::Void >( "c3d_initSharedMemory"
				, [&c3d_cameraData, &c3d_froxelsData, &gsFroxelGroupMinIndex1D, &gsFroxelGroupMinIndex3D, &gsFroxelGroupMaxIndex3D, &gsFroxelGroupFrustum
					, &writer, &utils]( sdw::U32Vec3 const & groupID, sdw::UInt const & groupIndex )
				{
					auto froxelSize = writer.declLocale( "froxelSize"
						, c3d_froxelsData.froxelSize() );
					auto minFroxelIndex3D = writer.declLocale( "minFroxelIndex3D"
						, sdw::u32vec3( groupID * shader::FroxelsData::FroxelGroupSize ) );
					auto maxFroxelIndex3D = writer.declLocale( "maxFroxelIndex3D"
						, minFroxelIndex3D + ( shader::FroxelsData::FroxelGroupSize - 1u ) );
					maxFroxelIndex3D.x() = min( maxFroxelIndex3D.x(), c3d_froxelsData.dimensions().x() - 1u );
					maxFroxelIndex3D.y() = min( maxFroxelIndex3D.y(), c3d_froxelsData.dimensions().y() - 1u );
					maxFroxelIndex3D.z() = min( maxFroxelIndex3D.z(), c3d_froxelsData.dimensions().z() - 1u );
					auto minDepthBounds = writer.declLocale( "minDepthBounds"
						, c3d_froxelsData.getDepthBounds( minFroxelIndex3D.z() ) );
					auto maxDepthBounds = writer.declLocale( "maxDepthBounds"
						, c3d_froxelsData.getDepthBounds( maxFroxelIndex3D.z() ) );
					auto minScreenPosition = writer.declLocale( "minScreenPosition"
						, sdw::vec3( sdw::vec2( minFroxelIndex3D.xy() ) * froxelSize
							, c3d_froxelsData.getScreenSpaceDepth( minDepthBounds.x() ) ) );
					auto maxScreenPosition = writer.declLocale( "maxScreenPosition"
						, sdw::vec3( sdw::vec2( maxFroxelIndex3D.xy() + 1u ) * froxelSize
							, c3d_froxelsData.getScreenSpaceDepth( maxDepthBounds.y() ) ) );

					auto vsNearTopLeft = writer.declLocale( "vsNearTopLeft"
						, c3d_froxelsData.getViewSpacePosition( utils, c3d_cameraData.getInvProjMtx(), minScreenPosition.x(), minScreenPosition.y(), minScreenPosition.z() ) );
					auto vsNearTopRight = writer.declLocale( "vsNearTopRight"
						, c3d_froxelsData.getViewSpacePosition( utils, c3d_cameraData.getInvProjMtx(), maxScreenPosition.x(), minScreenPosition.y(), minScreenPosition.z() ) );
					auto vsFarTopLeft = writer.declLocale( "vsFarTopLeft"
						, c3d_froxelsData.getViewSpacePosition( utils, c3d_cameraData.getInvProjMtx(), minScreenPosition.x(), minScreenPosition.y(), maxScreenPosition.z() ) );
					auto vsFarTopRight = writer.declLocale( "vsFarTopRight"
						, c3d_froxelsData.getViewSpacePosition( utils, c3d_cameraData.getInvProjMtx(), maxScreenPosition.x(), minScreenPosition.y(), maxScreenPosition.z() ) );
					auto vsNearBottomLeft = writer.declLocale( "vsNearBottomLeft"
						, c3d_froxelsData.getViewSpacePosition( utils, c3d_cameraData.getInvProjMtx(), minScreenPosition.x(), maxScreenPosition.y(), minScreenPosition.z() ) );
					auto vsNearBottomRight = writer.declLocale( "vsNearBottomRight"
						, c3d_froxelsData.getViewSpacePosition( utils, c3d_cameraData.getInvProjMtx(), maxScreenPosition.x(), maxScreenPosition.y(), minScreenPosition.z() ) );
					auto vsFarBottomLeft = writer.declLocale( "vsFarBottomLeft"
						, c3d_froxelsData.getViewSpacePosition( utils, c3d_cameraData.getInvProjMtx(), minScreenPosition.x(), maxScreenPosition.y(), maxScreenPosition.z() ) );
					auto vsFarBottomRight = writer.declLocale( "vsFarBottomRight"
						, c3d_froxelsData.getViewSpacePosition( utils, c3d_cameraData.getInvProjMtx(), maxScreenPosition.x(), maxScreenPosition.y(), maxScreenPosition.z() ) );
					gsFroxelGroupFrustum.set( vsNearTopLeft, vsNearTopRight
							, vsNearBottomLeft, vsNearBottomRight
							, vsFarTopLeft, vsFarTopRight
							, vsFarBottomLeft, vsFarBottomRight );
					gsFroxelGroupMinIndex1D = c3d_froxelsData.computeIndex1D( minFroxelIndex3D );
					gsFroxelGroupMinIndex3D = minFroxelIndex3D;
					gsFroxelGroupMaxIndex3D = maxFroxelIndex3D;
				}
				, sdw::InU32Vec3{ writer, "groupID" }
				, sdw::InUInt{ writer, "groupIndex" } );

			writer.implementMainT< sdw::VoidT >( NumThreads
				, [&writer, &lights, &c3d_cameraData, &c3d_clustersData, &c3d_pointLightIndices, &c3d_spotLightIndices
					, &c3d_indirectDispatch, &c3d_pointLightBVH, &c3d_spotLightBVH
					, &gsFroxelGroupMinIndex3D, &gsFroxelGroupMaxIndex3D, &gsFroxelGroupFrustum
					, &bvh, &lightsBvh, &initSharedMemory, &processPointLights, &processSpotLights]( sdw::ComputeIn const & in )
				{
					auto const & groupIndex = in.localInvocationIndex;
					auto const & groupID = in.workGroupID;

					sdwIF( writer, groupIndex == 0_u )
					{
						initSharedMemory( groupID, groupIndex );
						bvh.resetTraversal();
						lightsBvh.resetTraversal();

						// one thread must write the indirect dispatch Y & Z size
						sdwIF( writer, groupID.x() == 0u && groupID.y() == 0u && groupID.z() == 0u )
						{
							c3d_indirectDispatch[0 + 1u] = 1u;
							c3d_indirectDispatch[0 + 2u] = 1u;

							c3d_indirectDispatch[3 + 1u] = 1u;
							c3d_indirectDispatch[3 + 2u] = 1u;
						}
						sdwFI
					}
					sdwFI

					shader::groupMemoryBarrierWithGroupSync( writer );

					//
					sdwIF( writer, gsFroxelGroupMaxIndex3D.x() >= gsFroxelGroupMinIndex3D.x()
						&& gsFroxelGroupMaxIndex3D.y() >= gsFroxelGroupMinIndex3D.y()
						&& gsFroxelGroupMaxIndex3D.z() >= gsFroxelGroupMinIndex3D.z() )
					{
						// Point lights first phase
						lightsBvh.traversePointLights( c3d_pointLightIndices, c3d_pointLightBVH
							, bvh, gsFroxelGroupFrustum, c3d_clustersData.pointLightLevels(), c3d_clustersData.pointLightCount(), groupIndex );

						// Point lights second phase
						processPointLights( groupIndex );

						shader::groupMemoryBarrierWithGroupSync( writer );

						// Reset before processing spot lights
						sdwIF( writer, groupIndex == 0_u )
						{
							bvh.resetTraversal();
							lightsBvh.resetTraversal();
						}
						sdwFI
						shader::groupMemoryBarrierWithGroupSync( writer );

						// Spot lights first phase
						lightsBvh.traverseSpotLights( c3d_spotLightIndices, lights, c3d_cameraData, c3d_spotLightBVH
							, bvh, gsFroxelGroupFrustum, c3d_clustersData.pointLightCount(), c3d_clustersData.spotLightLevels(), c3d_clustersData.spotLightCount(), groupIndex );

						// Spot lights second phase
						processSpotLights( groupIndex );
					}
					sdwFI
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
						.getPassIndex( RunnablePass::GetPassIndexCallback( [this, &clusters](){ return doGetPassIndex( clusters ); } ) )
						.programCreator( { 2u, [this]( uint32_t passIndex ){ return doCreateProgram( passIndex ); } } ) }
				, m_device{ device }
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
					program.shaderModule = ShaderModule{ VK_SHADER_STAGE_COMPUTE_BIT, cuT( "AssignLightsToFroxels" ), asnfxl::createShader( m_device ) };
					program.stages = ashes::PipelineShaderStageCreateInfoArray{ makeShaderState( m_device, program.shaderModule ) };
				}

				return ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( it->second.stages );
			}

		private:
			RenderDevice const & m_device;
			Map< uint32_t, ProgramData > m_programs;
		};
	}

	//*********************************************************************************************

	void createAssignLightsToFroxelsPass( crg::FramePassGroup & graph
		, RenderDevice const & device
		, FrustumFroxels const & froxels
		, FrustumClusters const & clusters
		, BufferBase & froxelsPointLightIndex
		, BufferBase & froxelsSpotLightIndex
		, BufferBase & counters
		, BufferBase & indirect )
	{
		Scene const & scene = froxels.getScene();
		auto const & lights = scene.getLightCache();

		auto & pass = graph.createPass( "AssignLightsToFroxels"
			, [&device, &froxels, &clusters, &scene]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runGraph )
			{
				auto result = makeRawUnique< asnfxl::FramePass >( framePass, context, runGraph, device
					, clusters
					, crg::cp::Config{}
						.groupCountX( froxels.getDimensions()->x )
						.groupCountY( froxels.getDimensions()->y )
						.groupCountZ( froxels.getDimensions()->z )
						.isEnabled( crg::RunnablePass::IsEnabledCallback( [&scene](){ return scene.hasClusteredLights(); } ) ) );
				c3d::getEngine( device ).registerTimer( makeString( framePass.getFullName() )
					, result->getTimer() );
				return result;
			} );
		clusters.getCameraUbo().createPassBinding( pass, asnfxl::Bindings::eCamera );
		lights.createPassBindingT( pass, asnfxl::Bindings::eLights );
		clusters.getClustersUbo().createPassBinding( pass, asnfxl::Bindings::eClusters );
		froxels.getFroxelsUbo().createPassBinding( pass, asnfxl::Bindings::eFroxels );
		pass.addInputStorageT( *clusters.getAllLightsAABBBuffer().getLastAttach(), asnfxl::Bindings::eAllLightsAABB );
		pass.addInputStorageT( *clusters.getPointLightBVHBuffer().getLastAttach(), asnfxl::Bindings::ePointLightBVH );
		pass.addInputStorageT( *clusters.getSpotLightBVHBuffer().getLastAttach(), asnfxl::Bindings::eSpotLightBVH );
		pass.addInputStorageT( clusters.getPointLightIndicesBuffer(), asnfxl::Bindings::ePointLightIndices );
		pass.addInputStorageT( clusters.getSpotLightIndicesBuffer(), asnfxl::Bindings::eSpotLightIndices );

		froxelsPointLightIndex.setLastAttach( pass.addClearableOutputStorageBufferT( froxelsPointLightIndex.bufferViewId, asnfxl::Bindings::eFroxelsPointLights ) );
		froxelsSpotLightIndex.setLastAttach( pass.addClearableOutputStorageBufferT( froxelsSpotLightIndex.bufferViewId, asnfxl::Bindings::eFroxelsSpotLights ) );
		counters.setLastAttach( pass.addClearableOutputStorageBufferT( counters.bufferViewId, asnfxl::Bindings::eCounters ) );
		indirect.setLastAttach( pass.addClearableOutputStorageBufferT( indirect.bufferViewId, asnfxl::Bindings::eIndirect ) );
	}

	//*********************************************************************************************
}
