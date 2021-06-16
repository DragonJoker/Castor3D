#include "Castor3D/Render/ToneMapping/ToneMapping.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/PoolUniformBuffer.hpp"
#include "Castor3D/Cache/TargetCache.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Shader/Program.hpp"

#include <RenderGraph/FrameGraph.hpp>
#include <RenderGraph/ImageData.hpp>
#include <RenderGraph/ImageViewData.hpp>
#include <RenderGraph/RunnablePasses/RenderQuad.hpp>

#include <CastorUtils/Graphics/Size.hpp>

#include <ShaderWriter/Source.hpp>

using namespace castor;
using namespace sdw;

namespace castor3d
{
	namespace
	{
		static uint32_t constexpr HdrCfgUboIdx = 0u;
		static uint32_t constexpr HdrMapIdx = 1u;
	}

	ToneMapping::ToneMapping( Engine & engine
		, RenderDevice const & device
		, castor::Size const & size
		, crg::FrameGraph & graph
		, crg::ImageViewId const & source
		, crg::ImageViewId const & target
		, crg::FramePass const & previousPass
		, HdrConfigUbo & hdrConfigUbo
		, Parameters const & parameters )
		: OwnedBy< Engine >{ engine }
		, m_name{ cuT( "linear" ) }
		, m_hdrConfigUbo{ hdrConfigUbo }
		, m_vertexShader{ VK_SHADER_STAGE_VERTEX_BIT, "ToneMapping" }
		, m_pixelShader{ VK_SHADER_STAGE_FRAGMENT_BIT, "ToneMapping" }
		, m_pass{ &doCreatePass( size, graph, source, target, previousPass ) }
	{
	}

	ToneMapping::~ToneMapping()
	{
	}

	void ToneMapping::initialise( castor::String const & name )
	{
		doCreate( name );
	}

	castor::String const & ToneMapping::getFullName()const
	{
		return getEngine()->getRenderTargetCache().getToneMappingName( m_name );
	}

	void ToneMapping::updatePipeline( castor::String const & name )
	{
		if ( name != m_name )
		{
			doCreate( name );
			m_quad->resetPipeline( ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( m_program ) );
		}
	}

	void ToneMapping::accept( ToneMappingVisitor & visitor )
	{
		visitor.visit( m_vertexShader );
		visitor.visit( m_pixelShader );
	}

	crg::FramePass & ToneMapping::doCreatePass( castor::Size const & size
		, crg::FrameGraph & graph
		, crg::ImageViewId const & source
		, crg::ImageViewId const & target
		, crg::FramePass const & previousPass )
	{
		auto & result = graph.createPass( "ToneMapping"
			, [this, size]( crg::FramePass const & pass
				, crg::GraphContext const & context
				, crg::RunnableGraph & graph )
			{
				auto result = crg::RenderQuadBuilder{}
					.renderPosition( {} )
					.renderSize( makeExtent2D( size ) )
					.texcoordConfig( {} )
					.program( ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( m_program ) )
					.build( pass, context, graph );
				getEngine()->registerTimer( "ToneMapping"
					, result->getTimer() );
				return result;
				m_quad = result.get();
				return result;
			} );
		result.addDependency( previousPass );
		m_hdrConfigUbo.createPassBinding( result
			, HdrCfgUboIdx );
		result.addSampledView( source
			, HdrMapIdx
			, {} );
		result.addOutputColourView( target );
		return result;
	}

	void ToneMapping::doCreate( castor::String const & name )
	{
		m_name = name;
		{
			VertexWriter writer;

			// Shader inputs
			auto position = writer.declInput< Vec2 >( "position", 0u );
			auto uv = writer.declInput< Vec2 >( "uv", 1u );

			// Shader outputs
			auto vtx_texture = writer.declOutput< Vec2 >( "vtx_texture", 0u );
			auto out = writer.getOut();

			writer.implementFunction< sdw::Void >( "main"
				, [&]()
				{
					vtx_texture = uv;
					out.vtx.position = vec4( position.x(), position.y(), 0.0_f, 1.0_f );
				} );

			m_vertexShader.shader = std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		m_pixelShader.shader = getEngine()->getRenderTargetCache().getToneMappingFactory().create( name );
		auto & device = *getEngine()->getRenderSystem()->getMainRenderDevice();
		m_program =
		{
			makeShaderState( device, m_vertexShader ),
			makeShaderState( device, m_pixelShader ),
		};
	}
}
