#include "Castor3D/Render/Volumetric/IntegrateFroxels.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/LightCache.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/Volumetric/FrustumFroxels.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/Light/PointLight.hpp"
#include "Castor3D/Scene/Light/SpotLight.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/Shaders/GlslBaseIO.hpp"
#include "Castor3D/Shader/Shaders/GlslLight.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Ubos/CameraUbo.hpp"
#include "Castor3D/Shader/Ubos/ClustersUbo.hpp"
#include "Castor3D/Shader/Ubos/FroxelsUbo.hpp"
#include "Castor3D/Shader/Ubos/RenderUbo.hpp"

#include <CastorUtils/Design/DataHolder.hpp>

#include <ShaderWriter/ComputeWriter.hpp>
#include <ShaderWriter/TraditionalGraphicsWriter.hpp>

#include <RenderGraph/FramePassGroup.hpp>
#include <RenderGraph/RunnablePasses/ComputePass.hpp>

namespace c3d
{
	//*********************************************************************************************

	namespace intfxl
	{
		enum class Bindings
		{
			eLights,
			eCamera,
			eFroxels,
			eClusters,
			eCounters,
			eFroxelsLights,
			eFroxelsLightingR,
			eFroxelsLightingG,
			eFroxelsLightingB,
		};

		static sdw::RetVec3 computeLighting( sdw::ComputeWriter &
			, shader::PointLight const & light
			, sdw::Vec3 const & wsTapPosition )
		{
			return light.colour() * light.intensity() * light.getAttenuationFactor( distance( light.position(), wsTapPosition ) );
		}

		static sdw::RetVec3 computeLighting( sdw::ComputeWriter & writer
			, shader::SpotLight const & light
			, sdw::Vec3 const & wsTapPosition )
		{
			auto distance = writer.declLocale( "vertexToLight", sdw::distance( light.position(), wsTapPosition ) );
			auto spotFactor = writer.declLocale( "spotFactor"
				, dot( ( light.position() - wsTapPosition ) / distance, light.direction() ) );
			spotFactor = clamp( ( spotFactor - light.outerCutOffCos() ) / light.cutOffsCosDiff(), 0.0_f, 1.0_f );
			return light.colour() * light.intensity() * spotFactor * light.getAttenuationFactor( distance );
		}

