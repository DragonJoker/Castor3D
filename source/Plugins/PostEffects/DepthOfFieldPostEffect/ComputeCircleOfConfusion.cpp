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
		namespace c3ds = c3d::shader;
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
				: CoCResultStructT< FlagT >{ writer, c3d::move( expr ), enabled }
			{
			}

			auto near()const{ return this->template getMember< "near" >(); }
			auto far()const{ return this->template getMember< "far" >(); }
		};


		static c3d::ShaderPtr getProgram( c3d::RenderDevice const & device )
		{
			sdw::TraditionalGraphicsWriter writer{ &c3d::getEngine( device ).getShaderAllocator() };

			auto c3d_mapDepth = writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapDepth", 0u, 0u );
			auto c3d_mapColour = writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapColour", 1u, 0u );
			C3D_DepthOfField( writer, 2u, 0u );

			writer.implementEntryPointT< c3ds::Position2FT, c3ds::Uv2FT >( []( sdw::VertexInT< c3ds::Position2FT > const & in
				, sdw::VertexOutT< c3ds::Uv2FT > out )
				{
					out.uv() = ( in.position() + 1.0_f ) / 2.0_f;
					out.vtx.position = vec4( in.position(), 0.0_f, 1.0_f );
				} );

			writer.implementEntryPointT< c3ds::Uv2FT, CoCResultT >( [&]( sdw::FragmentInT< c3ds::Uv2FT > const & in
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

					sdwIF( writer, c3d_dofData.enableFarBlur() )
					{
						out.far() = step( 0.0_f, signedDist ) * CoC;
					}
					sdwFI
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
					m_program.programModule = c3d::ProgramModule{ cuT( "DoF/ComputeCoC" ), getProgram( m_device ) };
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

	void createComputeCircleOfConfusionPass( c3d::RenderDevice const & device
		, crg::FramePassGroup & graph
		, DepthOfFieldUbo const & configurationUbo
		, c3d::Texture const & depth
		, c3d::Texture const & colour
		, c3d::Texture & nearCoC
		, c3d::Texture & farCoC
		, bool const * enabled
		, uint32_t const * passIndex )
	{
		auto extent = nearCoC.getExtent();
		auto & pass =graph.createPass( "ComputeCoC"
			, [&device, extent, enabled, passIndex]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				auto result = c3d::makeRawUnique< coc::FramePass >( framePass
					, context
					, graph
					, device
					, crg::rq::Config{}
						.renderSize( c3d::makeExtent2D( extent ) )
						.enabled( enabled )
						.passIndex( passIndex ) );
				c3d::getEngine( device ).registerTimer( c3d::makeString( framePass.getFullName() )
					, result->getTimer() );
				return result;
			} );
		pass.addInputSampled( *depth.getSampledLastAttach(), 0u );
		pass.addInputSampled( *colour.getSampledLastAttach(), 1u );
		configurationUbo.createPassBinding( pass, 2u );

		nearCoC.setLastAttach( pass.addOutputColourTarget( nearCoC.getTargetViewId() ) );
		farCoC.setLastAttach( pass.addOutputColourTarget( farCoC.getTargetViewId() ) );
	}
}
