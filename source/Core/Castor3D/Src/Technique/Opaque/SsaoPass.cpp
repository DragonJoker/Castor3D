#include "SsaoPass.hpp"

#include "Engine.hpp"
#include "Scene/Camera.hpp"

using namespace castor;
using namespace castor3d;

namespace castor3d
{
	SsaoPass::SsaoPass( Engine & engine
		, Size const & size
		, SsaoConfig const & config
		, GeometryPassResult const & gpResult
		, Camera const & camera )
		: m_engine{ engine }
		, m_config{ config }
		, m_matrixUbo{ engine }
		, m_ssaoConfigUbo{ engine }
		, m_linearisePass{ engine, size, m_ssaoConfigUbo, *gpResult[size_t( DsTexture::eDepth )], camera.getViewport() }
		, m_rawSsaoPass{ engine, size, config, m_ssaoConfigUbo, m_linearisePass.getResult(), *gpResult[size_t( DsTexture::eData1 )] }
		, m_horizontalBlur{ engine, size, config, m_ssaoConfigUbo, Point2i{ 1, 0 }, m_rawSsaoPass.getResult(), *gpResult[size_t( DsTexture::eData1 )] }
		, m_verticalBlur{ engine, size, config, m_ssaoConfigUbo, Point2i{ 0, 1 }, m_horizontalBlur.getResult(), *gpResult[size_t( DsTexture::eData1 )] }
	{
	}

	SsaoPass::~SsaoPass()
	{
		m_ssaoConfigUbo.cleanup();
	}

	void SsaoPass::render( Camera const & camera )
	{
		m_ssaoConfigUbo.update( m_config, camera );
		m_linearisePass.linearise();
		m_rawSsaoPass.compute();
		m_horizontalBlur.blur();
		m_verticalBlur.blur();
	}

	TextureUnit const & SsaoPass::getResult()const
	{
		return m_verticalBlur.getResult();
	}
}
