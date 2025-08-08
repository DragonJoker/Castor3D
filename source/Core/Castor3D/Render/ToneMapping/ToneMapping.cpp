#include "Castor3D/Render/ToneMapping/ToneMapping.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/PoolUniformBuffer.hpp"
#include "Castor3D/Cache/TargetCache.hpp"
#include "Castor3D/Miscellaneous/ProgressBar.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Shader/Program.hpp"
#include <Castor3D/Shader/Shaders/GlslBaseIO.hpp>

#include <RenderGraph/FrameGraph.hpp>
#include <RenderGraph/ImageData.hpp>
#include <RenderGraph/ImageViewData.hpp>
#include <RenderGraph/RunnablePasses/RenderQuad.hpp>

#include <CastorUtils/Graphics/Size.hpp>

#include <ShaderWriter/Source.hpp>

CU_ImplementSmartPtr( c3d, ToneMapping )

namespace c3d
{
	namespace rendtonmap
	{
		static uint32_t constexpr HdrCfgUboIdx = 0u;
		static uint32_t constexpr ClrGrdUboIdx = 1u;
		static uint32_t constexpr HdrMapIdx = 2u;
	}

	ToneMapping::ToneMapping( Engine & engine
		, crg::FramePassGroup & graph
		, Texture const & source
		, Texture & target
		, RenderUbo const & renderUbo
		, ColourGradingUbo & colourGradingUbo
		, ProgressBar * progress )
		: OwnedBy< Engine >{ engine }
		, m_renderUbo{ renderUbo }
		, m_colourGradingUbo{ colourGradingUbo }
		, m_source{ source }
	{
		auto & pass = graph.createPass( "ToneMapping"
			, [this, progress, &target]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				stepProgressBarLocal( progress, cuT( "Initialising tone mapping pass" ) );
				auto result = crg::RenderQuadBuilder{}
					.renderPosition( {} )
					.renderSize( makeExtent2D( target.getExtent() ) )
					.texcoordConfig( {} )
					.passIndex( &m_passIndex )
					.program( ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( m_program ) )
					.build( framePass, context, graph, crg::ru::Config{ 2u } );
				getEngine()->registerTimer( makeString( framePass.getFullName() )
					, result->getTimer() );
				m_quad = result.get();
				return result;
			} );
		m_renderUbo.createPassBinding( pass, rendtonmap::HdrCfgUboIdx );
		m_colourGradingUbo.createPassBinding( pass, rendtonmap::ClrGrdUboIdx );
		pass.addInputSampled( *source.getSampledLastAttach(), rendtonmap::HdrMapIdx );
		target.setLastAttach( pass.addOutputColourTarget( target.getTargetViewId() ) );
	}

	void ToneMapping::initialise( String const & name
		, Texture const & source )
	{
		doCreate( name );
		doUpdatePassIndex( source );
	}

	void ToneMapping::update( CpuUpdater & updater
		, Texture const & source )
	{
		doUpdatePassIndex( source );
	}

	String const & ToneMapping::getFullName()const
	{
		return getEngine()->getRenderTargetCache().getToneMappingName( m_name );
	}

	void ToneMapping::updatePipeline( String const & name )
	{
		if ( name != m_name
			&& m_quad )
		{
			doCreate( name );
			m_quad->resetPipeline( ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( m_program )
				, m_passIndex );
		}
	}

	void ToneMapping::accept( ToneMappingVisitor & visitor )
	{
		visitor.visit( m_shader );
	}

	void ToneMapping::getVertexProgram( ast::ShaderBuilder & builder )
	{
		sdw::VertexWriter writer{ builder };
		writer.implementMainT< shader::PosUv2FT, shader::Uv2FT >( []( sdw::VertexInT< shader::PosUv2FT > const & in
			, sdw::VertexOutT< shader::Uv2FT > out )
			{
				out.uv() = in.uv();
				out.vtx.position = vec4( in.position().x(), in.position().y(), 0.0_f, 1.0_f );
			} );
	}

	void ToneMapping::doCreate( String const & name )
	{
		m_name = name;
		ast::ShaderBuilder builder{ ast::ShaderStage::eTraditionalGraphics
			, &getEngine()->getShaderAllocator() };
		ToneMapping::getVertexProgram( builder );
		getEngine()->getToneMappingFactory().create( name, builder );
		m_shader.shader = builder.releaseShader();
		auto const & device = getEngine()->getRenderSystem()->getRenderDevice();
		m_program = makeProgramStates( device, m_shader );
	}

	void ToneMapping::doUpdatePassIndex( Texture const & source )
	{
		m_passIndex = ( &source == &m_source ) ? 1u : 0u;
	}
}
