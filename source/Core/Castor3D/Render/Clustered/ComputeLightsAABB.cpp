#include "Castor3D/Render/Clustered/ComputeLightsAABB.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/Clustered/FrustumClusters.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/Light/PointLight.hpp"
#include "Castor3D/Scene/Light/SpotLight.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/Shaders/GlslAABB.hpp"
#include "Castor3D/Shader/Shaders/GlslBaseIO.hpp"
#include "Castor3D/Shader/Shaders/GlslClusteredLights.hpp"
#include "Castor3D/Shader/Shaders/GlslLight.hpp"
#include "Castor3D/Shader/Ubos/CameraUbo.hpp"
#include "Castor3D/Shader/Ubos/ClustersUbo.hpp"

#include <CastorUtils/Design/DataHolder.hpp>

#include <ShaderWriter/Source.hpp>
#include <ShaderWriter/TraditionalGraphicsWriter.hpp>

#include <RenderGraph/FramePassGroup.hpp>
#include <RenderGraph/RunnablePasses/ComputePass.hpp>

namespace castor3d
{
	//*********************************************************************************************

	namespace cptlgtb
	{
		enum BindingPoints
		{
			eMainCamera,
			eClustersCamera,
			eClusters,
			eLights,
			eAllLightsAABB,
		};

