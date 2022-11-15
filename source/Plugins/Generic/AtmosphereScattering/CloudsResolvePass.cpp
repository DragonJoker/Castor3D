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
#include <Castor3D/Shader/Shaders/GlslUtils.hpp>

#include <RenderGraph/RunnableGraph.hpp>
#include <RenderGraph/RunnablePasses/RenderQuad.hpp>

#include <ShaderWriter/CompositeTypes/IOStructHelper.hpp>
#include <ShaderWriter/CompositeTypes/IOStructInstanceHelper.hpp>
#include <ShaderWriter/Source.hpp>

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

		static castor3d::ShaderPtr getVertexProgram()
		{
			sdw::VertexWriter writer;

			sdw::Vec2 position = writer.declInput< sdw::Vec2 >( "position", 0u );

			writer.implementMainT< sdw::VoidT, sdw::VoidT >( sdw::VertexIn{ writer }
				, sdw::VertexOut{ writer }
				, [&]( sdw::VertexIn in
					, sdw::VertexOut out )
				{
					out.vtx.position = vec4( position, 0.0_f, 1.0_f );
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		static castor3d::ShaderPtr getPixelProgram( castor3d::Engine const & engine
			, VkExtent3D renderSize )
		{
			sdw::FragmentWriter writer;
			castor3d::shader::Utils utils{ writer };

			C3D_Camera( writer
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
				, AtmosphereModel::Settings{ castor::Length::fromUnit( 1.0f, engine.getLengthUnit() ) }
					.setCameraData( &c3d_cameraData ) };

			auto fragColour = writer.declOutput< sdw::Vec4 >( "fragColour", 0u );

			auto targetSize = writer.declConstant( "targetSize"
				, vec2( sdw::Float{ float( renderSize.width ) }, float( renderSize.height ) ) );
			auto offsetX = 1.0_f / targetSize.x();
			auto offsetY = 1.0_f / targetSize.y();
			auto kernel = std::vector< sdw::Float >{ 1.0_f / 16.0_f, 2.0_f / 16.0_f, 1.0_f / 16.0_f
					, 2.0_f / 16.0_f, 4.0_f / 16.0_f, 2.0_f / 16.0_f
					, 1.0_f / 16.0_f, 2.0_f / 16.0_f, 1.0_f / 16.0_f };
			auto offsets = std::vector< sdw::Vec2 >{ vec2( -offsetX, offsetY ) // top-left
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

			auto computeLighting = writer.implementFunction< sdw::Vec4 >( "computeLighting"
				, [&]( sdw::Vec4 skyColor
					, sdw::Vec3 cloudsColor
					, sdw::Float const & skyBlendFactor
					, sdw::Float const & cloudsDensity )
				{
					// Blend background and clouds.
					auto blendSkyColor = writer.declLocale( "blendSkyColor"
						, mix( skyColor.rgb()
							, c3d_cloudsData.bottomColor()
							, vec3( c3d_cloudsData.coverage() ) ) );

					writer.returnStmt( vec4( mix( skyColor.rgb()
							, cloudsColor + ( skyBlendFactor * blendSkyColor )
							, vec3( cloudsDensity ) )
						, skyColor.a() ) );
				}
				, sdw::InVec4{ writer, "skyColor" }
				, sdw::InVec3{ writer, "cloudsColor" }
				, sdw::InFloat{ writer, "skyBlendFactor" }
				, sdw::InFloat{ writer, "cloudsDensity" } );

			writer.implementMainT< sdw::VoidT, sdw::VoidT >( sdw::FragmentIn{ writer }
				, sdw::FragmentOut{ writer }
				, [&]( sdw::FragmentIn in
					, sdw::FragmentOut out )
				{
					auto texCoords = writer.declLocale( "texCoords"
						, vec2( in.fragCoord.xy() ) / targetSize );
					texCoords.y() = 1.0_f - texCoords.y();

					auto sky = writer.declLocale( "sky"
						, skyMap.sample( texCoords ) );
					auto sun = writer.declLocale( "sun"
						, sunMap.sample( texCoords ) );

					IF( writer, c3d_cloudsData.coverage() > 0.0_f )
					{
						auto clouds = writer.declLocale( "clouds"
							, gaussianBlur( cloudsMap, texCoords ) );
						fragColour = computeLighting( sky
							, clouds.rgb()
							, sky.a()
							, clouds.a() );
					}
					ELSE
					{
						fragColour = sky + sun;
					}
					FI;
				} );

			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}
	}

	//************************************************************************************************

	CloudsResolvePass::CloudsResolvePass( crg::FramePassGroup & graph
		, crg::FramePassArray const & previousPasses
		, castor3d::RenderDevice const & device
		, CameraUbo const & cameraUbo
		, AtmosphereScatteringUbo const & atmosphereUbo
		, CloudsUbo const & cloudsUbo
		, crg::ImageViewId const & sky
		, crg::ImageViewId const & sun
		, crg::ImageViewId const & clouds
		, crg::ImageViewId const & resultView
		, uint32_t index )
		: castor::Named{ "Clouds/ResolvePass" + castor::string::toString( index ) }
		, m_vertexShader{ VK_SHADER_STAGE_VERTEX_BIT, getName(), cloudsres::getVertexProgram() }
		, m_pixelShader{ VK_SHADER_STAGE_FRAGMENT_BIT, getName(), cloudsres::getPixelProgram( *device.renderSystem.getEngine(), getExtent( resultView ) ) }
		, m_stages{ makeShaderState( device, m_vertexShader )
			, makeShaderState( device, m_pixelShader ) }
	{
		auto renderSize = getExtent( resultView );
		auto & pass = graph.createPass( getName()
			, [this, &device, renderSize]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				auto result = crg::RenderQuadBuilder{}
					.renderSize( { renderSize.width, renderSize.height } )
					.program( ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( m_stages ) )
					.build( framePass, context, graph );
				device.renderSystem.getEngine()->registerTimer( framePass.getFullName()
					, result->getTimer() );
				return result;
			} );
		pass.addDependencies( previousPasses );
		cameraUbo.createPassBinding( pass
			, cloudsres::eCamera );
		atmosphereUbo.createPassBinding( pass
			, cloudsres::eAtmosphere );
		cloudsUbo.createPassBinding( pass
			, cloudsres::eClouds );
		pass.addSampledView( sky
			, cloudsres::eMapSky );
		pass.addSampledView( sun
			, cloudsres::eMapSun );
		pass.addSampledView( clouds
			, cloudsres::eMapClouds );
		pass.addOutputColourView( resultView );
		m_lastPass = &pass;
	}

	void CloudsResolvePass::accept( castor3d::PipelineVisitor & visitor )
	{
		visitor.visit( m_vertexShader );
		visitor.visit( m_pixelShader );
	}

	//************************************************************************************************
}
