#include "AtmosphereScattering/CloudsResolvePass.hpp"

#include "AtmosphereScattering/AtmosphereCameraUbo.hpp"
#include "AtmosphereScattering/AtmosphereModel.hpp"
#include "AtmosphereScattering/AtmosphereScatteringUbo.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Render/RenderDevice.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Render/RenderTechniqueVisitor.hpp>
#include <Castor3D/Shader/Program.hpp>
#include <Castor3D/Shader/Shaders/GlslRay.hpp>
#include <Castor3D/Shader/Shaders/GlslBaseIO.hpp>
#include <Castor3D/Shader/Shaders/GlslUtils.hpp>

#include <RenderGraph/RunnableGraph.hpp>
#include <RenderGraph/RunnablePasses/RenderQuad.hpp>

#include <ShaderWriter/Source.hpp>
#include <ShaderWriter/TraditionalGraphicsWriter.hpp>
#include <ShaderWriter/CompositeTypes/IOStructHelper.hpp>
#include <ShaderWriter/CompositeTypes/IOStructInstanceHelper.hpp>

#include <ashespp/Buffer/Buffer.hpp>

namespace atmosphere_scattering
{
	//*********************************************************************************************

	namespace cloudsres
	{
		enum Bindings : uint32_t
		{
			eCamera,
			eAtmosphere,
			eClouds,
			eMapSky,
			eMapSun,
			eMapClouds,
			eCount,
		};

		static c3d::ShaderPtr getProgram( c3d::Engine & engine
			, c3d::Extent3D const & renderSize )
		{
			sdw::TraditionalGraphicsWriter writer{ &engine.getShaderAllocator() };

			c3d::shader::Utils utils{ writer };

			ATM_Camera( writer
				, uint32_t( Bindings::eCamera )
				, 0u );
			C3D_AtmosphereScattering( writer
				, uint32_t( Bindings::eAtmosphere )
				, 0u );
			C3D_Clouds( writer
				, uint32_t( Bindings::eClouds )
				, 0u );
			auto skyMap = writer.declCombinedImg< sdw::CombinedImage2DRgba32 >("skyMap"
				, uint32_t( Bindings::eMapSky )
				, 0u );
			auto sunMap = writer.declCombinedImg< sdw::CombinedImage2DRgba32 >("sunMap"
				, uint32_t( Bindings::eMapSun )
				, 0u );
			auto cloudsMap = writer.declCombinedImg< sdw::CombinedImage2DRgba32 >( "cloudsMap"
				, uint32_t( Bindings::eMapClouds )
				, 0u );

			AtmosphereModel atmosphere{ writer
				, c3d_atmosphereData
				, AtmosphereModel::Settings{ c3d::Length::fromUnit( 1.0f, engine.getLengthUnit() ) }
					.setCameraData( &atm_cameraData ) };

			auto targetSize = writer.declConstant( "targetSize"
				, vec2( sdw::Float{ float( renderSize.width ) }, float( renderSize.height ) ) );
			auto offsetX = writer.declConstant( "offsetX"
				, 1.0_f / targetSize.x() );
			auto offsetY = writer.declConstant( "offsetY"
				, 1.0_f / targetSize.y() );
			auto kernel = c3d::Vector< sdw::Float >{ 1.0_f / 16.0_f, 2.0_f / 16.0_f, 1.0_f / 16.0_f
					, 2.0_f / 16.0_f, 4.0_f / 16.0_f, 2.0_f / 16.0_f
					, 1.0_f / 16.0_f, 2.0_f / 16.0_f, 1.0_f / 16.0_f };
			auto offsets = c3d::Vector< sdw::Vec2 >{ vec2( -offsetX, offsetY ) // top-left
					, vec2( 0.0_f, offsetY ) // top-center
					, vec2( offsetX, offsetY ) // top-right
					, vec2( -offsetX, 0.0_f )   // center-left
					, vec2( 0.0_f, 0.0_f )   // center-center
					, vec2( offsetX, 0.0_f )   // center-right
					, vec2( -offsetX, -offsetY ) // bottom-left
					, vec2( 0.0_f, -offsetY ) // bottom-center
					, vec2( offsetX, -offsetY ) };  // bottom-right

			auto gaussianBlur = writer.implementFunction< sdw::Vec4 >( "gaussianBlur"
				, [&]( sdw::CombinedImage2DRgba32 const & tex
					, sdw::Vec2 const & uv )
				{
					auto col = writer.declLocale( "col"
						, vec4( 0.0_f ) );

					for ( uint32_t i = 0u; i < 9u; ++i )
					{
						col += kernel[i] * tex.sample( uv + offsets[i] );
					}

					writer.returnStmt( col );
				}
				, sdw::InCombinedImage2DRgba32{ writer, "tex" }
				, sdw::InVec2{ writer, "uv" } );

			auto computeLighting = writer.implementFunction< sdw::Vec3 >( "computeLighting"
				, [&]( c3d::shader::Ray const & ray
					, sdw::Vec3 skyColor
					, sdw::Vec3 sunColour
					, sdw::Vec3 cloudsColor
					, sdw::Float const & skyBlendFactor
					, sdw::Float const & cloudsDensity )
				{
					// Blend background and clouds.
					auto blendSkyColor = writer.declLocale( "blendSkyColor"
						, mix( skyColor
							, c3d_cloudsData.bottomColor()
							, vec3( c3d_cloudsData.coverage() ) ) );
					writer.returnStmt( mix( skyColor
						, cloudsColor + ( skyBlendFactor * ( blendSkyColor ) )
						, vec3( cloudsDensity ) ) );
				}
				, c3d::shader::InRay{ writer, "ray" }
				, sdw::InVec3{ writer, "skyColor" }
				, sdw::InVec3{ writer, "sunColour" }
				, sdw::InVec3{ writer, "cloudsColor" }
				, sdw::InFloat{ writer, "skyBlendFactor" }
				, sdw::InFloat{ writer, "cloudsDensity" } );

			writer.implementEntryPointT< c3ds::Position2FT, sdw::VoidT >( [&]( sdw::VertexInT< c3ds::Position2FT > in
				, sdw::VertexOut out )
				{
					out.vtx.position = vec4( in.position(), 0.0_f, 1.0_f );
				} );

			writer.implementEntryPointT< sdw::VoidT, c3ds::Colour4FT >( [&]( sdw::FragmentIn in
				, sdw::FragmentOutT< c3ds::Colour4FT > out )
				{
					auto texCoords = writer.declLocale( "texCoords"
						, vec2( in.fragCoord.xy() ) / targetSize );
					texCoords.y() = 1.0_f - texCoords.y();

					auto sky = writer.declLocale( "sky"
						, skyMap.sample( texCoords ) );
					auto sun = writer.declLocale( "sun"
						, sunMap.sample( texCoords ) );

					sdwIF( writer, c3d_cloudsData.coverage() > 0.0_f )
					{
						auto ray = writer.declLocale( "ray"
							, atmosphere.castRay( texCoords ) );
						auto clouds = writer.declLocale( "clouds"
							, gaussianBlur( cloudsMap, texCoords ) );
						out.colour() = vec4( computeLighting( ray
								, sky.rgb()
								, sun.rgb()
								, clouds.rgb()
								, sky.a()
								, clouds.a() )
							, clouds.a() );
					}
					sdwELSE
					{
						out.colour() = sky + sun;
					}
					sdwFI
				} );

			return writer.getBuilder().releaseShader();
		}
	}

