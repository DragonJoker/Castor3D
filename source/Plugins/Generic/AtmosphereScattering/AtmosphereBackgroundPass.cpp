#include "AtmosphereScattering/AtmosphereBackgroundPass.hpp"

#include "AtmosphereScattering/Atmosphere.hpp"
#include "AtmosphereScattering/AtmosphereBackground.hpp"
#include "AtmosphereScattering/AtmosphereScatteringUbo.hpp"
#include "AtmosphereScattering/Scattering.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Render/RenderDevice.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Render/Technique/RenderTechniqueVisitor.hpp>
#include <Castor3D/Shader/Program.hpp>
#include <Castor3D/Shader/Ubos/MatrixUbo.hpp>
#include <Castor3D/Shader/Ubos/SceneUbo.hpp>

#include <RenderGraph/RunnableGraph.hpp>
#include <RenderGraph/RunnablePasses/RenderQuad.hpp>

#include <ShaderWriter/Source.hpp>

namespace atmosphere_scattering
{
	//*********************************************************************************************

	namespace atmos
	{
		castor::String const Name{ "Atmosphere" };

		static castor3d::ShaderPtr getVertexProgram()
		{
			sdw::VertexWriter writer;
			sdw::Vec2 position = writer.declInput< sdw::Vec2 >( "position", 0u );

			writer.implementMainT< sdw::VoidT, sdw::VoidT >( sdw::VertexIn{ writer }
				, sdw::VertexOut{ writer }
				, [&]( sdw::VertexIn in
					, sdw::VertexOut out )
				{
					out.vtx.position = vec4( position, 1.0_f, 1.0_f );
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		static castor3d::ShaderPtr getPixelProgram( VkExtent2D const & renderSize
			, VkExtent3D const & transmittanceExtent
			, bool colorTransmittance = false
			, bool fastSky = true
			, bool fastAerialPerspective = true
			, bool renderSunDisk = true )
		{
			sdw::FragmentWriter writer;

			C3D_Camera( writer
				, AtmosphereBackgroundPass::eCamera
				, 0u );
			C3D_AtmosphereScattering( writer
				, AtmosphereBackgroundPass::eAtmosphere
				, 0u );
			auto transmittanceMap = writer.declCombinedImg< sdw::CombinedImage2DRgba32 >( "transmittanceMap"
				, uint32_t( AtmosphereBackgroundPass::eTransmittance )
				, 0u );
			auto multiScatterMap = writer.declCombinedImg< sdw::CombinedImage2DRgba32 >( "multiScatterMap"
				, uint32_t( AtmosphereBackgroundPass::eMultiScatter )
				, 0u );
			auto skyViewMap = writer.declCombinedImg< sdw::CombinedImage2DRgba32 >( "skyViewMap"
				, uint32_t( AtmosphereBackgroundPass::eSkyView )
				, 0u );
			auto volumeMap = writer.declCombinedImg< sdw::CombinedImage3DRgba32 >( "volumeMap"
				, uint32_t( AtmosphereBackgroundPass::eVolume )
				, 0u );

			// Fragment Outputs
			auto outLuminance( writer.declOutput< sdw::Vec4 >( "outLuminance", 0u ) );
			auto outTransmittance( writer.declOutput< sdw::Vec4 >( "outTransmittance", 1u, colorTransmittance ) );

			AtmosphereConfig atmosphereConfig{ writer
				, c3d_atmosphereData
				, { false, &c3d_cameraData, true, true }
				, { transmittanceExtent.width, transmittanceExtent.height }
				, &transmittanceMap };
			ScatteringConfig scatteringConfig{ writer
				, atmosphereConfig
				, c3d_cameraData
				, c3d_atmosphereData
				, colorTransmittance
				, fastSky
				, fastAerialPerspective
				, renderSunDisk };

			writer.implementMainT< sdw::VoidT, sdw::VoidT >( sdw::FragmentIn{ writer }
				, sdw::FragmentOut{ writer }
				, [&]( sdw::FragmentIn in
					, sdw::FragmentOut out )
				{
					outTransmittance = vec4( 0.0_f, 0.0_f, 0.0_f, 1.0_f );
					auto targetSize = writer.declLocale( "targetSize"
						, vec2( sdw::Float{ float( renderSize.width + 1u ) }, float( renderSize.height + 1u ) ) );
					auto depthBufferValue = writer.declLocale( "depthBufferValue"
						, 1.0_f );
					auto luminance = writer.declLocale< sdw::Vec4 >( "luminance" );
					auto transmittance = writer.declLocale< sdw::Vec4 >( "transmittance" );
					scatteringConfig.getPixelTransLum( in.fragCoord.xy()
						, targetSize
						, depthBufferValue
						, transmittanceMap
						, skyViewMap
						, volumeMap
						, transmittance
						, luminance );
					outLuminance = scatteringConfig.rescaleLuminance( luminance );
					outTransmittance = transmittance;
				} );

			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}
	}

	//************************************************************************************************

	AtmosphereBackgroundPass::AtmosphereBackgroundPass( crg::FramePass const & pass
		, crg::GraphContext & context
		, crg::RunnableGraph & graph
		, castor3d::RenderDevice const & device
		, AtmosphereBackground & background
		, VkExtent2D const & size
		, crg::ImageViewId const * depth )
		: castor3d::BackgroundPassBase{ pass
			, context
			, graph
			, device
			, background }
		, crg::RenderQuad{ pass
			, context
			, graph
			, crg::ru::Config{ 1u, true }
			, crg::rq::Config{}
				.isEnabled( IsEnabledCallback( [this](){ return castor3d::BackgroundPassBase::doIsEnabled(); } ) )
				.renderSize( size )
				.depthStencilState( ashes::PipelineDepthStencilStateCreateInfo{ 0u, VK_TRUE, VK_FALSE, VK_COMPARE_OP_LESS_OR_EQUAL } )
				.program( doInitialiseShader( device, background, size ) ) }
	{
	}

	void AtmosphereBackgroundPass::doResetPipeline()
	{
		resetCommandBuffer();
		resetPipeline( {} );
		reRecordCurrent();
	}

	crg::VkPipelineShaderStageCreateInfoArray AtmosphereBackgroundPass::doInitialiseShader( castor3d::RenderDevice const & device
		, AtmosphereBackground & background
		, VkExtent2D const & size )
	{
		castor::DataHolderT< Shaders >::getData().vertexShader = { VK_SHADER_STAGE_VERTEX_BIT
			, atmos::Name
			, atmos::getVertexProgram() };
		castor::DataHolderT< Shaders >::getData().pixelShader = { VK_SHADER_STAGE_FRAGMENT_BIT
			, atmos::Name
			, atmos::getPixelProgram( size, background.getTransmittance().getExtent() ) };
		castor::DataHolderT< Shaders >::getData().stages =
		{
			makeShaderState( device, castor::DataHolderT< Shaders >::getData().vertexShader ),
			makeShaderState( device, castor::DataHolderT< Shaders >::getData().pixelShader ),
		};
		return ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( castor::DataHolderT< Shaders >::getData().stages );
	}

	//************************************************************************************************
}
