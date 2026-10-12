#include "Castor3D/Render/Clustered/ComputeLightsAABB.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/Clustered/FrustumClusters.hpp"
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
			shader::AllLightsAABB allLightsAABB{ writer
				, Bindings::eAllLightsAABB
				, 0u };

			auto loadPointLightAABB = writer.implementFunction< shader::AABB >( "loadPointLightAABB"
				, [&writer, &lights, c3d_cameraData]( sdw::UInt const & lightIndex )
				{
					auto colourRange = writer.declLocale< sdw::Vec4 >( "colourRange", vec4( 0.0_f ) );
					auto intensityShadowCascade = writer.declLocale< sdw::Vec4 >( "intensityShadowCascade", vec4( 0.0_f ) );
					auto posEnabled = writer.declLocale< sdw::Vec4 >( "posEnabled", vec4( 0.0_f ) );
					auto lightOffset = writer.declLocale( "lightOffset"
						, lights.getDirectionalsEnd() + lightIndex * PointLightInstance::LightDataComponents );
					lights.getPointLightData( lightOffset, colourRange, intensityShadowCascade, posEnabled );
					auto result = writer.declLocale< shader::AABB >( "result" );

					// TODO: Remove the if by sending only enabled lights to the shader.
					sdwIF( writer, posEnabled.w() != 0.0_f )
					{
						auto vsPosition = writer.declLocale( "vsPosition"
							, c3d_cameraData.worldToCurView( vec4( posEnabled.xyz(), 1.0_f)).xyz());
						auto range = writer.declLocale( "range"
							, shader::computeRange( colourRange.w(), intensityShadowCascade.x() ) );
						result = shader::AABB{ vsPosition, range };
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

			auto generateOrthonormalBasis = writer.implementFunction< sdw::Void >( "generateOrthonormalBasis"
				, [&writer]( sdw::Vec3 const & n
					, sdw::Vec3 b1
					, sdw::Vec3 b2 )
				{
					// https://graphics.pixar.com/library/OrthonormalB/paper.pdf
					auto sign = writer.declLocale( "sign", writer.ternary( n.z() > 0.0_f, 1.0_f, -1.0_f ) );
					auto a = writer.declLocale( "a", -1.0_f / ( sign + n.z() ) );
					auto b = writer.declLocale( "b", n.x() * n.y() * a );
					b1 = sdw::vec3( 1.0_f + sign * n.x() * n.x() * a, sign * b, -sign * n.x() );
					b2 = sdw::vec3( b, sign + n.y() * n.y() * a, -n.y() );
				}
				, sdw::InVec3{ writer, "n" }
				, sdw::OutVec3{ writer, "b1" }
				, sdw::OutVec3{ writer, "b2" } );

			auto getConeAABB = writer.implementFunction< shader::AABB >( "getConeAABB"
				, [&writer, &generateOrthonormalBasis]( sdw::Vec3 const & vsApex
					, sdw::Vec3 const & vsDirection
					, sdw::Float const & range
					, sdw::Float const & tanHalfAngle )
				{
					//
					auto up = writer.declLocale< sdw::Vec3 >( "up" );
					auto right = writer.declLocale< sdw::Vec3 >( "right" );
					generateOrthonormalBasis( vsDirection, up, right );

					// r = range

					// a = d * tanHalfAngle 
					// => d² = a²/tanHalfAngle²

					// r² = a² + d²
					//    = a² + a²/tanHalfAngle²
					//    = a² * (1 + 1/tanHalfAngle²)
					// a² = r² / (1 + 1/tanHalfAngle²)
					// a  = sqrt(r² / (1 + 1/tanHalfAngle²))
					//    = r * 1 / sqrt(1 + 1/tanHalfAngle²)
					//    = r * inverseSqrt(1 + 1/tanHalfAngle²)

					auto invSqrt = writer.declLocale( "invSqrt", inverseSqrt( ( 1.0_f + 1.0_f / ( tanHalfAngle * tanHalfAngle ) ) ) );
					auto a = writer.declLocale( "a", range * sdw::vec3( invSqrt ) );
					auto d = writer.declLocale( "d", a / tanHalfAngle );
					auto e = writer.declLocale( "e", range - d );

					//
					auto z = writer.declLocale( "z", vsApex + vsDirection * d );
					auto dirE = writer.declLocale( "dirE", vsDirection * e );

					//
					auto bottomA = writer.declLocale( "bottomA", z + a * up + a * right );
					auto bottomB = writer.declLocale( "bottomB", z + a * up - a * right );
					auto bottomC = writer.declLocale( "bottomC", z - a * up + a * right );
					auto bottomD = writer.declLocale( "bottomD", z - a * up - a * right );

					auto topA = writer.declLocale( "topA", bottomA + dirE );
					auto topB = writer.declLocale( "topB", bottomB + dirE );
					auto topC = writer.declLocale( "topC", bottomC + dirE );
					auto topD = writer.declLocale( "topD", bottomD + dirE );

					// initialize AABB with apex position
					auto vMin = writer.declLocale( "vMin", vsApex );
					auto vMax = writer.declLocale( "vMax", vsApex );

					// add 2D quad corners
					vMin = min( vMin, bottomA );
					vMin = min( vMin, bottomB );
					vMin = min( vMin, bottomC );
					vMin = min( vMin, bottomD );
					vMin = min( vMin, topA );
					vMin = min( vMin, topB );
					vMin = min( vMin, topC );
					vMin = min( vMin, topD );

					vMax = max( vMax, bottomA );
					vMax = max( vMax, bottomB );
					vMax = max( vMax, bottomC );
					vMax = max( vMax, bottomD );
					vMax = max( vMax, topA );
					vMax = max( vMax, topB );
					vMax = max( vMax, topC );
					vMax = max( vMax, topD );

					//
					writer.returnStmt( shader::AABB{ sdw::vec4( vMin, 1.0_f ), sdw::vec4( vMax, 1.0_f ) } );
				}
				, sdw::InVec3{ writer, "vsApex" }
				, sdw::InVec3{ writer, "vsDirection" }
				, sdw::InFloat{ writer, "range" }
				, sdw::InFloat{ writer, "tanHalfAngle" } );

			auto loadSpotLightAABB = writer.implementFunction< shader::AABB >( "loadSpotLightAABB"
				, [&writer, &lights, &c3d_cameraData, &getConeAABB]( sdw::UInt const & lightIndex )
				{
					auto colourRange = writer.declLocale< sdw::Vec4 >( "colourRange", vec4( 0.0_f ) );
					auto intensityShadowCascade = writer.declLocale< sdw::Vec4 >( "intensityShadowCascade", vec4( 0.0_f ) );
					auto posEnabled = writer.declLocale< sdw::Vec4 >( "posEnabled", vec4( 0.0_f ) );
					auto directionExponent = writer.declLocale< sdw::Vec4 >( "directionExponent", vec4( 0.0_f ) );
					auto cutOffData1 = writer.declLocale< sdw::Vec4 >( "cutOffData1", vec4( 0.0_f ) );
					auto cutOffData2 = writer.declLocale< sdw::Vec4 >( "cutOffData2", vec4( 0.0_f ) );
					auto lightOffset = writer.declLocale( "lightOffset"
						, lights.getPointsEnd() + lightIndex * SpotLightInstance::LightDataComponents );
					lights.getSpotLightData( lightOffset, colourRange, intensityShadowCascade, posEnabled
						, directionExponent, cutOffData1, cutOffData2 );
					auto result = writer.declLocale< shader::AABB >( "result" );

					// TODO: Remove the if by sending only enabled lights to the shader.
					sdwIF( writer, posEnabled.w() != 0.0_f )
					{
						auto vsApex = writer.declLocale( "vsApex"
							, c3d_cameraData.worldToCurView( vec4( posEnabled.xyz(), 1.0_f ) ).xyz() );
						auto vsDirection = writer.declLocale( "vsDirection"
							, c3d_cameraData.worldToCurView( -directionExponent.xyz() ) );

						auto range = writer.declLocale( "range"
							, shader::computeRange( colourRange.w(), intensityShadowCascade.x() ) );
						auto tanHalfAngle = writer.declLocale( "tanHalfAngle"
							, cutOffData2.z() );

						result = getConeAABB( vsApex, vsDirection, range, tanHalfAngle );
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
				, [&writer, &loadPointLightAABB, &allLightsAABB, &loadSpotLightAABB, &c3d_clustersData]( sdw::ComputeIn const & in )
				{
					// First compute point lights AABB.
					sdwIF( writer, in.globalInvocationID.x() < c3d_clustersData.pointLightCount() )
					{
						auto aabb = writer.declLocale( "aabb"
							, loadPointLightAABB( in.globalInvocationID.x() ) );
						allLightsAABB[in.globalInvocationID.x()] = aabb;
					}
					sdwFI

					// Next, compute AABB for spot lights.
					sdwIF( writer, in.globalInvocationID.x() < c3d_clustersData.spotLightCount() )
					{
						auto aabb = writer.declLocale( "aabb"
							, loadSpotLightAABB( in.globalInvocationID.x() ) );
						allLightsAABB[c3d_clustersData.pointLightCount() + in.globalInvocationID.x()] = aabb;
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
				: crg::ComputePass{ framePass
					, context
					, graph
					, crg::ru::Config{ 1u }
					, config
						.getGroupCountX( crg::cp::GetGroupCountCallback( [this](){ return doGetGroupsCountX(); } ) )
						.programCreator( { 1u, [this, &device]( uint32_t passIndex ){ return doCreateProgram( device, passIndex ); } } ) }
				, m_clusters{ clusters }
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
					program.shaderModule = ShaderModule{ VK_SHADER_STAGE_COMPUTE_BIT, cuT( "ComputeLightsAABB" ), createShader( device ) };
					program.stages = ashes::PipelineShaderStageCreateInfoArray{ makeShaderState( device, program.shaderModule ) };
				}

				return ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( it->second.stages );
			}

			u32 doGetGroupsCountX()const
			{
				auto const & lightCache = m_clusters.getScene().getLightCache();
				auto pointLightsCount = lightCache.getLightsBufferCount( LightType::ePoint );
				auto spoLightsCount = lightCache.getLightsBufferCount( LightType::eSpot );
				auto totalValues = std::max( pointLightsCount, spoLightsCount );
				return divRoundUp( totalValues, 1024u );
			}

		private:
			FrustumClusters const & m_clusters;
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
			shader::AllLightsAABB allLightsAABB{ writer
				, Bindings::eLightsAABB
				, 0u };

			auto colorPalette = writer.declConstantArray( "colorPalette"
				, std::vector< sdw::Vec4 >{ vec4( 0.25_f, 0.25_f, 0.25_f, 1.0_f )
					, vec4( 0.25_f, 0.25_f, 1.00_f, 1.0_f )
					, vec4( 0.25_f, 1.00_f, 0.25_f, 1.0_f )
					, vec4( 0.25_f, 1.00_f, 1.00_f, 1.0_f )
					, vec4( 1.00_f, 0.25_f, 0.25_f, 1.0_f )
					, vec4( 1.00_f, 0.25_f, 1.00_f, 1.0_f )
					, vec4( 1.00_f, 1.00_f, 0.25_f, 1.0_f )
					, vec4( 1.00_f, 1.00_f, 1.00_f, 1.0_f ) } );

			writer.implementEntryPointT< shader::Position4FT, shader::Colour4FT >( [&writer, &c3d_cameraDataMain, &c3d_cameraDataClusters, &allLightsAABB
				, &colorPalette]( sdw::VertexInT< shader::Position4FT > const & in
					, sdw::VertexOutT< shader::Colour4FT > out )
				{
					auto aabb = writer.declLocale( "aabb"
						, allLightsAABB[writer.cast< sdw::UInt >( in.instanceIndex )] );
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
		, PagedBufferBase & allLightsAABBB )
	{
		auto & pass = graph.createPass( "ComputeLightsAABB"
			, [&clusters, &device]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runGraph )
			{
				auto result = makeRawUnique< cptlgtb::FramePass >( framePass, context, runGraph
					, device, clusters
					, crg::cp::Config{}
						.enabled( &clusters.needsClustersUpdate() ) );
				c3d::getEngine( device ).registerTimer( makeString( framePass.getFullName() )
					, result->getTimer() );
				return result;
			} );
		clustersCameraUbo.createPassBinding( pass, cptlgtb::Bindings::eCamera );
		clusters.getClustersUbo().createPassBinding( pass, cptlgtb::Bindings::eClusters );
		auto const & lights = clusters.getScene().getLightCache();
		lights.createPassBindingT( pass, cptlgtb::Bindings::eLights );
		allLightsAABBB.setLastAttach( pass.addClearableOutputStorageBuffer( allLightsAABBB.bufferViewId, uint32_t( cptlgtb::Bindings::eAllLightsAABB ) ) );
	}

	void createDisplayLightsAABBProgram( RenderDevice const & device
		, CameraUbo const & mainCameraUbo
		, CameraUbo const & clustersCameraUbo
		, ashes::PipelineShaderStageCreateInfoArray & program
		, ashes::VkDescriptorSetLayoutBindingArray & bindings
		, ashes::WriteDescriptorSetArray & writes
		, PagedBufferBase const & allLightsAABBB )
	{
		ProgramModule programModule{ "LightsAABB", dsplgtb::createDebugDisplayShader( device ) };
		program = makeProgramStates( device, programModule );

		mainCameraUbo.addLayoutBindingT( bindings, dsplgtb::Bindings::eMainCamera, VK_SHADER_STAGE_VERTEX_BIT );
		clustersCameraUbo.addLayoutBindingT( bindings, dsplgtb::Bindings::eClustersCamera, VK_SHADER_STAGE_VERTEX_BIT );
		allLightsAABBB.addLayoutBindingT( bindings, dsplgtb::Bindings::eLightsAABB, VK_SHADER_STAGE_VERTEX_BIT );

		mainCameraUbo.addDescriptorWriteT( writes, dsplgtb::Bindings::eMainCamera );
		clustersCameraUbo.addDescriptorWriteT( writes, dsplgtb::Bindings::eClustersCamera );
		allLightsAABBB.addDescriptorWriteT( writes, dsplgtb::Bindings::eLightsAABB );
	}

	//*********************************************************************************************
}
