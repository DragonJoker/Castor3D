#include "DepthOfFieldPostEffect/ComputeCircleOfConfusion.hpp"

#include "DepthOfFieldPostEffect/DepthOfFieldUbo.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Render/RenderDevice.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Shader/Program.hpp>
#include <Castor3D/Shader/Shaders/GlslBaseIO.hpp>

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
	namespace coc
	{
		namespace c3d = castor3d::shader;
		template< sdw::var::Flag FlagT >
		using CoCResultStructT = sdw::IOStructInstanceHelperT< FlagT
			, "C3D_CoCResult"
			, sdw::IOFloatField< "near", 0u >
			, sdw::IOFloatField< "far", 1u > >;

		template< sdw::var::Flag FlagT >
		struct CoCResultT
			: public CoCResultStructT< FlagT >
		{
			CoCResultT( sdw::ShaderWriter & writer
				, sdw::expr::ExprPtr expr
				, bool enabled )
				: CoCResultStructT< FlagT >{ writer, castor::move( expr ), enabled }
			{
			}

			auto near()const{ return this->template getMember< "near" >(); }
			auto far()const{ return this->template getMember< "far" >(); }
		};


		static castor3d::ShaderPtr getProgram( castor3d::RenderDevice const & device )
		{
			sdw::TraditionalGraphicsWriter writer{ &device.renderSystem.getEngine()->getShaderAllocator() };

			auto c3d_mapDepth = writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapDepth", 0u, 0u );
			auto c3d_mapColour = writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapColour", 1u, 0u );
			C3D_DepthOfField( writer, 2u, 0u );

			writer.implementEntryPointT< c3d::Position2FT, c3d::Uv2FT >( []( sdw::VertexInT< c3d::Position2FT > const & in
				, sdw::VertexOutT< c3d::Uv2FT > out )
				{
					out.uv() = ( in.position() + 1.0_f ) / 2.0_f;
					out.vtx.position = vec4( in.position(), 0.0_f, 1.0_f );
				} );

			writer.implementEntryPointT< c3d::Uv2FT, CoCResultT >( [&]( sdw::FragmentInT< c3d::Uv2FT > const & in
				, sdw::FragmentOutT< CoCResultT > out )
				{
					auto depth = writer.declLocale( "depth"
						, c3d_mapDepth.lod( in.uv(), 0.0_f ).g() ); // G is linearised depth

					auto signedDist = writer.declLocale( "signedDist"
						, depth - c3d_dofData.focalDistance() );
					auto magnitude = writer.declLocale( "magnitude"
						, smoothStep( 0.0_f, c3d_dofData.focalLength(), abs( signedDist ) ) );
					auto CoC = writer.declLocale( "CoC"
						, magnitude );

					auto col = writer.declLocale( "col"
						, c3d_mapColour.lod( in.uv(), 0.0_f ).rgb() );

					out.near() = step( signedDist, 0.0_f ) * CoC;
					out.far() = step( 0.0_f, signedDist ) * CoC;
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
					m_program.programModule = castor3d::ProgramModule{ cuT( "DoF/ComputeCoC" ), getProgram( m_device ) };
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

	crg::FramePassArray createComputeCircleOfConfusionPass( castor3d::RenderDevice const & device
		, crg::FramePassGroup & graph
		, crg::FramePassArray const & previousPasses
		, DepthOfFieldUbo const & configurationUbo
		, castor3d::Texture const & depth
		, crg::ImageViewIdArray const & colour
		, castor3d::Texture const & nearCoC
		, castor3d::Texture const & farCoC
		, bool const * enabled
		, uint32_t const * passIndex )
	{
		auto extent = nearCoC.getExtent();
		auto & pass =graph.createPass( "ComputeCoC"
			, [&device, extent, enabled, passIndex]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				auto result = castor::make_unique< coc::FramePass >( framePass
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

		pass.addSampledView( depth.sampledViewId, 0u );
		pass.addSampledView( colour, 1u );
		configurationUbo.createPassBinding( pass, 2u );

		pass.addOutputColourView( nearCoC.targetViewId );
		pass.addOutputColourView( farCoC.targetViewId );

		return { &pass };
	}
}