		static ShaderPtr createShader( RenderDevice const & device
			, bool pointLightsPass )
		{
			sdw::ComputeWriter writer{ &c3d::getEngine( device ).getShaderAllocator() };
			shader::Utils utils{ writer };

			shader::LightsBuffer lights{ writer
				, uint32_t( Bindings::eLights )
				, 0u };
			C3D_Camera( writer
				, Bindings::eCamera
				, 0u );
			C3D_Froxels( writer
				, Bindings::eFroxels
				, 0u );
			C3D_Clusters( writer
				, Bindings::eClusters
				, 0u );

			auto countersBuffer = writer.declStorageBuffer( "c3d_countersBuffer", uint32_t( Bindings::eCounters ), 0u );
			auto c3d_counters = countersBuffer.declMemberArray< sdw::UInt >( "c" );
			countersBuffer.end();

			auto froxelsLightsBuffer = writer.declStorageBuffer( "c3d_froxelsLightsBuffer", uint32_t( Bindings::eFroxelsLights ), 0u );
			auto c3d_froxelsLights = froxelsLightsBuffer.declMemberArray< sdw::U32Vec2 >( "fl" );
			froxelsLightsBuffer.end();

			auto c3d_froxelsLightingR = writer.declStorageImg< RWUImg3DR32 >( "c3d_froxelsLightingR", Bindings::eFroxelsLightingR, 0u );
			auto c3d_froxelsLightingG = writer.declStorageImg< RWUImg3DR32 >( "c3d_froxelsLightingG", Bindings::eFroxelsLightingG, 0u );
			auto c3d_froxelsLightingB = writer.declStorageImg< RWUImg3DR32 >( "c3d_froxelsLightingB", Bindings::eFroxelsLightingB, 0u );

			auto processFroxelLight = [&c3d_froxelsData, &c3d_cameraData, &c3d_froxelsLightingR, &c3d_froxelsLightingG, &c3d_froxelsLightingB
					, &writer, &utils]( auto const & light, sdw::U32Vec3 const & froxelIndex3D )
				{
					auto depthBounds = writer.declLocale( "depthBounds", c3d_froxelsData.getDepthBounds( froxelIndex3D.z() ) );
					auto minScreenPosition = writer.declLocale( "minScreenPosition"
						, sdw::vec3( sdw::vec2( froxelIndex3D.xy() ) * c3d_froxelsData.froxelSize(), c3d_froxelsData.getScreenSpaceDepth( depthBounds.x() ) ) );
					auto maxScreenPosition = writer.declLocale( "maxScreenPosition"
						, sdw::vec3( sdw::vec2( froxelIndex3D.xy() + 1u ) * c3d_froxelsData.froxelSize(), c3d_froxelsData.getScreenSpaceDepth( depthBounds.y() ) ) );

					auto vsNearTopLeft = writer.declLocale( "vsNearTopLeft", c3d_froxelsData.getViewSpacePosition( utils, c3d_cameraData.getInvProjMtx(), minScreenPosition.x(), minScreenPosition.y(), minScreenPosition.z() ) );
					auto vsNearTopRight = writer.declLocale( "vsNearTopRight", c3d_froxelsData.getViewSpacePosition( utils, c3d_cameraData.getInvProjMtx(), maxScreenPosition.x(), minScreenPosition.y(), minScreenPosition.z() ) );
					auto vsFarTopLeft = writer.declLocale( "vsFarTopLeft", c3d_froxelsData.getViewSpacePosition( utils, c3d_cameraData.getInvProjMtx(), minScreenPosition.x(), minScreenPosition.y(), maxScreenPosition.z() ) );
					auto vsFarTopRight = writer.declLocale( "vsFarTopRight", c3d_froxelsData.getViewSpacePosition( utils, c3d_cameraData.getInvProjMtx(), maxScreenPosition.x(), minScreenPosition.y(), maxScreenPosition.z() ) );
					auto vsNearBottomLeft = writer.declLocale( "vsNearBottomLeft", c3d_froxelsData.getViewSpacePosition( utils, c3d_cameraData.getInvProjMtx(), minScreenPosition.x(), maxScreenPosition.y(), minScreenPosition.z() ) );
					auto vsFarBottomLeft = writer.declLocale( "vsFarBottomLeft", c3d_froxelsData.getViewSpacePosition( utils, c3d_cameraData.getInvProjMtx(), minScreenPosition.x(), maxScreenPosition.y(), maxScreenPosition.z() ) );

					auto volumetricSampleCount = writer.declLocale( "volumetricSampleCount", c3d_froxelsData.sampleCount() );
					auto stepCountDepth = writer.declLocale( "stepCountDepth", c3d_froxelsData.getSamplesCountZ( froxelIndex3D, depthBounds ) );
					auto stepZ = writer.declLocale( "stepZ", 1.0f / writer.cast< sdw::Float >( stepCountDepth ) );
					auto count = writer.declLocale( "count", 0_u );
					auto ratioZ = writer.declLocale( "ratioZ", stepZ / 2.0f );

					auto stepX = writer.declLocale( "stepX", 1.0f / writer.cast< sdw::Float >( volumetricSampleCount.x() ) );
					auto stepY = writer.declLocale( "stepY", 1.0f / writer.cast< sdw::Float >( volumetricSampleCount.y() ) );
					auto minX = writer.declLocale( "minX", stepX / 2.0f );
					auto minY = writer.declLocale( "minY", stepY / 2.0f );

					auto accumulation = writer.declLocale( "accumulation", sdw::vec3( 0.0_f ) );
					sdwFOR( writer, sdw::UInt, z, 0u, z < stepCountDepth, ++z )
					{
						auto ratioY = writer.declLocale( "ratioY", minY );
						sdwFOR( writer, sdw::UInt, y, 0u, y < volumetricSampleCount.y(), ++y )
						{
							// No need to lerp right side, they are supposed to be the same.
							auto nearY = writer.declLocale( "nearY", mix( vsNearTopLeft.y(), vsNearBottomLeft.y(), ratioY ) );
							auto farY = writer.declLocale( "farY", mix( vsFarTopLeft.y(), vsFarBottomLeft.y(), ratioY ) );
							auto ratioX = writer.declLocale( "ratioY", minX );
							sdwFOR( writer, sdw::UInt, x, 0u, x < volumetricSampleCount.x(), ++x )
							{
								// No need to lerp bottom side, they are supposed to be the same.
								auto nearX = writer.declLocale( "nearX", mix( vsNearTopLeft.x(), vsNearTopRight.x(), ratioX ) );
								auto farX = writer.declLocale( "farX", mix( vsFarTopLeft.x(), vsFarTopRight.x(), ratioX ) );
								auto tapsVSPosition = writer.declLocale( "tapsVSPosition", sdw::vec3( mix( nearX, farX, ratioZ )
									, mix( nearY, farY, ratioZ )
									, mix( depthBounds.x(), depthBounds.y(), ratioZ ) ) );
								accumulation += computeLighting( writer, light, c3d_cameraData.curViewToWorld( sdw::vec4( tapsVSPosition, 1.0_f ) ).xyz() );

								//
								++count;
								ratioX += stepX;
							}
							sdwROF
								ratioY += stepY;
						}
						sdwROF
							ratioZ += stepZ;
					}
					sdwROF

					accumulation /= writer.cast< sdw::Float >( count );
					c3d_froxelsLightingR.atomicAdd( sdw::i32vec3( froxelIndex3D ), c3d_froxelsData.quantize( accumulation.x() ) );
					c3d_froxelsLightingG.atomicAdd( sdw::i32vec3( froxelIndex3D ), c3d_froxelsData.quantize( accumulation.y() ) );
					c3d_froxelsLightingB.atomicAdd( sdw::i32vec3( froxelIndex3D ), c3d_froxelsData.quantize( accumulation.z() ) );
				};

			auto processFroxelPointLight = writer.implementFunction< sdw::Void >( "c3d_processFroxelPointLight"
				, [&processFroxelLight]( shader::PointLight const & light, sdw::U32Vec3 const & froxelIndex3D )
				{
					processFroxelLight( light, froxelIndex3D );
				}
				, shader::InPointLight{ writer, "light" }
				, sdw::InU32Vec3{ writer, "froxelIndex3D" } );

			auto processFroxelSpotLight = writer.implementFunction< sdw::Void >( "c3d_processFroxelSpotLight"
				, [&processFroxelLight]( shader::SpotLight const & light, sdw::U32Vec3 const & froxelIndex3D )
				{
					processFroxelLight( light, froxelIndex3D );
				}
				, shader::InSpotLight{ writer, "light" }
				, sdw::InU32Vec3{ writer, "froxelIndex3D" } );

			writer.implementMainT< sdw::VoidT >( FroxelIntegrateTileSize
				, [pointLightsPass, &writer, &c3d_counters, &c3d_froxelsLights, &c3d_froxelsData, &lights, &processFroxelPointLight, &processFroxelSpotLight]( sdw::ComputeIn const & in )
				{
					auto counterIndex = pointLightsPass ? 0u : 1u;
					auto entryID = writer.declLocale( "entryID", in.workGroupID.x() * FroxelIntegrateTileSize + in.localInvocationIndex );
					sdwIF( writer, entryID >= c3d_counters[counterIndex] )
					{
						writer.returnStmt();
					}
					sdwFI;
					auto lightAndFroxelID = writer.declLocale< sdw::U32Vec2 >( "lightAndFroxelID", c3d_froxelsLights[entryID] );

					//
					auto lightID = writer.declLocale( "lightID", lightAndFroxelID.x() );
					auto froxelGroupID = writer.declLocale( "froxelGroupID", lightAndFroxelID.y() );
					auto minFroxelIndex3D = writer.declLocale( "minFroxelIndex3D", c3d_froxelsData.computeIndex3D( froxelGroupID ) );
					auto froxelSize = writer.declLocale( "froxelSize", c3d_froxelsData.froxelSize() );

					if ( pointLightsPass )
					{
						auto lightOffset = writer.declLocale( "lightOffset", lights.getDirectionalsEnd() + lightID * PointLightInstance::LightDataComponents );
						auto light = writer.declLocale( "light", lights.getPointLight( lightOffset ) );
						auto froxelIndex3D = writer.declLocale< sdw::U32Vec3 >( "froxelIndex3D" );

						for( u32 froxelIndex = 0u; froxelIndex < shader::FroxelsData::FroxelsPerGroup; ++froxelIndex )
						{
							froxelIndex3D = minFroxelIndex3D
								+ sdw::u32vec3( shader::FroxelsData::FroxelOffsets[froxelIndex]->x, shader::FroxelsData::FroxelOffsets[froxelIndex]->y, shader::FroxelsData::FroxelOffsets[froxelIndex]->z );

							sdwIF( writer, froxelIndex3D.x() < c3d_froxelsData.dimensions().x()
								&& froxelIndex3D.y() < c3d_froxelsData.dimensions().y()
								&& froxelIndex3D.z() < c3d_froxelsData.dimensions().z() )
							{
								processFroxelPointLight( light, froxelIndex3D );
							}
							sdwFI
						}
					}
					else
					{
						auto lightOffset = writer.declLocale( "lightOffset", lights.getPointsEnd() + lightID * SpotLightInstance::LightDataComponents );
						auto light = writer.declLocale( "light", lights.getSpotLight( lightOffset ) );
						auto froxelIndex3D = writer.declLocale< sdw::U32Vec3 >( "froxelIndex3D" );

						for ( u32 froxelIndex = 0u; froxelIndex < shader::FroxelsData::FroxelsPerGroup; ++froxelIndex )
						{
							froxelIndex3D = minFroxelIndex3D
								+ sdw::u32vec3( shader::FroxelsData::FroxelOffsets[froxelIndex]->x, shader::FroxelsData::FroxelOffsets[froxelIndex]->y, shader::FroxelsData::FroxelOffsets[froxelIndex]->z );

							sdwIF( writer, froxelIndex3D.x() < c3d_froxelsData.dimensions().x()
								&& froxelIndex3D.y() < c3d_froxelsData.dimensions().y()
								&& froxelIndex3D.z() < c3d_froxelsData.dimensions().z() )
							{
								processFroxelSpotLight( light, froxelIndex3D );
							}
							sdwFI
						}
					}
				} );
			return writer.getBuilder().releaseShader();
		}

