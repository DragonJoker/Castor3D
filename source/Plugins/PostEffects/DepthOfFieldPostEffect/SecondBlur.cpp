#include "DepthOfFieldPostEffect/SecondBlur.hpp"

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
	namespace blur2
	{
		namespace c3ds = c3d::shader;

		static c3d::ShaderPtr getProgram( c3d::RenderDevice const & device )
		{
			sdw::TraditionalGraphicsWriter writer{ &device.renderSystem.getEngine()->getShaderAllocator() };

			auto c3d_mapColour = writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapColour", 0u, 0u );
			C3D_DepthOfField( writer, 1u, 0u );

			writer.implementEntryPointT< c3ds::Position2FT, c3ds::Uv2FT >( []( sdw::VertexInT< c3ds::Position2FT > const & in
				, sdw::VertexOutT< c3ds::Uv2FT > out )
				{
					out.uv() = ( in.position() + 1.0_f ) / 2.0_f;
					out.vtx.position = vec4( in.position(), 0.0_f, 1.0_f );
				} );

			writer.implementEntryPointT< c3ds::Uv2FT, c3ds::Colour4FT >( [&]( sdw::FragmentInT< c3ds::Uv2FT > const & in
				, sdw::FragmentOutT< c3ds::Colour4FT > out )
				{
					auto col = writer.declLocale( "col"
						, c3d_mapColour.lod( in.uv(), 0.0_f ) );
					auto maxVal = writer.declLocale( "maxVal"
						, col.rgb() );
					auto CoC = writer.declLocale( "CoC"
						, col.a() );
					auto sampleStep = writer.declLocale( "sampleStep"
						, vec2( c3d_dofData.pixelStepHalf() * c3d_dofData.bokehScale() * CoC ) );

					sdwFOR( writer, sdw::Int, j, 0_i, j < 16_i, ++j )
					{
						auto sUV = writer.declLocale( "sUV"
							, fma( sampleStep, c3d_dofData.points16()[j].xy(), in.uv() ) );
						auto tap = writer.declLocale( "tap"
							, c3d_mapColour.lod( sUV, 0.0_f ) );

						maxVal = max( tap.rgb(), maxVal );
					}
					sdwROF

					out.colour() = vec4( maxVal.rgb(), CoC );
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
					m_program.programModule = c3d::ProgramModule{ cuT( "DoF/SecondBlur" ), getProgram( m_device ) };
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

	void createSecondBlurPass( c3d::RenderDevice const & device
		, crg::FramePassGroup & graph
		, DepthOfFieldUbo const & configurationUbo
		, c3d::Texture const & firstBlurResult
		, c3d::Texture & blurResult
		, crg::RunnablePass::IsEnabledCallback isEnabled
		, uint32_t const * passIndex )
	{
		auto extent = blurResult.getExtent();
		auto & pass = graph.createPass( "SecondBlur"
			, [&device, extent, isEnabled, passIndex]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				auto result = c3d::makeRawUnique< blur2::FramePass >( framePass
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
		pass.addInputSampled( *firstBlurResult.getSampledLastAttach(), 0u );
		configurationUbo.createPassBinding( pass, 1u );

		blurResult.setLastAttach( pass.addOutputColourTarget( blurResult.getTargetViewId() ) );
	}
}