		static ShaderPtr createShader( RenderDevice const & device
			, ClustersConfig const & config )
		{
			sdw::ComputeWriter writer{ &device.renderSystem.getEngine()->getShaderAllocator() };

			// Inputs
			C3D_CameraNamed( writer
				, Main
				, eMainCamera
				, 0u );
			C3D_CameraNamed( writer
				, Clusters
				, eClustersCamera
				, 0u );
			C3D_Clusters( writer
				, eClusters
				, 0u
				, &config );
			shader::LightsBuffer lights{ writer
				, eLights
				, 0u };
			C3D_AllLightsAABB( writer
				, eAllLightsAABB
				, 0u );

			auto loadPointLightAABB = writer.implementFunction< shader::AABB >( "loadPointLightAABB"
				, [&]( sdw::UInt const & lightIndex )
				{
					auto lightOffset = writer.declLocale( "lightOffset"
						, lights.getDirectionalsEnd() + lightIndex * PointLight::LightDataComponents );
					auto point = writer.declLocale( "point"
						, lights.getPointLight( lightOffset ) );
					auto vsPosition = writer.declLocale( "vsPosition"
						, c3d_cameraDataClusters.worldToCurView( vec4( point.position(), 1.0_f ) ).xyz() );

					writer.returnStmt( shader::AABB{ vsPosition, computeRange( point ) } );
				}
				, sdw::InUInt{ writer, "lightIndex" } );

			auto getConeAABB = writer.implementFunction< shader::AABB >( "getConeAABB"
				, [&]( sdw::Vec3 const & vsApex
					, sdw::Vec3 const & vsBase
					, sdw::Float const & fBaseRadius )
				{
					auto a = writer.declLocale( "a"
						, vsBase - vsApex );
					auto e = writer.declLocale( "e"
						, sqrt( vec3( 1.0_f ) - a * a / dot( a, a ) ) );

					writer.returnStmt( shader::AABB( vec4( min( vsApex, vsBase - e * fBaseRadius ), 1.0f )
						, vec4( max( vsApex, vsBase + e * fBaseRadius ), 1.0f ) ) );
				}
				, sdw::InVec3{ writer, "vsApex" }
				, sdw::InVec3{ writer, "vsBase" }
				, sdw::InFloat{ writer, "fBaseRadius" } );

			auto loadSpotLightAABB = writer.implementFunction< shader::AABB >( "loadSpotLightAABB"
				, [&]( sdw::UInt const & lightIndex )
				{
					auto lightOffset = writer.declLocale( "lightOffset"
						, lights.getPointsEnd() + lightIndex * SpotLight::LightDataComponents );
					auto spot = writer.declLocale( "spot"
						, lights.getSpotLight( lightOffset ) );

					if ( config.useSpotTightBoundingBox )
					{
						auto vsApex = writer.declLocale( "vsApex"
							, c3d_cameraDataClusters.worldToCurView( vec4( spot.position(), 1.0_f ) ).xyz() );
						auto vsDirection = writer.declLocale( "vsDirection"
							, c3d_cameraDataClusters.worldToCurView( -spot.direction() ) );

						auto largeRange = writer.declLocale( "largeRange"
							, computeRange( spot ) );
						auto smallRange = writer.declLocale( "smallRange"
							, largeRange * spot.outerCutOffCos() );
						auto baseRadius = writer.declLocale( "baseRadius"
							, smallRange * spot.outerCutOffTan() );

						auto smallBase = writer.declLocale( "smallBase"
							, vsApex + smallRange * vsDirection );

						IF( writer, dot( vsDirection, vec3( 0.0_f, 0.0_f, -1.0_f ) ) > 0.999_f )
						{
							// Light is looking the same direction as the camera.
							// Weird bug here, resulting in both small and large AABB having min.z == max.z
							// whilst everything looks good when debugging step by step in RenderDoc...
							// Hence just take the disk AABB
							auto e = writer.declLocale( "e"
								, baseRadius * sqrt( vec3( 1.0_f ) - vsDirection * vsDirection ) );

							writer.returnStmt( shader::AABB{ vec4( min( vsApex, smallBase - e ), 1.0_f )
								, vec4( max( vsApex, smallBase + e ), 1.0_f ) } );
						}
						FI

						auto smallAABB = writer.declLocale( "smallAABB"
							, getConeAABB( vsApex, smallBase, baseRadius ) );

						auto largeBase = writer.declLocale( "largeBase"
							, vsApex + largeRange * vsDirection );
						auto largeAABB = writer.declLocale( "largeAABB"
							, getConeAABB( vsApex, largeBase, baseRadius ) );

						writer.returnStmt( shader::AABB{ min( smallAABB.min(), largeAABB.min() )
							, max( smallAABB.max(), largeAABB.max() ) } );
					}
					else
					{
						auto vsPosition = writer.declLocale( "vsPosition"
							, c3d_cameraDataClusters.worldToCurView( vec4( spot.position(), 1.0_f ) ).xyz() );
						writer.returnStmt( shader::AABB{ vsPosition, computeRange( spot ) } );
					}
				}
				, sdw::InUInt{ writer, "lightIndex" } );

			writer.implementMainT< sdw::VoidT >( 1024u, 1u, 1u
				, [&]( sdw::ComputeIn const & in )
				{
					// First compute point lights AABB.
					IF( writer, in.globalInvocationID.x() < c3d_clustersData.pointLightCount() )
					{
						auto aabb = writer.declLocale( "aabb"
							, loadPointLightAABB( in.globalInvocationID.x() ) );
						c3d_allLightsAABB[in.globalInvocationID.x()] = aabb;
					}
					FI

					// Next, compute AABB for spot lights.
					IF( writer, in.globalInvocationID.x() < c3d_clustersData.spotLightCount() )
					{
						auto aabb = writer.declLocale( "aabb"
							, loadSpotLightAABB( in.globalInvocationID.x() ) );
						c3d_allLightsAABB[c3d_clustersData.pointLightCount() + in.globalInvocationID.x()] = aabb;
					}
					FI
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
				, crg::cp::Config config
				, ClustersConfig const & clustersConfig )
				: crg::ComputePass{framePass
					, context
					, graph
					, crg::ru::Config{ 2u }
					, config
						.getPassIndex( RunnablePass::GetPassIndexCallback( [this, &clustersConfig](){ return doGetPassIndex( clustersConfig ); } ) )
						.programCreator( { 2u, [this, &device, &clustersConfig]( uint32_t passIndex ){ return doCreateProgram( device, clustersConfig, passIndex ); } } )
						.end( RecordCallback{ [this]( crg::RecordContext & ctx, VkCommandBuffer cb, uint32_t idx ) { doPostRecord( ctx, cb, idx ); } } ) }
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
			uint32_t doGetPassIndex( ClustersConfig const & clustersConfig )const
			{
				return clustersConfig.useSpotTightBoundingBox ? 1u : 0u;
			}

			crg::VkPipelineShaderStageCreateInfoArray doCreateProgram( RenderDevice const & device
				, ClustersConfig const & clustersConfig
				, uint32_t passIndex )
			{
				auto [it, res] = m_programs.try_emplace( passIndex );

				if ( res )
				{
					auto & program = it->second;
					program.shaderModule = ShaderModule{ VK_SHADER_STAGE_COMPUTE_BIT, cuT( "AssignLightsToClusters" ), createShader( device, clustersConfig ) };
					program.stages = ashes::PipelineShaderStageCreateInfoArray{ makeShaderState( device, program.shaderModule ) };
				}

				return ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( it->second.stages );
			}

			void doPostRecord( crg::RecordContext & context
				, VkCommandBuffer commandBuffer
				, uint32_t index )const
			{
				for ( auto & attach : m_pass.buffers )
				{
					if ( !attach.isNoTransition()
						&& attach.isStorageBuffer()
						&& attach.isClearableBuffer() )
					{
						auto currentState = context.getAccessState( attach.buffer( index )
							, attach.getBufferRange() );
						context.memoryBarrier( commandBuffer
							, attach.buffer( index )
							, attach.getBufferRange()
							, currentState.access
							, currentState.pipelineStage
							, { VK_ACCESS_SHADER_READ_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT } );
					}
				}
			}

		private:
			castor::Map< uint32_t, ProgramData > m_programs;
		};
	}

