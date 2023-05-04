#include "Castor3D/Render/Clustered/AssignLightsToClusters.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/LightCache.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/Clustered/FrustumClusters.hpp"
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

namespace castor3d
{
	//*********************************************************************************************

	namespace dspclst
	{
		enum BindingPoints
		{
			eCamera,
			eLights,
			eClusters,
			eClustersAABB,
			ePointLightGridIndex,
			ePointLightGridCluster,
			eSpotLightGridIndex,
			eSpotLightGridCluster,
		};

		static ShaderPtr createShader()
		{
			using namespace sdw;
			ComputeWriter writer;

			// Inputs
			C3D_Camera( writer
				, eCamera
				, 0u );
			shader::LightsBuffer lights{ writer
				, eLights
				, 0u };
			C3D_Clusters( writer
				, eClusters
				, 0u );
			C3D_ClustersAABB( writer
				, eClustersAABB
				, 0u );
			C3D_PointLightGridIndices( writer
				, ePointLightGridIndex
				, 0u );
			C3D_PointLightGridClusters( writer
				, ePointLightGridCluster
				, 0u );
			C3D_SpotLightGridIndices( writer
				, eSpotLightGridIndex
				, 0u );
			C3D_SpotLightGridClusters( writer
				, eSpotLightGridCluster
				, 0u );

			auto gsPointLightStartOffset = writer.declSharedVariable< sdw::UInt >( "gsPointLightStartOffset" );
			auto gsPointLights = writer.declSharedVariable< shader::AppendArrayT< sdw::UInt > >( "gsPointLights"
				, true, "U32", MaxLightsPerCluster );

			auto gsSpotLightStartOffset = writer.declSharedVariable< sdw::UInt >( "gsSpotLightStartOffset" );
			auto gsSpotLights = writer.declSharedVariable< shader::AppendArrayT< sdw::UInt > >( "gsSpotLights"
				, true, "U32", MaxLightsPerCluster );

			auto gsClusterIndex3D = writer.declSharedVariable< sdw::U32Vec3 >( "gsClusterIndex3D" );
			auto gsClusterIndex1D = writer.declSharedVariable< sdw::UInt32 >( "gsClusterIndex1D" );
			auto gsClusterAABB = writer.declSharedVariable< shader::AABB >( "gsClusterAABB" );
			auto gsClusterSphere = writer.declSharedVariable< sdw::Vec4 >( "gsClusterSphere" );

			shader::Utils utils{ writer };

			auto sphereInsideAABB = writer.implementFunction< sdw::Boolean >( "sphereInsideAABB"
				, [&]( sdw::Vec3 const & sphereCenter
					, sdw::Float const & sphereRadius
					, shader::AABB const & aabb )
				{
					auto sqDistance = writer.declLocale( "sqDistance"
						, 0.0_f );
					auto v = writer.declLocale( "v"
						, 0.0_f );

					for ( int i = 0; i < 3; ++i )
					{
						v = sphereCenter[i];

						IF( writer, v < aabb.min()[i] )
						{
							sqDistance += pow( aabb.min()[i] - v, 2.0_f );
						}
						FI;
						IF( writer, v > aabb.max()[i] )
						{
							sqDistance += pow( v - aabb.max()[i], 2.0_f );
						}
						FI;
					}

					writer.returnStmt( sqDistance <= sphereRadius * sphereRadius );
				}
				, sdw::InVec3{ writer, "sphereCenter" }
				, sdw::InFloat{ writer, "sphereRadius" }
				, shader::InAABB{ writer, "aabb" } );

			auto coneInsideSphere = writer.implementFunction< sdw::Boolean >( "coneInsideSphere"
				, [&]( shader::Cone const cone
					, sdw::Vec4 const & sphere )
				{
					auto V = writer.declLocale( "V"
						, sphere.xyz() - cone.origin() );
					auto lenSqV = writer.declLocale( "lenSqV"
						, dot( V, V ) );
					auto lenV1 = writer.declLocale( "lenV1"
						, dot( V, cone.direction() ) );
					auto distanceClosestPoint = writer.declLocale( "distanceClosestPoint"
						, cos( 2.0f * cone.aperture() ) * sqrt( lenSqV - lenV1 * lenV1 ) - lenV1 * sin( 2.0f * cone.aperture() ) );

					auto angleCull = distanceClosestPoint > sphere.w();
					auto frontCull = lenV1 > sphere.w() + cone.size();
					auto backCull = lenV1 < -sphere.w();

					writer.returnStmt( !( angleCull || frontCull || backCull ) );
				}
				, shader::InCone{ writer, "cone" }
				, sdw::InVec4{ writer, "sphere" } );

			writer.implementMainT< VoidT >( MaxLightsPerCluster
				, [&]( ComputeIn in )
				{
					auto groupIndex = writer.declLocale( "groupIndex"
						, writer.cast< sdw::UInt >( in.localInvocationIndex ) );

					IF( writer, groupIndex == 0u )
					{
						gsPointLights.resetCount();
						gsSpotLights.resetCount();

						gsClusterIndex3D = in.workGroupID;
						gsClusterIndex1D = c3d_clustersData.computeClusterIndex1D( gsClusterIndex3D );
						gsClusterAABB = c3D_clustersAABB[gsClusterIndex1D];
						auto aabbCenter = writer.declLocale( "aabbCenter"
							, gsClusterAABB.min().xyz() + ( gsClusterAABB.max().xyz() - gsClusterAABB.min().xyz() ) / 2.0f );
						gsClusterSphere = vec4( aabbCenter, distance( gsClusterAABB.max().xyz(), aabbCenter ) );
					}
					FI;

					writer.controlBarrier( sdw::type::Scope::eWorkgroup
						, sdw::type::Scope::eWorkgroup
						, ( sdw::type::MemorySemanticsMask::eAcquireRelease
							| sdw::type::MemorySemanticsMask::eWorkgroupMemory ) );

					// Intersect point lights against AABB.
					auto cur = writer.declLocale( "cur"
						, lights.getDirectionalsEnd() + groupIndex * PointLight::LightDataComponents );
					auto end = writer.declLocale( "end"
						, lights.getPointsEnd() );

					WHILE( writer, cur < lights.getPointsEnd() )
					{
						auto point = writer.declLocale( "point"
							, lights.getPointLight( cur ) );

						IF( writer, sphereInsideAABB( c3d_cameraData.worldToCurView( vec4( point.position(), 1.0_f ) ).xyz()
							, point.base().farPlane()
							, gsClusterAABB ) )
						{
							gsPointLights.appendData( cur, MaxLightsPerCluster );
						}
						FI;

						cur += PointLight::LightDataComponents * MaxLightsPerCluster;
					}
					ELIHW;

					// Intersect spot lights against AABB.
					cur = end + groupIndex * SpotLight::LightDataComponents;
					end = lights.getSpotsEnd();

					WHILE( writer, cur < end )
					{
						auto spot = writer.declLocale( "spot"
							, lights.getSpotLight( cur ) );

#if 1
						IF( writer, sphereInsideAABB( c3d_cameraData.worldToCurView( vec4( spot.position(), 1.0_f ) ).xyz()
							, spot.base().farPlane()
							, gsClusterAABB ) )
#else
						IF( writer, coneInsideSphere( shader::Cone{ c3d_cameraData.worldToCurView( vec4( spot.position(), 1.0_f ) ).xyz()
							, spot.direction()
							, spot.base().farPlane()
							, spot.outerCutOff()
							, spot.outerCutOffCos()
							, spot.outerCutOffSin() }
							, gsClusterSphere ) )
#endif
						{
							gsSpotLights.appendData( cur, MaxLightsPerCluster );
						}
						FI;

						cur += SpotLight::LightDataComponents * MaxLightsPerCluster;
					}
					ELIHW;

					writer.controlBarrier( sdw::type::Scope::eWorkgroup
						, sdw::type::Scope::eWorkgroup
						, ( sdw::type::MemorySemanticsMask::eAcquireRelease
							| sdw::type::MemorySemanticsMask::eWorkgroupMemory ) );

					// Now update the global light grids with the light lists and light counts.
					IF( writer, groupIndex == 0u )
					{
						gsPointLights.getCount() = min( sdw::UInt{ MaxLightsPerCluster }, gsPointLights.getCount() );
						gsPointLightStartOffset = sdw::atomicAdd( c3d_pointLightGridListCount, gsPointLights.getCount() );
						c3d_pointLightGridClusters[gsClusterIndex1D] = sdw::uvec2( gsPointLightStartOffset, gsPointLights.getCount() );

						gsSpotLights.getCount() = min( sdw::UInt{ MaxLightsPerCluster }, gsSpotLights.getCount() );
						gsSpotLightStartOffset = sdw::atomicAdd( c3d_spotLightGridListCount, gsSpotLights.getCount() );
						c3d_spotLightGridClusters[gsClusterIndex1D] = sdw::uvec2( gsSpotLightStartOffset, gsSpotLights.getCount() );
					}
					FI;

					writer.controlBarrier( sdw::type::Scope::eWorkgroup
						, sdw::type::Scope::eWorkgroup
						, ( sdw::type::MemorySemanticsMask::eAcquireRelease
							| sdw::type::MemorySemanticsMask::eWorkgroupMemory ) );

					// Now update the global light index lists with the group shared light lists.
					IF( writer, groupIndex < gsPointLights.getCount() )
					{
						c3d_pointLightGridIndices[gsPointLightStartOffset + groupIndex] = gsPointLights[groupIndex];
					}
					FI;

					IF( writer, groupIndex < gsSpotLights.getCount() )
					{
						c3d_spotLightGridIndices[gsSpotLightStartOffset + groupIndex] = gsSpotLights[groupIndex];
					}
					FI;
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		class FramePass
			: private castor::DataHolderT< ShaderModule >
			, private castor::DataHolderT< ashes::PipelineShaderStageCreateInfoArray >
			, private castor::DataHolderT< bool >
			, public crg::ComputePass
		{
			using ShaderHolder = DataHolderT< ShaderModule >;
			using CreateInfoHolder = DataHolderT< ashes::PipelineShaderStageCreateInfoArray >;
			using EnabledHolder = DataHolderT< bool >;

		public:
			FramePass( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph
				, RenderDevice const & device
				, FrustumClusters const & clusters
				, crg::cp::Config config )
				: ShaderHolder{ ShaderModule{ VK_SHADER_STAGE_COMPUTE_BIT, "AssignLightsToClusters", dspclst::createShader() } }
				, CreateInfoHolder{ ashes::PipelineShaderStageCreateInfoArray{ makeShaderState( device, ShaderHolder::getData() ) } }
				, EnabledHolder{ true }
				, crg::ComputePass{framePass
					, context
					, graph
					, crg::ru::Config{}
					, config
						.program( ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( CreateInfoHolder::getData() ) )
						.enabled( &clusters.needsLightsUpdate() )
						.end( RecordCallback{ [this]( crg::RecordContext & ctx, VkCommandBuffer cb, uint32_t idx ) { doPostRecord( ctx, cb, idx ); } } ) }
			{
			}

		private:
			void doPostRecord( crg::RecordContext & context
				, VkCommandBuffer commandBuffer
				, uint32_t index )
			{
				for ( auto & attach : m_pass.buffers )
				{
					auto buffer = attach.buffer;

					if ( !attach.isNoTransition()
						&& attach.isStorageBuffer()
						&& attach.isClearableBuffer() )
					{
						auto currentState = context.getAccessState( buffer.buffer.buffer
							, buffer.range );
						context.memoryBarrier( commandBuffer
							, buffer.buffer.buffer
							, buffer.range
							, currentState.access
							, currentState.pipelineStage
							, { VK_ACCESS_SHADER_READ_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT } );
					}
				}
			}
		};
	}

	//*********************************************************************************************

	crg::FramePass const & createAssignLightsToClustersPass( crg::FramePassGroup & graph
		, crg::FramePass const * previousPass
		, RenderDevice const & device
		, LightCache const & lights
		, CameraUbo const & cameraUbo
		, FrustumClusters const & clusters )
	{
		auto & pass = graph.createPass( "AssignLightsToClusters"
			, [&clusters, &device]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				auto result = std::make_unique< dspclst::FramePass >( framePass
					, context
					, graph
					, device
					, clusters
					, crg::cp::Config{}
						.groupCountX( clusters.getDimensions()->x )
						.groupCountY( clusters.getDimensions()->y )
						.groupCountZ( clusters.getDimensions()->z ) );
				device.renderSystem.getEngine()->registerTimer( framePass.getFullName()
					, result->getTimer() );
				return result;
			} );
		pass.addDependency( *previousPass );
		cameraUbo.createPassBinding( pass, dspclst::eCamera );
		lights.createPassBinding( pass, dspclst::eLights );
		clusters.getClustersUbo().createPassBinding( pass, dspclst::eClusters );
		createInputStoragePassBinding( pass, uint32_t( dspclst::eClustersAABB ), "C3D_ClustersAABB", clusters.getAabbBuffer().getBuffer(), 0u, ashes::WholeSize );
		createClearableOutputStorageBinding( pass, uint32_t( dspclst::ePointLightGridIndex ), "C3D_PointLightGridIndices", clusters.getPointLightIndexBuffer().getBuffer(), 0u, ashes::WholeSize );
		createClearableOutputStorageBinding( pass, uint32_t( dspclst::ePointLightGridCluster ), "C3D_PointLightGridClusters", clusters.getPointLightClusterBuffer().getBuffer(), 0u, ashes::WholeSize );
		createClearableOutputStorageBinding( pass, uint32_t( dspclst::eSpotLightGridIndex ), "C3D_SpotLightGridIndices", clusters.getSpotLightIndexBuffer().getBuffer(), 0u, ashes::WholeSize );
		createClearableOutputStorageBinding( pass, uint32_t( dspclst::eSpotLightGridCluster ), "C3D_LSpotightGridClusters", clusters.getSpotLightClusterBuffer().getBuffer(), 0u, ashes::WholeSize );
		return pass;
	}

	//*********************************************************************************************
}
