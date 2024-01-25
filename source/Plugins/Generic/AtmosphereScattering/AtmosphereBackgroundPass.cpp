#include "AtmosphereScattering/AtmosphereBackgroundPass.hpp"

#include "AtmosphereScattering/AtmosphereBackground.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Miscellaneous/makeVkType.hpp>
#include <Castor3D/Render/RenderDevice.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Render/RenderTechniqueVisitor.hpp>
#include <Castor3D/Shader/Program.hpp>
#include <Castor3D/Shader/Shaders/GlslBaseIO.hpp>
#include <Castor3D/Shader/Ubos/HdrConfigUbo.hpp>
#include <Castor3D/Shader/Ubos/SceneUbo.hpp>

#include <RenderGraph/RunnableGraph.hpp>
#include <RenderGraph/RunnablePasses/RenderQuad.hpp>

#include <ShaderWriter/Source.hpp>
#include <ShaderWriter/TraditionalGraphicsWriter.hpp>

namespace atmosphere_scattering
{
	//*********************************************************************************************

	namespace atmos
	{
		castor::String const Name{ cuT( "Atmosphere" ) };

		static castor3d::ShaderPtr getProgram( castor3d::Engine & engine
			, VkExtent2D const & renderSize
			, bool isVisible )
		{
			sdw::TraditionalGraphicsWriter writer{ &engine.getShaderAllocator() };

			C3D_Scene( writer, AtmosphereBackgroundPass::eScene, 0u );
			C3D_HdrConfig( writer, AtmosphereBackgroundPass::eHdrConfig, 0u );
			auto cloudsMap = writer.declCombinedImg< sdw::CombinedImage2DRgba32 >( "cloudsMap"
				, uint32_t( AtmosphereBackgroundPass::eClouds )
				, 0u );

			writer.implementEntryPointT< c3d::Position2FT, sdw::VoidT >( [&]( sdw::VertexInT< c3d::Position2FT > in
				, sdw::VertexOut out )
				{
					out.vtx.position = vec4( in.position(), 1.0_f, 1.0_f );
				} );

			writer.implementEntryPointT< sdw::VoidT, c3d::Colour4FT >( [&]( sdw::FragmentIn in
				, sdw::FragmentOutT< c3d::Colour4FT > out )
				{
					if ( isVisible )
					{
						auto targetSize = writer.declLocale( "targetSize"
							, vec2( sdw::Float{ float( renderSize.width + 1u ) }, float( renderSize.height + 1u ) ) );
						out.colour() = cloudsMap.sample( in.fragCoord.xy() / targetSize );
					}
					else
					{
						out.colour() = vec4( c3d_sceneData.getBackgroundColour( c3d_hdrConfigData ).xyz(), 1.0_f );
					}
				} );

			return writer.getBuilder().releaseShader();
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
		auto & engine = *device.renderSystem.getEngine();
		castor::DataHolderT< Shaders >::getData().shader = { atmos::Name
			, atmos::getProgram( engine, size, passIndex == 0u ) };
		castor::DataHolderT< Shaders >::getData().stages = castor3d::makeProgramStates( device, castor::DataHolderT< Shaders >::getData().shader );
		return ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( castor::DataHolderT< Shaders >::getData().stages );
	}

	//************************************************************************************************
}
