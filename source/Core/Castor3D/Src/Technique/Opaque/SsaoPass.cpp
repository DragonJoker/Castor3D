#include "SsaoPass.hpp"

#include "Engine.hpp"
#include "Scene/Camera.hpp"

using namespace castor;
using namespace castor3d;

namespace castor3d
{
	SsaoPass::SsaoPass( Engine & engine
		, Size const & size
		, SsaoConfig const & config )
		: m_engine{ engine }
		, m_config{ config }
		, m_matrixUbo{ engine }
		, m_ssaoConfigUbo{ engine }
		, m_linearisePass{ engine, size, m_matrixUbo, m_ssaoConfigUbo }
		, m_rawSsaoPass{ engine, size, config, m_matrixUbo, m_ssaoConfigUbo }
		, m_horizontalBlur{ engine, size, config, m_matrixUbo, m_ssaoConfigUbo, Point2i{ 1, 0 } }
		, m_verticalBlur{ engine, size, config, m_matrixUbo, m_ssaoConfigUbo, Point2i{ 0, 1 } }
	{
	}

	SsaoPass::~SsaoPass()
	{
		m_ssaoConfigUbo.getUbo().cleanup();
	}

	void SsaoPass::render( GeometryPassResult const & gpResult
		, Camera const & camera
		, RenderInfo & info )
	{
		m_ssaoConfigUbo.update( m_config, camera );
		m_linearisePass.linearise( *gpResult[size_t( DsTexture::eDepth )]
			, camera.getViewport() );
		m_rawSsaoPass.compute( m_linearisePass.getResult()
			, *gpResult[size_t( DsTexture::eData1 )] );
		m_horizontalBlur.blur( m_rawSsaoPass.getResult()
			, *gpResult[size_t( DsTexture::eData1 )] );
		m_verticalBlur.blur( m_horizontalBlur.getResult()
			, *gpResult[size_t( DsTexture::eData1 )] );
	}

	TextureUnit const & SsaoPass::getResult()const
	{
		return m_verticalBlur.getResult();
	}
}
