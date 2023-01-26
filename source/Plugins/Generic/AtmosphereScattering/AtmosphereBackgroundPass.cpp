#include "AtmosphereScattering/AtmosphereBackgroundPass.hpp"

#include "AtmosphereScattering/AtmosphereBackground.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Render/RenderDevice.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Render/RenderTechniqueVisitor.hpp>
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

		static castor3d::ShaderPtr getPixelProgram( VkExtent2D const & renderSize )
		{
			sdw::FragmentWriter writer;

			auto cloudsMap = writer.declCombinedImg< sdw::CombinedImage2DRgba32 >( "cloudsMap"
				, uint32_t( AtmosphereBackgroundPass::eClouds )
				, 0u );

			// Fragment Outputs
			auto outColour( writer.declOutput< sdw::Vec4 >( "outColour", 0u ) );

			writer.implementMainT< sdw::VoidT, sdw::VoidT >( sdw::FragmentIn{ writer }
				, sdw::FragmentOut{ writer }
				, [&]( sdw::FragmentIn in
					, sdw::FragmentOut out )
				{
					auto targetSize = writer.declLocale( "targetSize"
						, vec2( sdw::Float{ float( renderSize.width + 1u ) }, float( renderSize.height + 1u ) ) );
					outColour = cloudsMap.sample( in.fragCoord.xy() / targetSize );
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
		, VkExtent2D const & size )
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

	void AtmosphereBackgroundPass::doResetPipeline( uint32_t index )
	{
		resetCommandBuffer();
		resetPipeline( {}, index );
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
			, atmos::getPixelProgram( size ) };
		castor::DataHolderT< Shaders >::getData().stages =
		{
			makeShaderState( device, castor::DataHolderT< Shaders >::getData().vertexShader ),
			makeShaderState( device, castor::DataHolderT< Shaders >::getData().pixelShader ),
		};
		return ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( castor::DataHolderT< Shaders >::getData().stages );
	}

	//************************************************************************************************
}
