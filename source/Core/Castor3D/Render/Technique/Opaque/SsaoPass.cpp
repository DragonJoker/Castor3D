#include "Castor3D/Render/Technique/Opaque/SsaoPass.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Render/Technique/Opaque/GeometryPassResult.hpp"
#include "Castor3D/Render/Technique/Opaque/Ssao/BlurPass.hpp"
#include "Castor3D/Render/Technique/Opaque/Ssao/SsaoConfig.hpp"
#include "Castor3D/Render/Technique/Opaque/Ssao/SsaoConfigUbo.hpp"
#include "Castor3D/Render/Technique/Opaque/Ssao/RawSsaoPass.hpp"

using namespace castor;
using namespace castor3d;

#define C3D_DebugRawPass 0

namespace castor3d
{
	SsaoPass::SsaoPass( Engine & engine
		, VkExtent2D const & size
		, SsaoConfig & ssaoConfig
		, TextureUnit const & linearisedDepth
		, GeometryPassResult const & gpResult )
		: m_engine{ engine }
		, m_ssaoConfig{ ssaoConfig }
		, m_matrixUbo{ engine }
		, m_ssaoConfigUbo{ std::make_shared< SsaoConfigUbo >( engine ) }
		, m_rawSsaoPass{ std::make_shared< RawSsaoPass >( engine
			, size
			, m_ssaoConfig
			, *m_ssaoConfigUbo
			, linearisedDepth
			, gpResult.getViews()[size_t( DsTexture::eData1 )] ) }
#if !C3D_DebugRawPass
		, m_horizontalBlur{ std::make_shared< SsaoBlurPass >( engine
			, size
			, m_ssaoConfig
			, *m_ssaoConfigUbo
			, Point2i{ 1, 0 }
			, m_rawSsaoPass->getResult()
			, gpResult.getViews()[size_t( DsTexture::eData1 )] ) }
		, m_verticalBlur{ std::make_shared< SsaoBlurPass >( engine
			, size
			, m_ssaoConfig
			, *m_ssaoConfigUbo
			, Point2i{ 0, 1 }
			, m_horizontalBlur->getResult()
			, gpResult.getViews()[size_t( DsTexture::eData1 )] ) }
#endif
	{
	}

	SsaoPass::~SsaoPass()
	{
		m_ssaoConfigUbo->cleanup();
		m_ssaoConfigUbo.reset();
	}

	void SsaoPass::update( Camera const & camera )
	{
		m_ssaoConfigUbo->update( m_ssaoConfig, camera );
#if !C3D_DebugRawPass
		m_horizontalBlur->update();
		m_verticalBlur->update();
#endif
		m_ssaoConfig.blurRadius.reset();
	}

	ashes::Semaphore const & SsaoPass::render( ashes::Semaphore const & toWait )const
	{
		auto * result = &toWait;
		result = &m_rawSsaoPass->compute( *result );
#if !C3D_DebugRawPass
		result = &m_horizontalBlur->blur( *result );
		result = &m_verticalBlur->blur( *result );
#endif
		return *result;
	}

	void SsaoPass::accept( RenderTechniqueVisitor & visitor )
	{
		m_rawSsaoPass->accept( m_ssaoConfig, visitor );
#if !C3D_DebugRawPass
		m_horizontalBlur->accept( true, m_ssaoConfig, visitor );
		m_verticalBlur->accept( false, m_ssaoConfig, visitor );
#endif
	}

	TextureUnit const & SsaoPass::getResult()const
	{
#if C3D_DebugRawPass
		return m_rawSsaoPass->getResult();
#else
		return m_verticalBlur->getResult();
#endif
	}
}
