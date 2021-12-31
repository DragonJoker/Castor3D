#include "AtmosphereScattering/GenerateSkymap.hpp"

#include "AtmosphereScattering/Atmosphere.hpp"
#include "AtmosphereScattering/AtmosphereScatteringUbo.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Render/RenderDevice.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Render/Technique/RenderTechniqueVisitor.hpp>
#include <Castor3D/Shader/Program.hpp>
#include <Castor3D/Shader/Shaders/GlslUtils.hpp>
#include <Castor3D/Shader/Ubos/MatrixUbo.hpp>
#include <Castor3D/Shader/Ubos/SceneUbo.hpp>

#include <RenderGraph/RunnableGraph.hpp>
#include <RenderGraph/RunnablePasses/RenderMesh.hpp>

#include <ShaderWriter/Source.hpp>

#include <ashespp/Buffer/Buffer.hpp>

namespace atmosphere_scattering
{
	//*********************************************************************************************

	namespace skymap
	{
		castor::String const Name{ "GenerateSkymap" };

		enum Bindings : uint32_t
		{
			eAtmosphereScattering,
			eTransmittance,
			eInscatter,
			eSkyIrradiance,
			eCount,
		};

		static castor3d::ShaderPtr getVertexProgram()
		{
			sdw::VertexWriter writer;

			auto inPosition = writer.declInput< sdw::Vec4 >( "inPosition", 0u );
			auto U = writer.declOutput< sdw::Vec2 >( "U", 0u );

			writer.implementMainT< sdw::VoidT, sdw::VoidT >( sdw::VertexIn{ writer }
				, sdw::VertexOutT< sdw::VoidT >{ writer }
				, [&]( sdw::VertexIn in
					, sdw::VertexOutT< sdw::VoidT > out )
				{
					out.vtx.position = inPosition;
					U = inPosition.xy() * vec2( 1.1_f );
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		static castor3d::ShaderPtr getPixelProgram( castor3d::Engine const & engine )
		{
			sdw::FragmentWriter writer;

			castor3d::shader::Utils utils{ writer, engine };

			C3D_AtmosphereScattering( writer
				, uint32_t( Bindings::eAtmosphereScattering )
				, 0u );
			auto transmittanceMap = writer.declCombinedImg< sdw::CombinedImage2DRgba32 >( "transmittanceMap"
				, uint32_t( Bindings::eTransmittance )
				, 0u );
			auto inscatterMap = writer.declCombinedImg< sdw::CombinedImage3DRgba32 >( "inscatterMap"
				, uint32_t( Bindings::eInscatter )
				, 0u );
			auto skyIrradianceMap = writer.declCombinedImg< sdw::CombinedImage2DRgba32 >( "skyIrradianceMap"
				, uint32_t( Bindings::eSkyIrradiance )
				, 0u );

			auto U = writer.declInput< sdw::Vec2 >( "U", 0u );

			// Fragment Outputs
			auto pxl_colour( writer.declOutput< sdw::Vec4 >( "pxl_colour", 0u ) );
			auto const M_PI{ sdw::Float{ castor::Pi< float > } };

			AtmosphereConfig atmosphereConfig{ writer, c3d_atmosphereData };

			writer.implementMainT< SurfaceT, sdw::VoidT >( sdw::FragmentInT< SurfaceT >{ writer }
				, sdw::FragmentOut{ writer }
				, [&]( sdw::FragmentInT< SurfaceT > in
					, sdw::FragmentOut out )
				{
					auto worldSunDir = writer.declLocale( "worldSunDir"
						, c3d_atmosphereData.getSunDir() );

					auto u = writer.declLocale( "u"
						, U );
					auto l = writer.declLocale( "l"
						, dot( u, u ) );
					auto result = writer.declLocale( "result"
						, vec4( 0.0_f ) );

					IF( writer, l <= 1.02_f )
					{
						IF( writer, l > 1.0_f )
						{
							u = u / vec2( l );
							l = 1.0_f / l;
						}
						FI;

						// inverse stereographic projection,
						// from skymap coordinates to world space directions
						auto r = writer.declLocale( "r"
							, vec3( vec2( 2.0_f ) * u, 1.0_f - l ) / ( 1.0_f + l ) );

						auto extinction = writer.declLocale< sdw::Vec3 >( "extinction" );
						result.rgb() = atmosphereConfig.getSkyRadiance( c3d_atmosphereData.earthPos
							, r
							, worldSunDir
							, transmittanceMap
							, inscatterMap
							, extinction );

#ifdef CLOUDS
						vec4 cloudL = cloudColorV( vec3( 0.0 ), r, sunDir );
						result.rgb = cloudL.rgb * cloudL.a + result.rgb * ( 1.0 - cloudL.a );
#endif
					}
					ELSE
					{
						// below horizon:
						// use average fresnel * average sky radiance
						// to simulate multiple reflections on waves
						auto const avgFresnel = 0.17_f;
						result.rgb() = atmosphereConfig.getSkyIrradiance( skyIrradianceMap
							, c3d_atmosphereData.earthPos.z()
							, worldSunDir.z() ) / M_PI * avgFresnel;
					}
					FI;

					pxl_colour = result;
				} );

			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}
	}

	//************************************************************************************************

	crg::FramePass const * createGenerateSkymapPass( castor3d::RenderDevice const & device
		, crg::FramePassGroup & graph
		, crg::FramePassArray previousPasses
		, AtmosphereScatteringUbo const & oceanUbo
		, crg::ImageViewId const & transmittance
		, crg::ImageViewId const & inscatter
		, crg::ImageViewId const & skyIrradiance
		, crg::ImageViewId const & colourOutput
		, crg::RunnablePass::IsEnabledCallback isEnabled
		, ashes::PipelineShaderStageCreateInfoArray & modules )
	{
		modules.push_back( makeShaderState( device
			, { VK_SHADER_STAGE_VERTEX_BIT, skymap::Name, skymap::getVertexProgram() } ) );
		modules.push_back( makeShaderState( device
			, { VK_SHADER_STAGE_FRAGMENT_BIT, skymap::Name, skymap::getPixelProgram( *device.renderSystem.getEngine() ) } ) );

		auto extent = getExtent( colourOutput );
		auto & result = graph.createPass( skymap::Name
			, [&device, &modules, extent, isEnabled]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runnableGraph )
			{
				auto res = std::make_unique< crg::RenderQuad >( framePass
					, context
					, runnableGraph
					, crg::ru::Config{}
					, crg::rq::Config{}
						.isEnabled( isEnabled )
						.program( { modules.front(), modules.back() } )
						.renderSize( { extent.width, extent.height } ) );
				device.renderSystem.getEngine()->registerTimer( framePass.getFullName()
					, res->getTimer() );
				return res;
			} );
		result.addDependencies( previousPasses );
		oceanUbo.createPassBinding( result
			, skymap::Bindings::eAtmosphereScattering );
		result.addSampledView( transmittance
			, skymap::Bindings::eTransmittance
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
		result.addSampledView( inscatter
			, skymap::Bindings::eInscatter
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
		result.addSampledView( skyIrradiance
			, skymap::Bindings::eSkyIrradiance
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
		result.addInOutColourView( colourOutput );

		return &result;
	}

	//************************************************************************************************
}
