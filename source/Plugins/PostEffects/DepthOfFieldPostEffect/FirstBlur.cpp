#include "DepthOfFieldPostEffect/FirstBlur.hpp"

#include "DepthOfFieldPostEffect/DepthOfFieldUbo.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Render/RenderDevice.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Shader/Program.hpp>
#include <Castor3D/Shader/Shaders/GlslBaseIO.hpp>
#include <Castor3D/Shader/Shaders/GlslUtils.hpp>

#include <CastorUtils/Graphics/Image.hpp>

#include <ShaderWriter/Source.hpp>
#include <ShaderWriter/TraditionalGraphicsWriter.hpp>

#include <RenderGraph/RunnablePasses/RenderQuad.hpp>

#include <numeric>

namespace dof
{
	namespace blur1
	{
		namespace c3ds = c3d::shader;

		static c3d::ShaderPtr getProgram( c3d::RenderDevice const & device )
		{
			sdw::TraditionalGraphicsWriter writer{ &device.renderSystem.getEngine()->getShaderAllocator() };

			auto c3d_mapColour = writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapColour", 0u, 0u );
			auto c3d_mapCoC = writer.declCombinedImg< FImg2DR32 >( "c3d_mapCoC", 1u, 0u );
			C3D_DepthOfField( writer, 2u, 0u );

			writer.implementEntryPointT< c3ds::Position2FT, c3ds::Uv2FT >( []( sdw::VertexInT< c3ds::Position2FT > const & in
				, sdw::VertexOutT< c3ds::Uv2FT > out )
				{
					out.uv() = ( in.position() + 1.0_f ) / 2.0_f;
					out.vtx.position = vec4( in.position(), 0.0_f, 1.0_f );
				} );

			writer.implementEntryPointT< c3ds::Uv2FT, c3ds::Colour4FT >( [&]( sdw::FragmentInT< c3ds::Uv2FT > const & in
				, sdw::FragmentOutT< c3ds::Colour4FT > out )
				{
					auto acc = writer.declLocale( "acc"
						, vec3( 0.0_f, 0.0f, 0.0f ) );
					auto CoC = writer.declLocale( "CoC"
						, c3d_mapCoC.lod( in.uv(), 0.0_f ) );
					auto sampleStep = writer.declLocale( "sampleStep"
						, c3d_dofData.pixelStepFull() * vec2( c3d_dofData.bokehScale() * CoC ) );

					sdwFOR( writer, sdw::Int, j, 0_i, j < 64_i, ++j )
					{
						auto sUV = writer.declLocale( "sUV"
							, fma( sampleStep, c3d_dofData.points64()[j].xy(), in.uv() ) );
						auto tap = writer.declLocale( "tap"
							, c3d_mapColour.lod( sUV, 0.0_f ) );

						acc += tap.rgb();
					}
					sdwROF

					acc /= vec3( 64.0_f );
					out.colour() = vec4( acc.rgb(), CoC );
				} );

			return writer.getBuilder().releaseShader();
		}

		class FramePass
			: public crg::RenderQuad
		{
		public:
			FramePass( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph
				, c3d::RenderDevice const & device
				, crg::rq::Config config )
				: crg::RenderQuad{ framePass, context, graph
					, crg::ru::Config{ 2u }
					, config
						.programCreator( { 1u, [this]( uint32_t ) { return doCreateProgram(); } } ) }
				, m_device{ device }
			{
			}

		private:
			struct ProgramData
			{
				c3d::ProgramModule programModule;
				ashes::PipelineShaderStageCreateInfoArray stages;
			};

		private:
			crg::VkPipelineShaderStageCreateInfoArray doCreateProgram()
			{
				if ( m_program.stages.empty() )
				{
					m_program.programModule = c3d::ProgramModule{ cuT( "DoF/FirstBlur" ), getProgram( m_device ) };
					m_program.stages = makeProgramStates( m_device, m_program.programModule );
				}

				return ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( m_program.stages );
			}

		private:
			c3d::RenderDevice const & m_device;
			ProgramData m_program;
		};
	}

	//*********************************************************************************************

	void createFirstBlurPass( c3d::RenderDevice const & device
		, crg::FramePassGroup & graph
		, DepthOfFieldUbo const & configurationUbo
		, c3d::Texture const & colour
		, c3d::Texture const & cocResult
		, c3d::Texture & blurResult
		, crg::RunnablePass::IsEnabledCallback isEnabled
		, uint32_t const * passIndex )
	{
		auto extent = blurResult.getExtent();
		auto & pass = graph.createPass( "FirstBlur"
			, [&device, extent, isEnabled, passIndex]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				auto result = c3d::makeRawUnique< blur1::FramePass >( framePass
					, context
					, graph
					, device
					, crg::rq::Config{}
						.renderSize( c3d::makeExtent2D( extent ) )
						.isEnabled( isEnabled )
						.passIndex( passIndex ) );
				device.renderSystem.getEngine()->registerTimer( c3d::makeString( framePass.getFullName() )
					, result->getTimer() );
				return result;
			} );
		pass.addInputSampled( *colour.getSampledLastAttach(), 0u );
		pass.addInputSampled( *cocResult.getSampledLastAttach(), 1u );
		configurationUbo.createPassBinding( pass, 2u );

		blurResult.setLastAttach( pass.addOutputColourTarget( blurResult.getTargetViewId() ) );
	}
}
