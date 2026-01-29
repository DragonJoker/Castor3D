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

#include <ShaderWriter/Source.hpp>
#include <ShaderWriter/TraditionalGraphicsWriter.hpp>

#include <RenderGraph/FramePassGroup.hpp>
#include <RenderGraph/RunnablePasses/ComputePass.hpp>

namespace c3d
{
	//*********************************************************************************************

	namespace cptlgtb
	{
		enum class Bindings
		{
			eCamera,
			eClusters,
			eLights,
			eAllLightsAABB,
		};

		static ShaderPtr createShader( RenderDevice const & device )
		{
			static float constexpr FltMax = std::numeric_limits< float >::max();

			sdw::ComputeWriter writer{ &c3d::getEngine( device ).getShaderAllocator() };

			// Inputs
			C3D_Camera( writer
				, Bindings::eCamera
				, 0u );
			C3D_Clusters( writer
				, Bindings::eClusters
				, 0u );
			shader::LightsBuffer lights{ writer
				, uint32_t( Bindings::eLights )
				, 0u };
			C3D_AllLightsAABB( writer
				, Bindings::eAllLightsAABB
				, 0u );

			auto loadPointLightAABB = writer.implementFunction< shader::AABB >( "loadPointLightAABB"
				, [&writer, &lights, c3d_cameraData]( sdw::UInt const & lightIndex )
				{
					auto lightOffset = writer.declLocale( "lightOffset"
						, lights.getDirectionalsEnd() + lightIndex * PointLightInstance::LightDataComponents );
					auto point = writer.declLocale( "point"
						, lights.getPointLight( lightOffset ) );
					auto result = writer.declLocale< shader::AABB >( "result" );

					sdwIF( writer, point.enabled() )
					{
						auto vsPosition = writer.declLocale( "vsPosition"
							, c3d_cameraData.worldToCurView( vec4( point.position(), 1.0_f ) ).xyz() );

						result = shader::AABB{ vsPosition, computeRange( point ) };
					}
					sdwELSE
					{
						result = shader::AABB{ vec4( sdw::Float{ FltMax }, FltMax, FltMax, 1.0f )
							, vec4( sdw::Float{ -FltMax }, -FltMax, -FltMax, 1.0f ) };
					}
					sdwFI

					writer.returnStmt( result );
				}
				, sdw::InUInt{ writer, "lightIndex" } );

			auto getConeAABB = writer.implementFunction< shader::AABB >( "getConeAABB"
				, [&writer]( sdw::Vec3 const & vsApex
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
				, [&writer, &lights, &c3d_cameraData, &getConeAABB]( sdw::UInt const & lightIndex )
				{
					auto lightOffset = writer.declLocale( "lightOffset"
						, lights.getPointsEnd() + lightIndex * SpotLightInstance::LightDataComponents );
					auto spot = writer.declLocale( "spot"
						, lights.getSpotLight( lightOffset ) );
					auto result = writer.declLocale< shader::AABB >( "result" );

					sdwIF( writer, spot.enabled() )
					{
						auto vsApex = writer.declLocale( "vsApex"
							, c3d_cameraData.worldToCurView( vec4( spot.position(), 1.0_f ) ).xyz() );
						auto vsDirection = writer.declLocale( "vsDirection"
							, c3d_cameraData.worldToCurView( -spot.direction() ) );

						auto largeRange = writer.declLocale( "largeRange"
							, computeRange( spot ) );
						auto smallRange = writer.declLocale( "smallRange"
							, largeRange * spot.outerCutOffCos() );
						auto baseRadius = writer.declLocale( "baseRadius"
							, smallRange * spot.outerCutOffTan() );

						auto smallBase = writer.declLocale( "smallBase"
							, vsApex + smallRange * vsDirection );

						sdwIF( writer, dot( vsDirection, vec3( 0.0_f, 0.0_f, -1.0_f ) ) > 0.999_f )
						{
							// Light is looking the same direction as the camera.
							// Weird bug here, resulting in both small and large AABB having min.z == max.z
							// whilst everything looks good when debugging step by step in RenderDoc...
							// Hence just take the disk AABB
							auto e = writer.declLocale( "e"
								, baseRadius * sqrt( vec3( 1.0_f ) - vsDirection * vsDirection ) );

							result = shader::AABB{ vec4( min( vsApex, smallBase - e ), 1.0_f )
								, vec4( max( vsApex, smallBase + e ), 1.0_f ) };
						}
						sdwELSE
						{
							auto smallAABB = writer.declLocale( "smallAABB"
								, getConeAABB( vsApex, smallBase, baseRadius ) );

							auto largeBase = writer.declLocale( "largeBase"
								, vsApex + largeRange * vsDirection );
							auto largeAABB = writer.declLocale( "largeAABB"
								, getConeAABB( vsApex, largeBase, baseRadius ) );

							result = shader::AABB{ min( smallAABB.min(), largeAABB.min() )
								, max( smallAABB.max(), largeAABB.max() ) };
						}
						sdwFI
					}
					sdwELSE
					{
						result = shader::AABB{ vec4( sdw::Float{ FltMax }, FltMax, FltMax, 1.0f )
							, vec4( sdw::Float{ -FltMax }, -FltMax, -FltMax, 1.0f ) };
					}
					sdwFI

					writer.returnStmt( result );
				}
				, sdw::InUInt{ writer, "lightIndex" } );

			writer.implementMainT< sdw::VoidT >( 1024u, 1u, 1u
				, [&writer, &loadPointLightAABB, &c3d_allLightsAABB, &loadSpotLightAABB, &c3d_clustersData]( sdw::ComputeIn const & in )
				{
					// First compute point lights AABB.
					sdwIF( writer, in.globalInvocationID.x() < c3d_clustersData.pointLightCount() )
					{
						auto aabb = writer.declLocale( "aabb"
							, loadPointLightAABB( in.globalInvocationID.x() ) );
						c3d_allLightsAABB[in.globalInvocationID.x()] = aabb;
					}
					sdwFI

					// Next, compute AABB for spot lights.
					sdwIF( writer, in.globalInvocationID.x() < c3d_clustersData.spotLightCount() )
					{
						auto aabb = writer.declLocale( "aabb"
							, loadSpotLightAABB( in.globalInvocationID.x() ) );
						c3d_allLightsAABB[c3d_clustersData.pointLightCount() + in.globalInvocationID.x()] = aabb;
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
				, crg::cp::Config config )
				: crg::ComputePass{framePass
					, context
					, graph
					, crg::ru::Config{ 1u }
					, config
						.programCreator( { 1u, [this, &device]( uint32_t passIndex ){ return doCreateProgram( device, passIndex ); } } ) }
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
			crg::VkPipelineShaderStageCreateInfoArray doCreateProgram( RenderDevice const & device
				, uint32_t passIndex )
			{
				auto [it, res] = m_programs.try_emplace( passIndex );

				if ( res )
				{
					auto & program = it->second;
					program.shaderModule = ShaderModule{ VK_SHADER_STAGE_COMPUTE_BIT, cuT( "AssignLightsToClusters" ), createShader( device ) };
					program.stages = ashes::PipelineShaderStageCreateInfoArray{ makeShaderState( device, program.shaderModule ) };
				}

				return ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( it->second.stages );
			}

		private:
			Map< uint32_t, ProgramData > m_programs;
		};
	}

	namespace dsplgtb
	{
		enum class Bindings
		{
			eMainCamera,
			eClustersCamera,
			eLightsAABB,
		};

		static ShaderPtr createDebugDisplayShader( RenderDevice const & device )
		{
			sdw::TraditionalGraphicsWriter writer{ &c3d::getEngine( device ).getShaderAllocator() };

			C3D_CameraNamed( writer
				, Main
				, Bindings::eMainCamera
				, 0u );
			C3D_CameraNamed( writer
				, Clusters
				, Bindings::eClustersCamera
				, 0u );
			C3D_AllLightsAABB( writer
				, Bindings::eLightsAABB
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

			writer.implementEntryPointT< shader::Position4FT, shader::Colour4FT >( [&writer, &c3d_cameraDataMain, &c3d_cameraDataClusters, &c3d_allLightsAABB
				, &colorPalette]( sdw::VertexInT< shader::Position4FT > const & in
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

	void createComputeLightsAABBPass( crg::FramePassGroup & graph
		, RenderDevice const & device
		, FrustumClusters const & clusters
		, CameraUbo const & clustersCameraUbo
		, BufferBase & allLightsAABBB )
	{
		auto & pass = graph.createPass( "ComputeLightsAABB"
			, [&clusters, &device]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runGraph )
			{
				auto result = makeRawUnique< cptlgtb::FramePass >( framePass, context, runGraph, device
					, crg::cp::Config{}
						.groupCountX( MaxLightsCount / 1024u )
						.enabled( &clusters.needsClustersUpdate() ) );
				c3d::getEngine( device ).registerTimer( makeString( framePass.getFullName() )
					, result->getTimer() );
				return result;
			} );
		clustersCameraUbo.createPassBinding( pass, cptlgtb::Bindings::eCamera );
		clusters.getClustersUbo().createPassBinding( pass, cptlgtb::Bindings::eClusters );
		auto const & lights = clusters.getCamera().getScene()->getLightCache();
		lights.createPassBindingT( pass, cptlgtb::Bindings::eLights );
		allLightsAABBB.setLastAttach( pass.addClearableOutputStorageBuffer( allLightsAABBB.bufferViewId, uint32_t( cptlgtb::Bindings::eAllLightsAABB ) ) );
	}

	void createDisplayLightsAABBProgram( RenderDevice const & device
		, CameraUbo const & mainCameraUbo
		, CameraUbo const & clustersCameraUbo
		, ashes::PipelineShaderStageCreateInfoArray & program
		, ashes::VkDescriptorSetLayoutBindingArray & bindings
		, ashes::WriteDescriptorSetArray & writes
		, BufferBase const & allLightsAABBB )
	{
		ProgramModule programModule{ "LightsAABB", dsplgtb::createDebugDisplayShader( device ) };
		program = makeProgramStates( device, programModule );

		c3d::addDescriptorSetLayoutBindingT( bindings, dsplgtb::Bindings::eMainCamera, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT );
		c3d::addDescriptorSetLayoutBindingT( bindings, dsplgtb::Bindings::eClustersCamera, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT );
		c3d::addDescriptorSetLayoutBindingT( bindings, dsplgtb::Bindings::eLightsAABB, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_VERTEX_BIT );

		writes.emplace_back( uint32_t( dsplgtb::Bindings::eMainCamera ), 0u, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
			, ashes::VkDescriptorBufferInfoArray{ VkDescriptorBufferInfo{ mainCameraUbo.getUbo().getBuffer().getBuffer(), mainCameraUbo.getUbo().getByteOffset(), mainCameraUbo.getUbo().getByteRange() } } );
		writes.emplace_back( uint32_t( dsplgtb::Bindings::eClustersCamera ), 0u, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
			, ashes::VkDescriptorBufferInfoArray{ VkDescriptorBufferInfo{ clustersCameraUbo.getUbo().getBuffer().getBuffer(), clustersCameraUbo.getUbo().getByteOffset(), clustersCameraUbo.getUbo().getByteRange() } } );
		writes.emplace_back( uint32_t( dsplgtb::Bindings::eLightsAABB ), 0u, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
			, ashes::VkDescriptorBufferInfoArray{ VkDescriptorBufferInfo{ allLightsAABBB.getBuffer(), 0u, allLightsAABBB.getSize() } } );
	}

	//*********************************************************************************************
}
