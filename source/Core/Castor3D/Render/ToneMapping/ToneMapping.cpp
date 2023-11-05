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

CU_ImplementSmartPtr( castor3d, ToneMapping )

namespace castor3d
{
	namespace rendtonmap
	{
		static uint32_t constexpr HdrCfgUboIdx = 0u;
		static uint32_t constexpr HdrMapIdx = 1u;
	}

	ToneMapping::ToneMapping( Engine & engine
		, RenderDevice const & device
		, castor::Size const & size
		, crg::FrameGraph & graph
		, crg::ImageViewIdArray const & source
		, crg::ImageViewId const & target
		, crg::FramePass const & previousPass
		, HdrConfigUbo & hdrConfigUbo
		, Parameters const & parameters
		, ProgressBar * progress )
		: OwnedBy< Engine >{ engine }
		, m_name{ cuT( "linear" ) }
		, m_hdrConfigUbo{ hdrConfigUbo }
		, m_shader{ "ToneMapping" }
		, m_source{ source.front() }
		, m_pass{ &doCreatePass( size, graph, source, target, previousPass, progress ) }
	{
	}

	void ToneMapping::initialise( castor::String const & name
		, crg::ImageViewId const & source )
	{
		doCreate( name );
		doUpdatePassIndex( source );
	}

	void ToneMapping::update( CpuUpdater & updater
		, crg::ImageViewId const & source )
	{
		doUpdatePassIndex( source );
	}

	castor::String const & ToneMapping::getFullName()const
	{
		return getEngine()->getRenderTargetCache().getToneMappingName( m_name );
	}

	void ToneMapping::updatePipeline( castor::String const & name )
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

	crg::FramePass & ToneMapping::doCreatePass( castor::Size const & size
		, crg::FrameGraph & graph
		, crg::ImageViewIdArray const & source
		, crg::ImageViewId const & target
		, crg::FramePass const & previousPass
		, ProgressBar * progress )
	{
		auto & result = graph.createPass( "ToneMapping"
			, [this, progress, target]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				stepProgressBar( progress, "Initialising tone mapping pass" );
				auto result = crg::RenderQuadBuilder{}
					.renderPosition( {} )
					.renderSize( makeExtent2D( getExtent( target ) ) )
					.texcoordConfig( {} )
					.passIndex( &m_passIndex )
					.program( ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( m_program ) )
					.build( framePass, context, graph, crg::ru::Config{ 2u } );
				getEngine()->registerTimer( framePass.getFullName()
					, result->getTimer() );
				m_quad = result.get();
				return result;
			} );
		result.addDependency( previousPass );
		m_hdrConfigUbo.createPassBinding( result
			, rendtonmap::HdrCfgUboIdx );
		result.addSampledView( source
			, rendtonmap::HdrMapIdx );
		result.addOutputColourView( target );
		return result;
	}

	void ToneMapping::getVertexProgram( ast::ShaderBuilder & builder )
	{
		sdw::VertexWriter writer{ builder };
		writer.implementMainT< shader::PosUv2FT, shader::Uv2FT >( [&]( sdw::VertexInT< shader::PosUv2FT > in
			, sdw::VertexOutT< shader::Uv2FT > out )
			{
				out.uv() = in.uv();
				out.vtx.position = vec4( in.position().x(), in.position().y(), 0.0_f, 1.0_f );
			} );
	}

	void ToneMapping::doCreate( castor::String const & name )
	{
		m_name = name;
		ast::ShaderBuilder builder{ ast::ShaderStage::eTraditionalGraphics
			, &getEngine()->getShaderAllocator() };
		castor3d::ToneMapping::getVertexProgram( builder );
		getEngine()->getToneMappingFactory().create( name, builder );
		m_shader.shader = builder.releaseShader();
		auto & device = getEngine()->getRenderSystem()->getRenderDevice();
		m_program = makeProgramStates( device, m_shader );
	}

	void ToneMapping::doUpdatePassIndex( crg::ImageViewId const & source )
	{
		m_passIndex = ( source == m_source ) ? 1u : 0u;
	}
}