		class FramePass
			: private DataHolderT< ShaderModule >
			, private DataHolderT< ashes::PipelineShaderStageCreateInfoArray >
			, public crg::ComputePass
		{
			using ShaderHolder = DataHolderT< ShaderModule >;
			using CreateInfoHolder = DataHolderT< ashes::PipelineShaderStageCreateInfoArray >;

		public:
			FramePass( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph
				, RenderDevice const & device
				, crg::cp::Config config
				, LightType lightType )
				: ShaderHolder{ ShaderModule{ VK_SHADER_STAGE_COMPUTE_BIT, String{ cuT( "IntegrateFroxels/" ) } + getName( lightType ), createShader( device, lightType == LightType::ePoint ) } }
				, CreateInfoHolder{ ashes::PipelineShaderStageCreateInfoArray{ makeShaderState( device, ShaderHolder::getData() ) } }
				, crg::ComputePass{ framePass, context, graph
					, crg::ru::Config{}
					, config
						.program( ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( CreateInfoHolder::getData() ) ) }
			{
			}
		};
	}

	namespace dspfxlb
	{
		enum class Bindings
		{
			eMainCamera,
			eClustersCamera,
			eRender,
			eFroxels,
		};

		static ShaderPtr createDebugDisplayShader( RenderDevice const & device )
		{
			sdw::TraditionalGraphicsWriter writer{ &c3d::getEngine( device ).getShaderAllocator() };
			shader::Utils utils{ writer };

			C3D_CameraNamed( writer
				, Main
				, Bindings::eMainCamera
				, 0u );
			C3D_CameraNamed( writer
				, Clusters
				, Bindings::eClustersCamera
				, 0u );
			C3D_Render( writer
				, Bindings::eRender
				, 0u );
			C3D_Froxels( writer
				, Bindings::eFroxels
				, 0u );

			writer.implementEntryPointT< shader::Position4FT, shader::Colour4FT >( [&c3d_cameraDataMain, &c3d_cameraDataClusters, &c3d_renderData, &c3d_froxelsData
				, &writer, &utils]( sdw::VertexInT< shader::Position4FT > const & in
					, sdw::VertexOutT< shader::Colour4FT > out )
				{
					auto froxelIndex3D = writer.declLocale( "froxelIndex3D"
						, c3d_froxelsData.computeIndex3D( writer.cast< sdw::UInt >( in.instanceIndex ) ) );
					auto tileNearFarZ = writer.declLocale( "tileNearFarZ"
						, c3d_froxelsData.getDepthBounds( froxelIndex3D.z() ) );
					auto aabb = writer.declLocale( "aabb"
						, utils.computeAABB( froxelIndex3D, c3d_froxelsData.froxelSize(), tileNearFarZ, c3d_cameraDataMain.getInvProjMtx(), c3d_renderData.invRenderSize() ) );
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

					out.colour() = vec4( writer.cast< sdw::Float >( froxelIndex3D.x() ) / writer.cast< sdw::Float >( c3d_froxelsData.dimensions().x() )
						, writer.cast< sdw::Float >( froxelIndex3D.y() ) / writer.cast< sdw::Float >( c3d_froxelsData.dimensions().y() )
						, writer.cast< sdw::Float >( froxelIndex3D.z() ) / writer.cast< sdw::Float >( c3d_froxelsData.dimensions().z() )
						, 1.0_f );
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

	void createIntegrateFroxelsPass( crg::FramePassGroup & graph
		, RenderDevice const & device
		, FrustumFroxels const & froxels
		, Texture & rawFroxelsLightingU32R
		, Texture & rawFroxelsLightingU32G
		, Texture & rawFroxelsLightingU32B )
	{
		auto const & scene = froxels.getScene();
		auto const & lights = scene.getLightCache();
		{
			// Point lights
			auto & point = graph.createPass( "IntegrateFroxels/Point"
				, [&device, &froxels, &scene]( crg::FramePass const & framePass
					, crg::GraphContext & context
					, crg::RunnableGraph & runGraph )
				{
					auto runPass = makeRawUnique< intfxl::FramePass >( framePass, context, runGraph, device
						, crg::cp::Config{}
							.indirectBuffer( crg::IndirectBuffer{ froxels.getPointIndirectBuffer(), u32( sizeof( VkDispatchIndirectCommand ) ) } )
							.isEnabled( crg::RunnablePass::IsEnabledCallback( [&scene](){ return scene.hasClusteredLights( LightType::ePoint ); } ) )
						, LightType::ePoint );
					c3d::getEngine( device ).registerTimer( makeString( framePass.getFullName() )
						, runPass->getTimer() );
					return runPass;
				} );
			lights.createPassBindingT( point, intfxl::Bindings::eLights );
			froxels.getCameraUbo().createPassBinding( point, intfxl::Bindings::eCamera );
			froxels.getFroxelsUbo().createPassBinding( point, intfxl::Bindings::eFroxels );
			froxels.getClustersUbo().createPassBinding( point, intfxl::Bindings::eClusters );
			point.addImplicit( *froxels.getIndirectBuffer().getLastAttach(), AccessState{ AccessFlags::eIndirectCommandRead, PipelineStageFlags::eDrawIndirect } );
			point.addInputStorageT( *froxels.getCounterBuffer().getLastAttach(), intfxl::Bindings::eCounters );
			point.addInputStorageT( *froxels.getFroxelsPointLightsBuffer().getLastAttach(), intfxl::Bindings::eFroxelsLights );
			rawFroxelsLightingU32R.setLastAttach( point.addClearableOutputStorageImageT( rawFroxelsLightingU32R.getWholeViewId(), intfxl::Bindings::eFroxelsLightingR ) );
			rawFroxelsLightingU32G.setLastAttach( point.addClearableOutputStorageImageT( rawFroxelsLightingU32G.getWholeViewId(), intfxl::Bindings::eFroxelsLightingG ) );
			rawFroxelsLightingU32B.setLastAttach( point.addClearableOutputStorageImageT( rawFroxelsLightingU32B.getWholeViewId(), intfxl::Bindings::eFroxelsLightingB ) );
		}
		{
			// Spot lights
			auto & spot = graph.createPass( "IntegrateFroxels/Spot"
				, [&device, &froxels, &scene]( crg::FramePass const & framePass
					, crg::GraphContext & context
					, crg::RunnableGraph & runGraph )
				{
					auto runPass = makeRawUnique< intfxl::FramePass >( framePass, context, runGraph, device
						, crg::cp::Config{}
							.indirectBuffer( crg::IndirectBuffer{ froxels.getSpotIndirectBuffer(), u32( sizeof( VkDispatchIndirectCommand ) ) } )
							.isEnabled( crg::RunnablePass::IsEnabledCallback( [&scene](){ return scene.hasClusteredLights( LightType::eSpot ); } ) )
						, LightType::eSpot );
					c3d::getEngine( device ).registerTimer( makeString( framePass.getFullName() )
						, runPass->getTimer() );
					return runPass;
				} );
			lights.createPassBindingT( spot, intfxl::Bindings::eLights );
			froxels.getCameraUbo().createPassBinding( spot, intfxl::Bindings::eCamera );
			froxels.getFroxelsUbo().createPassBinding( spot, intfxl::Bindings::eFroxels );
			froxels.getClustersUbo().createPassBinding( spot, intfxl::Bindings::eClusters );
			spot.addImplicit( *froxels.getIndirectBuffer().getLastAttach(), AccessState{ AccessFlags::eIndirectCommandRead, PipelineStageFlags::eDrawIndirect } );
			spot.addInputStorageT( *froxels.getCounterBuffer().getLastAttach(), intfxl::Bindings::eCounters );
			spot.addInputStorageT( *froxels.getFroxelsSpotLightsBuffer().getLastAttach(), intfxl::Bindings::eFroxelsLights );
			rawFroxelsLightingU32R.setLastAttach( spot.addInOutStorageT( *rawFroxelsLightingU32R.getLastAttach(), intfxl::Bindings::eFroxelsLightingR ) );
			rawFroxelsLightingU32G.setLastAttach( spot.addInOutStorageT( *rawFroxelsLightingU32G.getLastAttach(), intfxl::Bindings::eFroxelsLightingG ) );
			rawFroxelsLightingU32B.setLastAttach( spot.addInOutStorageT( *rawFroxelsLightingU32B.getLastAttach(), intfxl::Bindings::eFroxelsLightingB ) );
		}
	}

	void createDisplayFroxelsAABBProgram( RenderDevice const & device
		, FrustumFroxels const & froxels
		, CameraUbo const & mainCameraUbo
		, CameraUbo const & clustersCameraUbo
		, RenderUbo const & renderUbo
		, ashes::PipelineShaderStageCreateInfoArray & program
		, ashes::VkDescriptorSetLayoutBindingArray & bindings
		, ashes::WriteDescriptorSetArray & writes )
	{
		ProgramModule programModule{ "FroxelsAABB", dspfxlb::createDebugDisplayShader( device ) };
		program = makeProgramStates( device, programModule );

		addDescriptorSetLayoutBindingT( bindings, dspfxlb::Bindings::eMainCamera, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT );
		addDescriptorSetLayoutBindingT( bindings, dspfxlb::Bindings::eClustersCamera, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT );
		addDescriptorSetLayoutBindingT( bindings, dspfxlb::Bindings::eRender, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT );
		addDescriptorSetLayoutBindingT( bindings, dspfxlb::Bindings::eFroxels, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT );

		writes.push_back( mainCameraUbo.getDescriptorWrite( dspfxlb::Bindings::eMainCamera ) );
		writes.push_back( clustersCameraUbo.getDescriptorWrite( dspfxlb::Bindings::eMainCamera ) );
		writes.push_back( renderUbo.getDescriptorWrite( dspfxlb::Bindings::eRender ) );
		writes.push_back( froxels.getFroxelsUbo().getDescriptorWrite( dspfxlb::Bindings::eFroxels ) );
	}

	//*********************************************************************************************
}
