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

#if defined( near )
#	undef near
#	undef far
#endif

namespace dof
{
	namespace comb
	{
		namespace c3d = castor3d::shader;

		static castor3d::ShaderPtr getProgram( castor3d::RenderDevice const & device )
		{
			sdw::TraditionalGraphicsWriter writer{ &device.renderSystem.getEngine()->getShaderAllocator() };

			auto c3d_mapColour = writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapColour", 0u, 0u );
			auto c3d_mapNear = writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapNear", 1u, 0u );
			auto c3d_mapFar = writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapFar", 2u, 0u );
			C3D_DepthOfField( writer, 3u, 0u );

			writer.implementEntryPointT< c3d::Position2FT, c3d::Uv2FT >( []( sdw::VertexInT< c3d::Position2FT > const & in
				, sdw::VertexOutT< c3d::Uv2FT > out )
				{
					out.uv() = ( in.position() + 1.0_f ) / 2.0_f;
					out.vtx.position = vec4( in.position(), 0.0_f, 1.0_f );
				} );

			writer.implementEntryPointT< c3d::Uv2FT, c3d::Colour4FT >( [&]( sdw::FragmentInT< c3d::Uv2FT > const & in
				, sdw::FragmentOutT< c3d::Colour4FT > out )
				{
					auto near = writer.declLocale( "near"
						, c3d_mapNear.lod( in.uv(), 0.0_f ) );
					auto far = writer.declLocale( "far"
						, c3d_mapFar.lod( in.uv(), 0.0_f ) );
					auto fullResCol = writer.declLocale( "fullResCol"
						, c3d_mapColour.lod( in.uv(), 0.0_f ) );

					auto blendNear = writer.declLocale( "blendNear"
						, min( c3d_dofData.bokehScale() * near.a(), 0.5_f ) * 2.0_f );
					auto blendFar = writer.declLocale( "blendFar"
						, min( c3d_dofData.bokehScale() * far.a(), 0.5_f ) * 2.0_f );

					auto result = writer.declLocale( "result"
						, vec4( 0.0_f, 0.0f, 0.0f, 1.0f ) );
					result.rgb() = mix( fullResCol.rgb(), far.rgb(), vec3( blendFar ) );
					result.rgb() = mix( result.rgb(), near.rgb(), vec3( blendNear ) );

					out.colour() = result;
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
					m_program.programModule = castor3d::ProgramModule{ cuT( "DoF/Combine" ), getProgram( m_device ) };
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

	crg::FramePass const & createCombinePass( castor3d::RenderDevice const & device
		, crg::FramePassGroup & graph
		, crg::FramePassArray const & previousPasses
		, DepthOfFieldUbo const & configurationUbo
		, crg::ImageViewIdArray const & colour
		, castor3d::Texture const & nearBlur
		, castor3d::Texture const & farBlur
		, crg::ImageViewIdArray const & target
		, bool const * enabled
		, uint32_t const * passIndex )
	{
		auto extent = getExtent( target.front() );
		auto & pass = graph.createPass( "Combine"
			, [&device, extent, enabled, passIndex]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				auto result = castor::make_unique< comb::FramePass >( framePass
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

		pass.addSampledView( colour, 0u, crg::SamplerDesc{ VK_FILTER_LINEAR, VK_FILTER_LINEAR } );
		pass.addSampledView( nearBlur.sampledViewId, 1u, crg::SamplerDesc{ VK_FILTER_LINEAR, VK_FILTER_LINEAR } );
		pass.addSampledView( farBlur.sampledViewId, 2u, crg::SamplerDesc{ VK_FILTER_LINEAR, VK_FILTER_LINEAR } );
		configurationUbo.createPassBinding( pass, 3u );

		pass.addOutputColourView( target );

		return pass;
	}
}
