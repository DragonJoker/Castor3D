#include "AtmosphereScattering/AtmosphereTransmittancePass.hpp"

#include "AtmosphereScattering/AtmosphereBackground.hpp"
#include "AtmosphereScattering/AtmosphereModel.hpp"
#include "AtmosphereScattering/AtmosphereScatteringUbo.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Render/RenderDevice.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Render/RenderTechniqueVisitor.hpp>
#include <Castor3D/Shader/Program.hpp>
#include <Castor3D/Shader/Shaders/GlslUtils.hpp>

#include <RenderGraph/RunnableGraph.hpp>
#include <RenderGraph/RunnablePasses/RenderQuad.hpp>

#include <ShaderWriter/Source.hpp>

#include <ashespp/Buffer/Buffer.hpp>

namespace atmosphere_scattering
{
	//*********************************************************************************************

	namespace transmittance
	{
		enum Bindings : uint32_t
		{
			eAtmosphere,
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

			C3D_AtmosphereScattering( writer
				, eAtmosphere
				, 0u );
			auto pxl_colour( writer.declOutput< sdw::Vec4 >( "pxl_colour", 0 ) );

			auto sampleCountIni = writer.declConstant( "sampleCountIni"
				, 40.0_f );	// Can go a low as 10 sample but energy lost starts to be visible.
			auto depthBufferValue = writer.declConstant( "depthBufferValue"
				, -1.0_f );

			AtmosphereModel atmosphere{ writer
				, c3d_atmosphereData
				, AtmosphereModel::Settings{ castor::Length::fromUnit( 1.0f, engine.getLengthUnit() ) } };

			writer.implementMainT< sdw::VoidT, sdw::VoidT >( sdw::FragmentIn{ writer }
				, sdw::FragmentOut{ writer }
				, [&]( sdw::FragmentIn in
					, sdw::FragmentOut out )
				{
					auto targetSize = writer.declLocale( "targetSize"
						, vec2( sdw::Float{ float( renderSize.width + 1u ) }, float( renderSize.height + 1u ) ) );
					auto pixPos = writer.declLocale( "pixPos"
						, in.fragCoord.xy() );

					// Compute camera position from LUT coords
					auto uv = writer.declLocale( "uv"
						, pixPos / targetSize );
					auto viewHeight = writer.declLocale< sdw::Float > ( "viewHeight" );
					auto viewZenithCosAngle = writer.declLocale< sdw::Float >( "viewZenithCosAngle" );
					atmosphere.uvToLutTransmittanceParams( viewHeight, viewZenithCosAngle, uv );
					auto viewZenithSinAngle = writer.declLocale< sdw::Float >( "viewZenithSinAngle"
						, sqrt( 1.0_f - viewZenithCosAngle * viewZenithCosAngle ) );

					auto ray = writer.declLocale< Ray >( "ray" );
					ray.origin = vec3( 0.0_f, viewHeight, 0.0_f );
					ray.direction = vec3( 0.0_f, viewZenithCosAngle, -viewZenithSinAngle );

					auto transmittance = writer.declLocale( "transmittance"
						, exp( -atmosphere.integrateScatteredLuminance( pixPos
							, ray
							, c3d_atmosphereData.sunDirection()
							, sampleCountIni
							, depthBufferValue ).opticalDepth() ) );

					// Optical depth to transmittance
					pxl_colour = vec4( transmittance, 1.0f );
				} );

			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}
	}

	//************************************************************************************************

	AtmosphereTransmittancePass::AtmosphereTransmittancePass( crg::FramePassGroup & graph
		, crg::FramePassArray const & previousPasses
		, castor3d::RenderDevice const & device
		, AtmosphereScatteringUbo const & atmosphereUbo
		, crg::ImageViewId const & resultView
		, bool const & enabled )
		: m_vertexShader{ VK_SHADER_STAGE_VERTEX_BIT, "TransmittancePass", transmittance::getVertexProgram() }
		, m_pixelShader{ VK_SHADER_STAGE_FRAGMENT_BIT, "TransmittancePass", transmittance::getPixelProgram( *device.renderSystem.getEngine(), getExtent( resultView ) ) }
		, m_stages{ makeShaderState( device, m_vertexShader )
			, makeShaderState( device, m_pixelShader ) }
	{
		auto renderSize = getExtent( resultView );
		auto & pass = graph.createPass( "TransmittancePass"
			, [this, &device, &enabled, renderSize]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				auto result = crg::RenderQuadBuilder{}
					.renderSize( { renderSize.width, renderSize.height } )
					.program( ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( m_stages ) )
					.enabled( &enabled )
					.build( framePass, context, graph );
				device.renderSystem.getEngine()->registerTimer( framePass.getFullName()
					, result->getTimer() );
				return result;
			} );
		pass.addDependencies( previousPasses );
		atmosphereUbo.createPassBinding( pass
			, transmittance::eAtmosphere );
		pass.addOutputColourView( resultView );
		m_lastPass = &pass;
	}

	void AtmosphereTransmittancePass::accept( castor3d::PipelineVisitor & visitor )
	{
		visitor.visit( m_vertexShader );
		visitor.visit( m_pixelShader );
	}

	//************************************************************************************************
}
