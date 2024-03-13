#include "DepthOfFieldPostEffect/ComputeCircleOfConfusion.hpp"

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
		namespace c3d = castor3d::shader;

		static castor3d::ShaderPtr getProgram( castor3d::RenderDevice const & device )
		{
			sdw::TraditionalGraphicsWriter writer{ &device.renderSystem.getEngine()->getShaderAllocator() };

			auto c3d_mapColour = writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapColour", 0u, 0u );
			auto c3d_mapCoC = writer.declCombinedImg< FImg2DR32 >( "c3d_mapCoC", 1u, 0u );
			C3D_DepthOfField( writer, 2u, 0u );

			writer.implementEntryPointT< c3d::Position2FT, c3d::Uv2FT >( []( sdw::VertexInT< c3d::Position2FT > const & in
				, sdw::VertexOutT< c3d::Uv2FT > out )
				{
					out.uv() = ( in.position() + 1.0_f ) / 2.0_f;
					out.vtx.position = vec4( in.position(), 0.0_f, 1.0_f );
				} );

			writer.implementEntryPointT< c3d::Uv2FT, c3d::Colour4FT >( [&]( sdw::FragmentInT< c3d::Uv2FT > const & in
				, sdw::FragmentOutT< c3d::Colour4FT > out )
				{
					auto acc = writer.declLocale( "acc"
						, vec3( 0.0_f, 0.0f, 0.0f ) );
					auto CoC = writer.declLocale( "CoC"
						, c3d_mapCoC.lod( in.uv(), 0.0_f ) );
					auto sampleStep = writer.declLocale( "sampleStep"
						, c3d_dofData.pixelStepFull() * vec2( c3d_dofData.bokehScale() * CoC ) );

					FOR ( writer, sdw::Int, j, 0_i, j < 64_i, ++j )
					{
						auto sUV = writer.declLocale( "sUV"
							, fma( sampleStep, c3d_dofData.points64()[j].xy(), in.uv() ) );
						auto tap = writer.declLocale( "tap"
							, c3d_mapColour.lod( sUV, 0.0_f ) );

						acc += tap.rgb();
					}
					ROF

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
					m_program.programModule = castor3d::ProgramModule{ cuT( "DoF/FirstBlur" ), getProgram( m_device ) };
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

	crg::FramePassArray createFirstBlurPass( castor3d::RenderDevice const & device
		, crg::FramePassGroup & graph
		, crg::FramePassArray const & previousPasses
		, DepthOfFieldUbo const & configurationUbo
		, crg::ImageViewIdArray const & colour
		, castor3d::Texture const & cocResult
		, castor3d::Texture const & blurResult
		, bool const * enabled
		, uint32_t const * passIndex )
	{
		auto extent = blurResult.getExtent();
		auto & pass = graph.createPass( "FirstBlur"
			, [&device, extent, enabled, passIndex]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				auto result = castor::make_unique< blur1::FramePass >( framePass
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

		pass.addSampledView( colour, 0u );
		pass.addSampledView( cocResult.sampledViewId, 1u );
		configurationUbo.createPassBinding( pass, 2u );

		pass.addOutputColourView( blurResult.targetViewId );

		return { &pass };
	}
}