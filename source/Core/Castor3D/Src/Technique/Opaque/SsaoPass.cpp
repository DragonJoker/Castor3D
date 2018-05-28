#include "SsaoPass.hpp"

#include "Engine.hpp"
#include "Scene/Camera.hpp"
#include "Technique/Opaque/GeometryPassResult.hpp"
#include "Technique/Opaque/Ssao/BlurPass.hpp"
#include "Technique/Opaque/Ssao/LineariseDepthPass.hpp"
#include "Technique/Opaque/Ssao/SsaoConfig.hpp"
#include "Technique/Opaque/Ssao/SsaoConfigUbo.hpp"
#include "Technique/Opaque/Ssao/RawSsaoPass.hpp"

using namespace castor;
using namespace castor3d;

namespace castor3d
{
	SsaoPass::SsaoPass( Engine & engine
		, renderer::Extent2D const & size
		, SsaoConfig & config
		, GeometryPassResult const & gpResult
		, Viewport const & viewport )
		: m_engine{ engine }
		, m_config{ config }
		, m_matrixUbo{ engine }
		, m_ssaoConfigUbo{ std::make_shared< SsaoConfigUbo >( engine ) }
		, m_linearisePass{ std::make_shared< LineariseDepthPass >( engine
			, size
			, *m_ssaoConfigUbo
			, *gpResult.getViews()[size_t( DsTexture::eDepth )]
			, viewport ) }
		, m_rawSsaoPass{ std::make_shared< RawSsaoPass >( engine
			, size
			, config
			, *m_ssaoConfigUbo
			, m_linearisePass->getResult()
			, *gpResult.getViews()[size_t( DsTexture::eData1 )] )}
		, m_horizontalBlur{ std::make_shared< SsaoBlurPass >( engine
			, size
			, config
			, *m_ssaoConfigUbo
			, Point2i{ 1, 0 }
			, m_rawSsaoPass->getResult()
			, *gpResult.getViews()[size_t( DsTexture::eData1 )] ) }
		, m_verticalBlur{ std::make_shared< SsaoBlurPass >( engine
			, size
			, config
			, *m_ssaoConfigUbo
			, Point2i{ 0, 1 }
			, m_horizontalBlur->getResult()
			, *gpResult.getViews()[size_t( DsTexture::eData1 )] ) }
	{
	}

	SsaoPass::~SsaoPass()
	{
		m_ssaoConfigUbo->cleanup();
		m_ssaoConfigUbo.reset();
	}

	void SsaoPass::update( Camera const & camera )
	{
		m_ssaoConfigUbo->update( m_config, camera );
	}

	renderer::Semaphore const & SsaoPass::render( renderer::Semaphore const & toWait )const
	{
		auto * result = &toWait;
		result = &m_linearisePass->linearise( *result );
		result = &m_rawSsaoPass->compute( *result );
		result = &m_horizontalBlur->blur( *result );
		result = &m_verticalBlur->blur( *result );
		return *result;
	}

	void SsaoPass::accept( RenderTechniqueVisitor & visitor )
	{
		m_linearisePass->accept( visitor );
		m_rawSsaoPass->accept( m_config, visitor );
		m_horizontalBlur->accept( true, m_config, visitor );
		m_verticalBlur->accept( false, m_config, visitor );
	}

	TextureUnit const & SsaoPass::getResult()const
	{
		return m_verticalBlur->getResult();
	}
}