	namespace dsplgtb
	{
		enum BindingPoints
		{
			eMainCamera,
			eClustersCamera,
			eLightsAABB,
		};

		static ShaderPtr createDebugDisplayShader( RenderDevice const & device
			, FrustumClusters const & frustumClusters )
		{
			sdw::TraditionalGraphicsWriter writer{ &device.renderSystem.getEngine()->getShaderAllocator() };

			C3D_CameraNamed( writer
				, Main
				, eMainCamera
				, 0u );
			C3D_CameraNamed( writer
				, Clusters
				, eClustersCamera
				, 0u );
			C3D_AllLightsAABB( writer
				, eLightsAABB
				, 0u );

			auto colorPalette = writer.declConstantArray( "colorPalette"
				, std::vector< sdw::Vec4 >{ vec4( 0.25_f, 0.25_f, 0.25_f, 1.0_f )
					, vec4( 0.25_f, 0.25_f, 1.00_f, 1.0_f )
					, vec4( 0.25_f, 1.00_f, 0.25_f, 1.0_f )
					, vec4( 0.25_f, 1.00_f, 1.00_f, 1.0_f )
					, vec4( 1.00_f, 0.25_f, 0.25_f, 1.0_f )
					, vec4( 1.00_f, 0.25_f, 1.00_f, 1.0_f )
					, vec4( 1.00_f, 1.00_f, 0.25_f, 1.0_f )
					, vec4( 1.00_f, 1.00_f, 1.00_f, 1.0_f ) } );

			writer.implementEntryPointT< shader::Position4FT, shader::Colour4FT >( [&writer, &c3d_cameraDataMain, &c3d_cameraDataClusters, &c3d_allLightsAABB, &colorPalette]( sdw::VertexInT< shader::Position4FT > const & in
				, sdw::VertexOutT< shader::Colour4FT > out )
				{
					auto aabb = writer.declLocale( "aabb"
						, c3d_allLightsAABB[in.instanceIndex] );
					auto position = writer.declLocale( "position"
						, in.position() );
					position.x() = mix( aabb.min().x(), aabb.max().x(), position.x() );
					position.y() = mix( aabb.min().y(), aabb.max().y(), position.y() );
					position.z() = mix( aabb.min().z(), aabb.max().z(), position.z() );
					// Convert from clusters view position to world position
					position = c3d_cameraDataClusters.curViewToWorld( position );
					position.w() = 1.0_f;
					// Then from world to main camera proj.
					out.vtx.position = c3d_cameraDataMain.worldToCurProj( position );

					out.colour() = colorPalette[in.instanceIndex % 8];
				} );

			writer.implementEntryPointT< shader::Colour4FT, shader::Colour4FT >( []( sdw::FragmentInT< shader::Colour4FT > const & in
				, sdw::FragmentOutT< shader::Colour4FT > const & out )
				{
					out.colour() = in.colour();
				} );

			return writer.getBuilder().releaseShader();
		}
	}

