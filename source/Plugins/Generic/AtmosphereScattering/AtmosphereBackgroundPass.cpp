#include "AtmosphereScattering/AtmosphereBackgroundPass.hpp"

#include "AtmosphereScattering/AtmosphereBackground.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Miscellaneous/makeVkType.hpp>
#include <Castor3D/Render/RenderDevice.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Render/RenderTechniqueVisitor.hpp>
#include <Castor3D/Shader/Program.hpp>
#include <Castor3D/Shader/Ubos/HdrConfigUbo.hpp>
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
			, bool isVisible )
		{
			sdw::FragmentWriter writer;

			C3D_Scene( writer, AtmosphereBackgroundPass::eScene, 0u );
			C3D_HdrConfig( writer, AtmosphereBackgroundPass::eHdrConfig, 0u );
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
					if ( isVisible )
					{
						auto targetSize = writer.declLocale( "targetSize"
							, vec2( sdw::Float{ float( renderSize.width + 1u ) }, float( renderSize.height + 1u ) ) );
						outColour = cloudsMap.sample( in.fragCoord.xy() / targetSize );
					}
					else
					{
						outColour = vec4( c3d_sceneData.getBackgroundColour( c3d_hdrConfigData ).xyz(), 1.0_f );
					}
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
		, crg::ImageViewIdArray const & colour
		, bool forceVisible )
		: castor3d::BackgroundPassBase{ pass
			, context
			, graph
			, device
			, background
			, forceVisible }
		, crg::RenderQuad{ pass
			, context
			, graph
			, crg::ru::Config{ 2u, true }
			, crg::rq::Config{}
				.isEnabled( IsEnabledCallback( [this](){ return castor3d::BackgroundPassBase::doIsEnabled(); } ) )
				.renderSize( size )
				.depthStencilState( castor3d::makeVkStruct< VkPipelineDepthStencilStateCreateInfo >( 0u, VK_TRUE, VK_FALSE, VK_COMPARE_OP_GREATER_OR_EQUAL ) )
				.passIndex( &background.getPassIndex( forceVisible ) )
				.programCreator( { 2u
					, [size, this, &background, &device]( uint32_t programIndex )
					{
						return crg::makeVkArray< VkPipelineShaderStageCreateInfo >( doInitialiseShader( device, background, size, programIndex ) );
					} } ) }
	{
	}

	void AtmosphereBackgroundPass::doResetPipeline( uint32_t index )
	{
		resetCommandBuffer( index );
		resetPipeline( {}, index );
		reRecordCurrent();
	}

	crg::VkPipelineShaderStageCreateInfoArray AtmosphereBackgroundPass::doInitialiseShader( castor3d::RenderDevice const & device
		, AtmosphereBackground & background
		, VkExtent2D const & size
		, uint32_t passIndex )
	{
		castor::DataHolderT< Shaders >::getData().vertexShader = { VK_SHADER_STAGE_VERTEX_BIT
			, atmos::Name
			, atmos::getVertexProgram() };
		castor::DataHolderT< Shaders >::getData().pixelShader = { VK_SHADER_STAGE_FRAGMENT_BIT
			, atmos::Name
			, atmos::getPixelProgram( size, passIndex == 0u ) };
		castor::DataHolderT< Shaders >::getData().stages =
		{
			makeShaderState( device, castor::DataHolderT< Shaders >::getData().vertexShader ),
			makeShaderState( device, castor::DataHolderT< Shaders >::getData().pixelShader ),
		};
		return ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( castor::DataHolderT< Shaders >::getData().stages );
	}

	//************************************************************************************************
}
