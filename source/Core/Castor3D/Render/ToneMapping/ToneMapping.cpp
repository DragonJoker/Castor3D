#include "Castor3D/Render/ToneMapping/ToneMapping.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/PoolUniformBuffer.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Shader/Program.hpp"

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
		, HdrConfigUbo & hdrConfigUbo
		, Parameters const & parameters )
		: OwnedBy< Engine >{ engine }
		, RenderQuad{ device
			, name
			, VK_FILTER_NEAREST
			, { createBindings()
				, ashes::nullopt
				, rq::Texcoord{} } }
		, m_hdrConfigUbo{ hdrConfigUbo }
		, m_fullName{ fullName }
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
		m_signalFinished = m_device->createSemaphore( m_fullName );

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
			makeShaderState( m_device, m_vertexShader ),
			makeShaderState( m_device, m_pixelShader ),
		};
		createPipelineAndPass( { size[0], size[1] }
			, Position{}
			, program
			, renderPass
			, {
				makeDescriptorWrite( m_hdrConfigUbo.getUbo()
					, HdrCfgUboIdx ),
				makeDescriptorWrite( source.getDefaultView().getSampledView()
					, m_sampler->getSampler()
					, HdrMapIdx ),
			}
			, {} );
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
}
