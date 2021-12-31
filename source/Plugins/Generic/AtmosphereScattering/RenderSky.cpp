#include "AtmosphereScattering/RenderSky.hpp"

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

	namespace sky
	{
		castor::String const Name{ "RenderSky" };

		enum Bindings : uint32_t
		{
			eMatrix,
			eScene,
			eAtmosphereScattering,
			eTransmittance,
			eInscatter,
			eCount,
		};

		static castor3d::ShaderPtr getVertexProgram()
		{
			sdw::VertexWriter writer;

			C3D_Matrix( writer
				, Bindings::eMatrix
				, 0u );
			C3D_Scene( writer
				, Bindings::eScene
				, 0u );
			C3D_AtmosphereScattering( writer
				, Bindings::eAtmosphereScattering
				, 0u );

			auto inPosition = writer.declInput< sdw::Vec4 >( "inPosition", 0u );
			auto outViewDir = writer.declOutput< sdw::Vec3 >( "outViewDir", 0u );

			writer.implementMainT< sdw::VoidT, sdw::VoidT >( sdw::VertexIn{ writer }
				, sdw::VertexOutT< sdw::VoidT >{ writer }
				, [&]( sdw::VertexIn in
					, sdw::VertexOutT< sdw::VoidT > out )
				{
					outViewDir = c3d_matrixData.curViewToWorld( vec4( c3d_matrixData.projToView( inPosition ).xyz(), 0.0_f ) ).xyz();
					out.vtx.position = vec4( inPosition.xy(), 0.9999999, 1.0 );
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		static castor3d::ShaderPtr getPixelProgram( castor3d::Engine const & engine )
		{
			sdw::FragmentWriter writer;

			castor3d::shader::Utils utils{ writer, engine };

			C3D_Scene( writer
				, Bindings::eScene
				, 0u );
			C3D_AtmosphereScattering( writer
				, uint32_t( Bindings::eAtmosphereScattering )
				, 0u );
			auto transmittanceMap = writer.declCombinedImg< sdw::CombinedImage2DRgba32 >( "transmittanceMap"
				, uint32_t( Bindings::eTransmittance )
				, 0u );
			auto inscatterMap = writer.declCombinedImg< sdw::CombinedImage3DRgba32 >( "inscatterMap"
				, uint32_t( Bindings::eInscatter )
				, 0u );

			auto inViewDir = writer.declInput< sdw::Vec3 >( "inViewDir", 0u );

			// Fragment Outputs
			auto pxl_colour( writer.declOutput< sdw::Vec4 >( "pxl_colour", 0 ) );
			auto const M_PI{ sdw::Float{ castor::Pi< float > } };

			AtmosphereConfig atmosphereConfig{ writer, c3d_atmosphereData };

			writer.implementMainT< SurfaceT, sdw::VoidT >( sdw::FragmentInT< SurfaceT >{ writer }
				, sdw::FragmentOut{ writer }
				, [&]( sdw::FragmentInT< SurfaceT > in
					, sdw::FragmentOut out )
				{
					auto pi = writer.declLocale( "pi"
						, sdw::Float{ castor::Pi< float > } );

					auto worldSunDir = writer.declLocale( "worldSunDir"
						, c3d_atmosphereData.getSunDir() );

					auto v = writer.declLocale( "v"
						, normalize( inViewDir ) );
					auto sunColor = writer.declLocale( "sunColor"
						, vec3( step( cos( pi / 180.0_f ), dot( v, worldSunDir ) ) ) * vec3( c3d_atmosphereData.sunIntensity ) );
					auto extinction = writer.declLocale< sdw::Vec3 >( "extinction" );
					auto inscatter = writer.declLocale( "inscatter"
						, atmosphereConfig.getSkyRadiance( c3d_sceneData.cameraPosition + c3d_atmosphereData.earthPos
							, v
							, worldSunDir
							, transmittanceMap
							, inscatterMap
							, extinction ) );
					auto finalColor = writer.declLocale( "finalColor"
						, sunColor * extinction + inscatter );
					pxl_colour = vec4( finalColor, 1.0_f );
				} );

			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}
	}

	//************************************************************************************************

	crg::FramePass const * createRenderSkyPass( castor3d::RenderDevice const & device
		, crg::FramePassGroup & graph
		, crg::FramePassArray previousPasses
		, AtmosphereScatteringUbo const & atmosphereUbo
		, castor3d::MatrixUbo const & matrixUbo
		, castor3d::SceneUbo const & sceneUbo
		, crg::ImageViewId const & transmittance
		, crg::ImageViewId const & inscatter
		, crg::ImageViewId const & colourOutput
		, crg::RunnablePass::IsEnabledCallback isEnabled
		, ashes::PipelineShaderStageCreateInfoArray & modules )
	{
		modules.push_back( makeShaderState( device
			, { VK_SHADER_STAGE_VERTEX_BIT, sky::Name, sky::getVertexProgram() } ) );
		modules.push_back( makeShaderState( device
			, { VK_SHADER_STAGE_FRAGMENT_BIT, sky::Name, sky::getPixelProgram( *device.renderSystem.getEngine() ) } ) );

		auto extent = getExtent( colourOutput );
		auto & result = graph.createPass( sky::Name
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
		matrixUbo.createPassBinding( result
			, sky::Bindings::eMatrix );
		sceneUbo.createPassBinding( result
			, sky::Bindings::eScene );
		atmosphereUbo.createPassBinding( result
			, sky::Bindings::eAtmosphereScattering );
		result.addSampledView( transmittance
			, sky::Bindings::eTransmittance
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
		result.addSampledView( inscatter
			, sky::Bindings::eInscatter
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
		result.addInOutColourView( colourOutput );

		return &result;
	}

	//************************************************************************************************
}