	//*********************************************************************************************

	crg::FramePass const & createComputeLightsAABBPass( crg::FramePassGroup & graph
		, crg::FramePass const * previousPass
		, RenderDevice const & device
		, CameraUbo const & mainCameraUbo
		, CameraUbo const & clustersCameraUbo
		, FrustumClusters const & clusters )
	{
		auto & pass = graph.createPass( "ComputeLightsAABB"
			, [&clusters, &device]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				auto result = castor::make_unique< cptlgtb::FramePass >( framePass
					, context
					, graph
					, device
					, crg::cp::Config{}
						.groupCountX( MaxLightsCount / 1024u )
						.enabled( &clusters.needsClustersUpdate() )
					, clusters.getConfig() );
				device.renderSystem.getEngine()->registerTimer( castor::makeString( framePass.getFullName() )
					, result->getTimer() );
				return result;
			} );
		pass.addDependency( *previousPass );
		mainCameraUbo.createPassBinding( pass, cptlgtb::eMainCamera );
		clustersCameraUbo.createPassBinding( pass, cptlgtb::eClustersCamera );
		clusters.getClustersUbo().createPassBinding( pass, cptlgtb::eClusters );
		auto const & lights = clusters.getCamera().getScene()->getLightCache();
		lights.createPassBinding( pass, cptlgtb::eLights );
		createClearableOutputStorageBinding( pass, uint32_t( cptlgtb::eAllLightsAABB ), cuT( "C3D_AllLightsAABB" ), clusters.getAllLightsAABBBuffer(), 0u, ashes::WholeSize );
		return pass;
	}

	void createDisplayLightsAABBProgram( RenderDevice const & device
		, FrustumClusters const & clusters
		, CameraUbo const & mainCameraUbo
		, CameraUbo const & clustersCameraUbo
		, ashes::PipelineShaderStageCreateInfoArray & program
		, ashes::VkDescriptorSetLayoutBindingArray & bindings
		, ashes::WriteDescriptorSetArray & writes )
	{
		ProgramModule programModule{ "LightsAABB", dsplgtb::createDebugDisplayShader( device, clusters ) };
		program = makeProgramStates( device, programModule );

		bindings.push_back( VkDescriptorSetLayoutBinding{ dsplgtb::eMainCamera, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1u, VK_SHADER_STAGE_VERTEX_BIT, nullptr } );
		bindings.push_back( VkDescriptorSetLayoutBinding{ dsplgtb::eClustersCamera, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1u, VK_SHADER_STAGE_VERTEX_BIT, nullptr } );
		bindings.push_back( VkDescriptorSetLayoutBinding{ dsplgtb::eLightsAABB, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1u, VK_SHADER_STAGE_VERTEX_BIT, nullptr } );

		writes.emplace_back( dsplgtb::eMainCamera, 0u, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
			, ashes::VkDescriptorBufferInfoArray{ VkDescriptorBufferInfo{ mainCameraUbo.getUbo().getBuffer().getBuffer(), mainCameraUbo.getUbo().getByteOffset(), mainCameraUbo.getUbo().getByteRange() } } );
		writes.emplace_back( dsplgtb::eClustersCamera, 0u, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
			, ashes::VkDescriptorBufferInfoArray{ VkDescriptorBufferInfo{ clustersCameraUbo.getUbo().getBuffer().getBuffer(), clustersCameraUbo.getUbo().getByteOffset(), clustersCameraUbo.getUbo().getByteRange() } } );
		auto & aabbBuffer = clusters.getAllLightsAABBBuffer();
		writes.emplace_back( dsplgtb::eLightsAABB, 0u, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
			, ashes::VkDescriptorBufferInfoArray{ VkDescriptorBufferInfo{ aabbBuffer, 0u, aabbBuffer.getSize() } } );
	}

	//*********************************************************************************************
}
