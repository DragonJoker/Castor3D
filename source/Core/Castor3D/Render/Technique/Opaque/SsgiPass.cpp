#include "Castor3D/Render/Technique/Opaque/SsgiPass.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Render/Passes/CombinePass.hpp"
#include "Castor3D/Render/Passes/GaussianBlur.hpp"
#include "Castor3D/Render/Technique/Opaque/GeometryPassResult.hpp"
#include "Castor3D/Render/Technique/Opaque/Ssgi/SsgiConfig.hpp"
#include "Castor3D/Render/Technique/Opaque/Ssgi/RawSsgiPass.hpp"

using namespace castor;
using namespace castor3d;

#define C3D_DebugRawPass 0

namespace castor3d
{
	namespace
	{
		std::unique_ptr< ast::Shader > getVertexProgram( RenderSystem & renderSystem )
		{
			using namespace sdw;
			VertexWriter writer;

			// Shader inputs
			Vec2 position = writer.declInput< Vec2 >( "position", 0u );

			// Shader outputs
			auto vtx_texture = writer.declOutput< Vec2 >( "vtx_texture", 0u );
			auto out = writer.getOut();

			writer.implementFunction< sdw::Void >( "main"
				, [&]()
				{
					vtx_texture = ( position + 1.0_f ) / 2.0_f;
					out.vtx.position = vec4( position, 0.0_f, 1.0_f );
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		std::unique_ptr< ast::Shader > getPixelProgram( RenderSystem & renderSystem )
		{
			using namespace sdw;
			FragmentWriter writer;

			// Shader inputs
			auto c3d_mapLhs = writer.declSampledImage< FImg2DRgba32 >( CombinePass::LhsMap, 0u, 0u );
			auto c3d_mapRhs = writer.declSampledImage< FImg2DRgba32 >( CombinePass::RhsMap, 1u, 0u );
			auto vtx_texture = writer.declInput< Vec2 >( "vtx_texture", 0u );

			// Shader outputs
			auto pxl_fragColor = writer.declOutput< Vec4 >( "pxl_fragColor", 0 );

			writer.implementFunction< sdw::Void >( "main"
				, [&]()
				{
					pxl_fragColor = texture( c3d_mapLhs, vtx_texture );
					pxl_fragColor += texture( c3d_mapRhs, vtx_texture );
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}
	}

	SsgiPass::SsgiPass( Engine & engine
		, VkExtent2D const & size
		, SsaoConfig & ssaoConfig
		, SsgiConfig & ssgiConfig
		, TextureLayout const & linearisedDepth
		, TextureLayoutSPtr scene )
		: m_engine{ engine }
		, m_size{ size.width >> 2, size.height >> 2 }
		, m_ssaoConfig{ ssaoConfig }
		, m_ssgiConfig{ ssgiConfig }
		, m_matrixUbo{ engine }
		, m_combineVtx{ VK_SHADER_STAGE_VERTEX_BIT, "SSGI - Combine", getVertexProgram( *engine.getRenderSystem() ) }
		, m_combinePxl{ VK_SHADER_STAGE_FRAGMENT_BIT, "SSGI - Combine", getPixelProgram( *engine.getRenderSystem() ) }
		, m_rawSsgi{ std::make_shared< RawSsgiPass >( engine
			, m_size
			, m_ssgiConfig
			, linearisedDepth
			, scene->getDefaultView() ) }
#if !C3D_DebugRawPass
		, m_gaussianBlur{ std::make_shared< GaussianBlur >( engine
			, cuT( "SSGI" )
			, m_rawSsgi->getResult().getTexture()->getDefaultView()
			, m_size
			, m_rawSsgi->getResult().getTexture()->getPixelFormat()
			, ssgiConfig.blurSize.value().value() ) }
		, m_combine{ std::make_shared< CombinePass >( engine
			, cuT( "SSGI" )
			, scene->getPixelFormat()
			, VkExtent2D{ scene->getWidth(), scene->getHeight() }
			, m_combineVtx
			, m_combinePxl
			, m_rawSsgi->getSceneView().getTexture()->getDefaultView()
			, m_rawSsgi->getResult().getTexture()->getDefaultView()
			, scene ) }
#endif
	{
	}

	SsgiPass::~SsgiPass()
	{
	}

	void SsgiPass::update( Camera const & camera )
	{
		m_ssaoConfig.blurRadius.reset();
	}

	ashes::Semaphore const & SsgiPass::render( ashes::Semaphore const & toWait )const
	{
		auto * result = &toWait;
		result = &m_rawSsgi->compute( *result );
#if !C3D_DebugRawPass
		result = &m_gaussianBlur->blur( *result );
		result = &m_combine->combine( *result );
#endif
		return *result;
	}

	void SsgiPass::accept( RenderTechniqueVisitor & visitor )
	{
		m_rawSsgi->accept( m_ssgiConfig, visitor );
#if !C3D_DebugRawPass
		m_gaussianBlur->accept( visitor );
		m_combine->accept( visitor );
#endif
	}

	TextureLayout const & SsgiPass::getResult()const
	{
#if C3D_DebugRawPass
		return *m_rawSsgi->getResult().getTexture();
#else
		return m_combine->getResult();
#endif
	}
}
