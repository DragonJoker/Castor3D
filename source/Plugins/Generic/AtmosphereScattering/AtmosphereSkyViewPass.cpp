#include "AtmosphereScattering/AtmosphereSkyViewPass.hpp"

#include "AtmosphereScattering/Atmosphere.hpp"
#include "AtmosphereScattering/AtmosphereScatteringUbo.hpp"
#include "AtmosphereScattering/AtmosphereCameraUbo.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Render/RenderDevice.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Render/Technique/RenderTechniqueVisitor.hpp>
#include <Castor3D/Shader/Program.hpp>
#include <Castor3D/Shader/Shaders/GlslUtils.hpp>
#include <Castor3D/Shader/Ubos/MatrixUbo.hpp>
#include <Castor3D/Shader/Ubos/SceneUbo.hpp>

#include <RenderGraph/RunnableGraph.hpp>
#include <RenderGraph/RunnablePasses/RenderQuad.hpp>

#include <ShaderWriter/Source.hpp>

namespace atmosphere_scattering
{
	//*********************************************************************************************

	namespace skyview
	{
		enum Bindings : uint32_t
		{
			eCamera,
			eAtmosphere,
			eTransmittance,
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

		static castor3d::ShaderPtr getPixelProgram( VkExtent3D const & renderSize
			, VkExtent3D const & transmittanceExtent )
		{
			sdw::FragmentWriter writer;

			C3D_Camera( writer
				, Bindings::eCamera
				, 0u );
			C3D_AtmosphereScattering( writer
				, uint32_t( Bindings::eAtmosphere )
				, 0u );
			auto transmittanceMap = writer.declCombinedImg< sdw::CombinedImage2DRgba32 >( "transmittanceMap"
				, uint32_t( Bindings::eTransmittance )
				, 0u );

			auto  sampleCountIni = writer.declConstant( "sampleCountIni"
				, 30.0_f );	// Can go a low as 10 sample but energy lost starts to be visible.
			auto  depthBufferValue = writer.declConstant( "depthBufferValue"
				, -1.0_f );
			auto planetRadiusOffset = writer.declConstant( "planetRadiusOffset"
				, 0.01_f );

			// Fragment Outputs
			auto pxl_colour( writer.declOutput< sdw::Vec4 >( "pxl_colour", 0u ) );
			auto const M_PI{ sdw::Float{ castor::Pi< float > } };

			AtmosphereConfig atmosphereConfig{ writer
				, c3d_atmosphereData
				, { false, nullptr, true, true }
				, { transmittanceExtent.width, transmittanceExtent.height }
				, &transmittanceMap };

			writer.implementMainT< sdw::VoidT, sdw::VoidT >( sdw::FragmentIn{ writer }
				, sdw::FragmentOut{ writer }
				, [&]( sdw::FragmentIn in
					, sdw::FragmentOut out )
				{
					auto targetSize = writer.declLocale( "targetSize"
						, vec2( sdw::Float{ float( renderSize.width + 1u ) }, float( renderSize.height + 1u ) ) );
					auto pixPos = writer.declLocale( "pixPos"
						, in.fragCoord.xy() );
					auto uv = writer.declLocale( "uv"
						, pixPos / targetSize );

					auto clipSpace = writer.declLocale( "clipSpace"
						, atmosphereConfig.getClipSpace( uv, 1.0_f ) );
					auto hPos = writer.declLocale( "hPos"
						, c3d_cameraData.camProjToWorld( vec4( clipSpace, 1.0_f ) ) );

					auto worldDir = writer.declLocale( "worldDir"
						, normalize( hPos.xyz() / hPos.w() - c3d_cameraData.position ) );
					auto worldPos = writer.declLocale( "worldPos"
						, c3d_cameraData.position + vec3( 0.0_f, c3d_atmosphereData.bottomRadius, 0.0_f ) );

					auto viewHeight = writer.declLocale( "viewHeight"
						, length( worldPos ) );

					auto viewZenithCosAngle = writer.declLocale< sdw::Float >( "viewZenithCosAngle" );
					auto lightViewCosAngle = writer.declLocale< sdw::Float >( "lightViewCosAngle" );
					atmosphereConfig.uvToSkyViewLutParams( viewZenithCosAngle, lightViewCosAngle, viewHeight, uv, targetSize );
					auto lightViewSinAngle = writer.declLocale( "lightViewSinAngle"
						, sqrt( 1.0_f - lightViewCosAngle * lightViewCosAngle ) );

					auto sunDir = writer.declLocale< sdw::Vec3 >( "sunDir" );
					{
						auto upVector = writer.declLocale( "upVector"
							, worldPos / viewHeight );
						auto sunZenithCosAngle = writer.declLocale( "sunZenithCosAngle"
							, dot( upVector, c3d_atmosphereData.sunDirection ) );
						auto sunZenithSinAngle = writer.declLocale( "sunZenithSinAngle"
							, sqrt( 1.0_f - sunZenithCosAngle * sunZenithCosAngle ) );
						sunDir = normalize( vec3( sunZenithSinAngle, sunZenithCosAngle, 0.0_f ) );
					}

					worldPos = vec3( 0.0_f, viewHeight, 0.0_f );

					auto viewZenithSinAngle = writer.declLocale( "viewZenithSinAngle"
						, sqrt( 1.0_f - viewZenithCosAngle * viewZenithCosAngle ) );
					worldDir = vec3( viewZenithSinAngle * lightViewCosAngle
						, viewZenithCosAngle
						, -viewZenithSinAngle * lightViewSinAngle );

					// Move to top atmospehre
					IF( writer, !atmosphereConfig.moveToTopAtmosphere( worldPos, worldDir ) )
					{
						// Ray is not intersecting the atmosphere
						pxl_colour = vec4( 0.0_f, 0.0_f, 0.0_f, 1.0_f );
					}
					ELSE
					{
						auto ss = writer.declLocale( "ss"
							, atmosphereConfig.integrateScatteredLuminance( pixPos
							, worldPos
							, worldDir
							, sunDir
							, sampleCountIni
							, depthBufferValue ) );
						pxl_colour = vec4( ss.luminance, 1.0_f );
					}
					FI;
				} );

			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}
	}

