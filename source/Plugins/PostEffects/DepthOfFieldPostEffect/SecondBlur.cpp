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
		namespace c3d = castor3d::shader;

		static castor3d::ShaderPtr getProgram( castor3d::RenderDevice const & device )
		{
			sdw::TraditionalGraphicsWriter writer{ &device.renderSystem.getEngine()->getShaderAllocator() };

			auto c3d_mapColour = writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapColour", 0u, 0u );
			C3D_DepthOfField( writer, 1u, 0u );

			writer.implementEntryPointT< c3d::Position2FT, c3d::Uv2FT >( []( sdw::VertexInT< c3d::Position2FT > const & in
				, sdw::VertexOutT< c3d::Uv2FT > out )
				{
					out.uv() = ( in.position() + 1.0_f ) / 2.0_f;
					out.vtx.position = vec4( in.position(), 0.0_f, 1.0_f );
				} );

			writer.implementEntryPointT< c3d::Uv2FT, c3d::Colour4FT >( [&]( sdw::FragmentInT< c3d::Uv2FT > const & in
				, sdw::FragmentOutT< c3d::Colour4FT > out )
				{
					auto col = writer.declLocale( "col"
						, c3d_mapColour.lod( in.uv(), 0.0_f ) );
					auto maxVal = writer.declLocale( "maxVal"
						, col.rgb() );
					auto CoC = writer.declLocale( "CoC"
						, col.a() );
					auto sampleStep = writer.declLocale( "sampleStep"
						, vec2( c3d_dofData.pixelStepHalf() * c3d_dofData.bokehScale() * CoC ) );

					FOR( writer, sdw::Int, j, 0_i, j < 16_i, ++j )
					{
						auto sUV = writer.declLocale( "sUV"
							, fma( sampleStep, c3d_dofData.points16()[j].xy(), in.uv() ) );
						auto tap = writer.declLocale( "tap"
							, c3d_mapColour.lod( sUV, 0.0_f ) );

						maxVal = max( tap.rgb(), maxVal );
					}
					ROF

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
				, castor3d::RenderDevice const & device
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
				castor3d::ProgramModule programModule;
				ashes::PipelineShaderStageCreateInfoArray stages;
			};

		private:
			crg::VkPipelineShaderStageCreateInfoArray doCreateProgram()
			{
				if ( m_program.stages.empty() )
				{
					m_program.programModule = castor3d::ProgramModule{ cuT( "DoF/SecondBlur" ), getProgram( m_device ) };
					m_program.stages = makeProgramStates( m_device, m_program.programModule );
				}

				return ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( m_program.stages );
			}

		private:
			castor3d::RenderDevice const & m_device;
			ProgramData m_program;
		};
	}

	//*********************************************************************************************

	crg::FramePassArray createSecondBlurPass( castor3d::RenderDevice const & device
		, crg::FramePassGroup & graph
		, crg::FramePassArray const & previousPasses
		, DepthOfFieldUbo const & configurationUbo
		, castor3d::Texture const & firstBlurResult
		, castor3d::Texture const & blurResult
		, bool const * enabled
		, uint32_t const * passIndex )
	{
		auto extent = blurResult.getExtent();
		auto & pass = graph.createPass( "SecondBlur"
			, [&device, extent, enabled, passIndex]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				auto result = castor::make_unique< blur2::FramePass >( framePass
					, context
					, graph
					, device
					, crg::rq::Config{}
						.renderSize( castor3d::makeExtent2D( extent ) )
						.enabled( enabled )
						.passIndex( passIndex ) );
				device.renderSystem.getEngine()->registerTimer( castor::makeString( framePass.getFullName() )
					, result->getTimer() );
				return result;
			} );
		pass.addDependencies( previousPasses );

		pass.addSampledView( firstBlurResult.sampledViewId, 0u );
		configurationUbo.createPassBinding( pass, 1u );

		pass.addOutputColourView( blurResult.targetViewId );

		return { &pass };
	}
}
