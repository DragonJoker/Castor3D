#include "Castor3D/Render/ToneMapping/ToneMapping.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/PoolUniformBuffer.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Shader/Program.hpp"

#include <RenderGraph/FrameGraph.hpp>
#include <RenderGraph/ImageData.hpp>
#include <RenderGraph/ImageViewData.hpp>
#include <RenderGraph/RenderQuad.hpp>

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

		rq::BindingDescriptionArray createBindings()
		{
			return rq::BindingDescriptionArray
			{
				{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, ashes::nullopt },	// HdrConfig UBO
				{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_IMAGE_VIEW_TYPE_2D },	// HDR Map
			};
		}
	}

	ToneMapping::ToneMapping( castor::String const & name
		, castor::String const & fullName
		, Engine & engine
		, RenderDevice const & device
		, castor::Size const & size
		, crg::FrameGraph & graph
		, crg::ImageViewId const & source
		, crg::ImageViewId const & target
		, crg::FramePass const & previousPass
		, HdrConfigUbo & hdrConfigUbo
		, Parameters const & parameters )
		: OwnedBy< Engine >{ engine }
		, Named{ name }
		, m_hdrConfigUbo{ hdrConfigUbo }
		, m_fullName{ fullName }
		, m_vertexShader{ VK_SHADER_STAGE_VERTEX_BIT, "ToneMapping" }
		, m_pixelShader{ VK_SHADER_STAGE_FRAGMENT_BIT, "ToneMapping" }
		, m_pass{ &doCreatePass( size, graph, source, target, previousPass ) }
	{
	}

	ToneMapping::~ToneMapping()
	{
	}

	bool ToneMapping::initialise()
	{
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

		m_pixelShader.shader = doCreate();
		auto & device = *getEngine()->getRenderSystem()->getMainRenderDevice();
		m_program =
		{
			makeShaderState( device, m_vertexShader ),
			makeShaderState( device, m_pixelShader ),
		};
		return true;
	}

	void ToneMapping::cleanup()
	{
		doDestroy();
	}

	void ToneMapping::update( CpuUpdater & updater )
	{
		doCpuUpdate();
	}

	void ToneMapping::update( GpuUpdater & updater )
	{
		doGpuUpdate();
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
		auto & result = graph.createPass( m_fullName
			, [this, size]( crg::FramePass const & pass
				, crg::GraphContext const & context
				, crg::RunnableGraph & graph )
			{
				return crg::RenderQuadBuilder{}
					.renderPosition( {} )
					.renderSize( makeExtent2D( size ) )
					.texcoordConfig( {} )
					.program( ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( m_program ) )
					.build( pass, context, graph );
			} );
		result.addDependency( previousPass );
		m_hdrConfigUbo.createPassBinding( result
			, HdrCfgUboIdx );
		result.addSampledView( source
			, HdrMapIdx
			, {}
			, VK_FILTER_LINEAR );
		result.addOutputColourView( target );
		return result;
	}
}
