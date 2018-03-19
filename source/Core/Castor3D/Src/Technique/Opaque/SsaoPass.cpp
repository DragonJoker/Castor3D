#include "SsaoPass.hpp"

#include "Engine.hpp"
#include "Scene/Camera.hpp"
#include "Technique/Opaque/GeometryPassResult.hpp"

using namespace castor;
using namespace castor3d;

namespace castor3d
{
	SsaoPass::SsaoPass( Engine & engine
		, renderer::Extent2D const & size
		, SsaoConfig const & config
		, GeometryPassResult const & gpResult
		, Viewport const & viewport )
		: m_engine{ engine }
		, m_config{ config }
		, m_matrixUbo{ engine }
		, m_ssaoConfigUbo{ engine }
		, m_linearisePass{ engine, size, m_ssaoConfigUbo, *gpResult.getViews()[size_t( DsTexture::eDepth )], viewport }
		, m_rawSsaoPass{ engine, size, config, m_ssaoConfigUbo, m_linearisePass.getResult(), *gpResult.getViews()[size_t( DsTexture::eData1 )] }
		, m_horizontalBlur{ engine, size, config, m_ssaoConfigUbo, Point2i{ 1, 0 }, m_rawSsaoPass.getResult(), *gpResult.getViews()[size_t( DsTexture::eData1 )] }
		, m_verticalBlur{ engine, size, config, m_ssaoConfigUbo, Point2i{ 0, 1 }, m_horizontalBlur.getResult(), *gpResult.getViews()[size_t( DsTexture::eData1 )] }
	{
	}

	SsaoPass::~SsaoPass()
	{
		m_ssaoConfigUbo.cleanup();
	}

	void SsaoPass::update( Camera const & camera )
	{
		m_ssaoConfigUbo.update( m_config, camera );
	}

	void SsaoPass::render( renderer::Semaphore const & toWait )const
	{
		m_linearisePass.linearise( toWait );
		m_rawSsaoPass.compute( m_linearisePass.getSemaphore() );
		m_horizontalBlur.blur( m_rawSsaoPass.getSemaphore() );
		m_verticalBlur.blur( m_horizontalBlur.getSemaphore() );
	}

	TextureUnit const & SsaoPass::getResult()const
	{
		return m_verticalBlur.getResult();
	}

	renderer::Semaphore const & SsaoPass::getSemaphore()const
	{
		return m_verticalBlur.getSemaphore();
	}
}
