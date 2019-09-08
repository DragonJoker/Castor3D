#include "Castor3D/HDR/ToneMapping.hpp"

#include "Castor3D/Engine.hpp"

#include "Castor3D/HDR/HdrConfig.hpp"
#include "Castor3D/RenderToTexture/RenderQuad.hpp"
#include "Castor3D/Render/RenderPassTimer.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/Ubos/HdrConfigUbo.hpp"
#include "Castor3D/Texture/TextureLayout.hpp"

#include <ashespp/Descriptor/DescriptorSet.hpp>
#include <ashespp/Descriptor/DescriptorSetLayout.hpp>
#include "Castor3D/Shader/GlslToSpv.hpp"

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
		, RenderQuad{ getCurrentRenderDevice( engine ), true, false }
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
		m_signalFinished = getCurrentRenderDevice( m_device )->createSemaphore();

		{
			VertexWriter writer;

			// Shader inputs
			auto position = writer.declInput< Vec2 >( cuT( "position" ), 0u );
			auto uv = writer.declInput< Vec2 >( cuT( "uv" ), 1u );

			// Shader outputs
			auto vtx_texture = writer.declOutput< Vec2 >( cuT( "vtx_texture" ), 0u );
			auto out = writer.getOut();

			writer.implementFunction< sdw::Void >( cuT( "main" )
				, [&]()
				{
					vtx_texture = uv;
					out.gl_out.gl_Position = vec4( position.x(), position.y(), 0.0, 1.0 );
				} );

			m_vertexShader.shader = std::make_unique< sdw::Shader >( std::move( writer.getShader() ) );
		}

		m_pixelShader.shader = doCreate();
		auto & device = getCurrentRenderDevice( m_device );
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
		visitor.visit( cuT( "ToneMapping" )
			, VK_SHADER_STAGE_VERTEX_BIT
			, *m_vertexShader.shader );
		visitor.visit( cuT( "ToneMapping" )
			, VK_SHADER_STAGE_FRAGMENT_BIT
			, *m_pixelShader.shader );
		visitor.visit( cuT( "ToneMapping" )
			, VK_SHADER_STAGE_FRAGMENT_BIT
			, m_config );
	}

	void ToneMapping::doFillDescriptorSet( ashes::DescriptorSetLayout & descriptorSetLayout
		, ashes::DescriptorSet & descriptorSet )
	{
		descriptorSet.createBinding( descriptorSetLayout.getBinding( 0u )
			, m_hdrConfigUbo.getUbo()
			, 0u
			, 1u );
	}
}