	//************************************************************************************************

	AtmosphereSkyViewPass::AtmosphereSkyViewPass( crg::FramePassGroup & graph
		, crg::FramePassArray const & previousPasses
		, castor3d::RenderDevice const & device
		, CameraUbo const & cameraUbo
		, AtmosphereScatteringUbo const & atmosphereUbo
		, crg::ImageViewId const & transmittanceView
		, crg::ImageViewId const & resultView
		, uint32_t index )
		: castor::Named{ "SkyViewPass" + castor::string::toString( index ) }
		, m_vertexShader{ VK_SHADER_STAGE_VERTEX_BIT, getName(), skyview::getVertexProgram() }
		, m_pixelShader{ VK_SHADER_STAGE_FRAGMENT_BIT, getName(), skyview::getPixelProgram( getExtent( resultView ), getExtent( transmittanceView ) ) }
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
					.instances( renderSize.depth )
					.build( framePass, context, graph );
				device.renderSystem.getEngine()->registerTimer( framePass.getFullName()
					, result->getTimer() );
				return result;
			} );
		pass.addDependencies( previousPasses );
		cameraUbo.createPassBinding( pass
			, skyview::eCamera );
		atmosphereUbo.createPassBinding( pass
			, skyview::eAtmosphere );
		crg::SamplerDesc linearSampler{ VK_FILTER_LINEAR
			, VK_FILTER_LINEAR };
		pass.addSampledView( transmittanceView
			, skyview::eTransmittance
			, VK_IMAGE_LAYOUT_UNDEFINED
			, linearSampler );
		pass.addOutputColourView( resultView );
		m_lastPass = &pass;
	}

	void AtmosphereSkyViewPass::accept( castor3d::BackgroundVisitor & visitor )
	{
		//visitor.visit( m_vertexShader );
		//visitor.visit( m_pixelShader );
	}

	//************************************************************************************************
}