	//************************************************************************************************

	CloudsResolvePass::CloudsResolvePass( crg::FramePassGroup & graph
		, c3d::RenderDevice const & device
		, CameraUbo const & cameraUbo
		, AtmosphereScatteringUbo const & atmosphereUbo
		, CloudsUbo const & cloudsUbo
		, c3d::Texture const & sky
		, c3d::Texture const & sun
		, c3d::Texture const & clouds
		, c3d::Texture & result
		, uint32_t index )
		: c3d::Named{ cuT( "Clouds/ResolvePass" ) + c3d::string::toString( index ) }
		, m_shader{ getName(), cloudsres::getProgram( *device.renderSystem.getEngine(), result.getExtent() ) }
		, m_stages{ makeProgramStates( device, m_shader ) }
	{
		auto renderSize = result.getExtent();
		auto & pass = graph.createPass( c3d::toUtf8( getName() )
			, [this, &device, renderSize]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				auto runPass = crg::RenderQuadBuilder{}
					.renderSize( { renderSize.width, renderSize.height } )
					.program( ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( m_stages ) )
					.build( framePass, context, graph );
				device.renderSystem.getEngine()->registerTimer( c3d::makeString( framePass.getFullName() )
					, runPass->getTimer() );
				return runPass;
			} );
		cameraUbo.createPassBinding( pass, cloudsres::eCamera );
		atmosphereUbo.createPassBinding( pass, cloudsres::eAtmosphere );
		cloudsUbo.createPassBinding( pass, cloudsres::eClouds );
		pass.addInputSampled( *sky.getSampledLastAttach(), cloudsres::eMapSky );
		pass.addInputSampled( *sun.getSampledLastAttach(), cloudsres::eMapSun );
		pass.addInputSampled( *clouds.getSampledLastAttach(), cloudsres::eMapClouds );
		result.setLastAttach( pass.addOutputColourTarget( result.getTargetViewId() ) );
	}

	void CloudsResolvePass::accept( c3d::ConfigurationVisitorBase & visitor )
	{
		visitor.visit( m_shader );
	}

	//************************************************************************************************
}
