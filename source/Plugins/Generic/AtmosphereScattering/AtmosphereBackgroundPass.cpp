#include "AtmosphereScattering/AtmosphereBackgroundPass.hpp"

#include "AtmosphereScattering/AtmosphereBackground.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Miscellaneous/makeVkType.hpp>
#include <Castor3D/Render/RenderDevice.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Render/RenderTechniqueVisitor.hpp>
#include <Castor3D/Shader/Program.hpp>
#include <Castor3D/Shader/Shaders/GlslBaseIO.hpp>
#include <Castor3D/Shader/Ubos/RenderUbo.hpp>
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
		c3d::String const Name{ cuT( "Atmosphere" ) };

		static c3d::ShaderPtr getProgram( c3d::Engine & engine
			, c3d::Extent2D const & renderSize
			, bool isVisible )
		{
			sdw::TraditionalGraphicsWriter writer{ &engine.getShaderAllocator() };

			C3D_Scene( writer, AtmosphereBackgroundBindings::eScene, 0u );
			C3D_Render( writer, AtmosphereBackgroundBindings::eRenderConfig, 0u );
			auto cloudsMap = writer.declCombinedImg< sdw::CombinedImage2DRgba16 >( "cloudsMap", AtmosphereBackgroundBindings::eClouds, 0u );

			writer.implementEntryPointT< c3ds::Position2FT, sdw::VoidT >( []( sdw::VertexInT< c3ds::Position2FT > const & in
				, sdw::VertexOut out )
				{
					out.vtx.position = vec4( in.position(), 1.0_f, 1.0_f );
				} );

			writer.implementEntryPointT< sdw::VoidT, c3ds::Colour4FT >( [&writer, &cloudsMap, &c3d_renderData, &c3d_sceneData
				, isVisible, &renderSize]( sdw::FragmentIn const & in
				, sdw::FragmentOutT< c3ds::Colour4FT > const & out )
				{
					if ( isVisible )
					{
						auto targetSize = writer.declLocale( "targetSize"
							, vec2( sdw::Float{ float( renderSize.width ) }, float( renderSize.height ) ) );
						out.colour() = cloudsMap.sample( in.fragCoord.xy() / targetSize );
					}
					else
					{
						out.colour() = vec4( c3d_sceneData.getBackgroundColour( c3d_renderData ).xyz(), 1.0_f );
					}
				} );

			return writer.getBuilder().releaseShader();
		}
	}

	//************************************************************************************************

	AtmosphereBackgroundPass::AtmosphereBackgroundPass( crg::FramePass const & pass
		, crg::GraphContext & context
		, crg::RunnableGraph & graph
		, c3d::RenderDevice const & device
		, AtmosphereBackground & background
		, c3d::Extent2D const & size
		, bool forceVisible )
		: c3d::BackgroundPassBase{ pass
			, device
			, background
			, forceVisible }
		, crg::RenderQuad{ pass
			, context
			, graph
			, crg::ru::Config{ 2u, true }
			, crg::rq::Config{}
				.isEnabled( IsEnabledCallback( [this](){ return c3d::BackgroundPassBase::doIsEnabled(); } ) )
				.renderSize( size )
				.depthStencilState( c3d::makeVkStruct< VkPipelineDepthStencilStateCreateInfo >( 0u, VK_TRUE, VK_FALSE, VK_COMPARE_OP_GREATER_OR_EQUAL ) )
				.passIndex( &background.getPassIndex( forceVisible ) )
				.programCreator( { 2u
					, [size, this, &device]( uint32_t programIndex )
					{
						return crg::makeVkArray< VkPipelineShaderStageCreateInfo >( doInitialiseShader( device, size, programIndex ) );
					} } ) }
	{
	}

	void AtmosphereBackgroundPass::doResetPipeline( uint32_t index )
	{
		resetCommandBuffer( index );
		resetPipeline( {}, index );
		reRecordCurrent();
	}

	crg::VkPipelineShaderStageCreateInfoArray AtmosphereBackgroundPass::doInitialiseShader( c3d::RenderDevice const & device
		, c3d::Extent2D const & size
		, uint32_t passIndex )
	{
		auto & engine = c3d::getEngine( device );
		c3d::DataHolderT< Shaders >::getData().shader = { atmos::Name
			, atmos::getProgram( engine, size, passIndex == 0u ) };
		c3d::DataHolderT< Shaders >::getData().stages = c3d::makeProgramStates( device, c3d::DataHolderT< Shaders >::getData().shader );
		return ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( c3d::DataHolderT< Shaders >::getData().stages );
	}

	//************************************************************************************************
}
