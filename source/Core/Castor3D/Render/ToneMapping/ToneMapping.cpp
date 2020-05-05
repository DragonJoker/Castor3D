#include "Castor3D/Render/ToneMapping/ToneMapping.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"

#include <CastorUtils/Graphics/Size.hpp>

#include <ShaderWriter/Source.hpp>

using namespace castor;
using namespace sdw;

namespace castor3d
{
	namespace
	{
		static uint32_t constexpr HdrCfgUboIdx = 0u;
	}

	ToneMapping::ToneMapping( castor::String const & name
		, castor::String const & fullName
		, Engine & engine
		, HdrConfig & config
		, Parameters const & parameters )
		: OwnedBy< Engine >{ engine }
		, Named{ name }
		, RenderQuad{ *engine.getRenderSystem(), VK_FILTER_NEAREST, TexcoordConfig{} }
		, m_config{ config }
		, m_fullName{ fullName }
		, m_hdrConfigUbo{ engine }
		, m_vertexShader{ VK_SHADER_STAGE_VERTEX_BIT, "ToneMapping" }
		, m_pixelShader{ VK_SHADER_STAGE_FRAGMENT_BIT, "ToneMapping" }
	{
	}

	ToneMapping::~ToneMapping()
	{
	}

	bool ToneMapping::initialise( Size const & size
		, TextureLayout const & source
		, ashes::RenderPass const & renderPass )
	{
		m_hdrConfigUbo.initialise();
		auto & device = getCurrentRenderDevice( m_renderSystem );
		m_signalFinished = device->createSemaphore( m_fullName );

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
		ashes::PipelineShaderStageCreateInfoArray program
		{
			makeShaderState( device, m_vertexShader ),
			makeShaderState( device, m_pixelShader ),
		};
		ashes::VkDescriptorSetLayoutBindingArray bindings
		{
			makeDescriptorSetLayoutBinding( HdrCfgUboIdx
				, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
				, VK_SHADER_STAGE_FRAGMENT_BIT ),
		};
		createPipeline( { size[0], size[1] }
			, Position{}
			, program
			, source.getDefaultView()
			, renderPass
			, std::move( bindings )
			, {} );
		return true;
	}

	void ToneMapping::cleanup()
	{
		doDestroy();
		m_hdrConfigUbo.cleanup();
	}

	void ToneMapping::update()
	{
		m_hdrConfigUbo.update( m_config );
		doUpdate();
	}

	void ToneMapping::accept( ToneMappingVisitor & visitor )
	{
		visitor.visit( m_vertexShader );
		visitor.visit( m_pixelShader );
		visitor.visit( m_vertexShader.name
			, VK_SHADER_STAGE_FRAGMENT_BIT
			, m_config );
	}

	void ToneMapping::doFillDescriptorSet( ashes::DescriptorSetLayout & descriptorSetLayout
		, ashes::DescriptorSet & descriptorSet )
	{
		descriptorSet.createSizedBinding( descriptorSetLayout.getBinding( 0u )
			, *m_hdrConfigUbo.getUbo().buffer
			, m_hdrConfigUbo.getUbo().offset
			, 1u );
	}
}
