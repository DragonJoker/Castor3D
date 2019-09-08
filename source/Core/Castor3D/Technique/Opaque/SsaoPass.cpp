#include "Castor3D/Technique/Opaque/SsaoPass.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Technique/Opaque/GeometryPassResult.hpp"
#include "Castor3D/Technique/Opaque/Ssao/BlurPass.hpp"
#include "Castor3D/Technique/Opaque/Ssao/LineariseDepthPass.hpp"
#include "Castor3D/Technique/Opaque/Ssao/SsaoConfig.hpp"
#include "Castor3D/Technique/Opaque/Ssao/SsaoConfigUbo.hpp"
#include "Castor3D/Technique/Opaque/Ssao/RawSsaoPass.hpp"

using namespace castor;
using namespace castor3d;

#define C3D_DebugLinearisePass 0
#define C3D_DebugRawPass 0

namespace castor3d
{
	SsaoPass::SsaoPass( Engine & engine
		, VkExtent2D const & size
		, SsaoConfig & config
		, GeometryPassResult const & gpResult )
		: m_engine{ engine }
		, m_config{ config }
		, m_matrixUbo{ engine }
		, m_ssaoConfigUbo{ std::make_shared< SsaoConfigUbo >( engine ) }
		, m_linearisePass{ std::make_shared< LineariseDepthPass >( engine
			, size
			, *m_ssaoConfigUbo
			, gpResult.getViews()[size_t( DsTexture::eDepth )] ) }
#if !C3D_DebugLinearisePass
		, m_rawSsaoPass{ std::make_shared< RawSsaoPass >( engine
			, size
			, config
			, *m_ssaoConfigUbo
			, m_linearisePass->getResult()
			, gpResult.getViews()[size_t( DsTexture::eData1 )] ) }
#	if !C3D_DebugRawPass
		, m_horizontalBlur{ std::make_shared< SsaoBlurPass >( engine
			, size
			, config
			, *m_ssaoConfigUbo
			, Point2i{ 1, 0 }
			, m_rawSsaoPass->getResult()
			, gpResult.getViews()[size_t( DsTexture::eData1 )] ) }
		, m_verticalBlur{ std::make_shared< SsaoBlurPass >( engine
			, size
			, config
			, *m_ssaoConfigUbo
			, Point2i{ 0, 1 }
			, m_horizontalBlur->getResult()
			, gpResult.getViews()[size_t( DsTexture::eData1 )] ) }
#	endif
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
		m_ssaoConfigUbo->update( m_config, camera );
		m_linearisePass->update( camera.getViewport() );
#if !C3D_DebugLinearisePass
#	if !C3D_DebugRawPass
		m_horizontalBlur->update();
		m_verticalBlur->update();
#	endif
#endif
		m_config.blurRadius.reset();
	}

	ashes::Semaphore const & SsaoPass::render( ashes::Semaphore const & toWait )const
	{
		auto * result = &toWait;
		result = &m_linearisePass->linearise( *result );
#if !C3D_DebugLinearisePass
		result = &m_rawSsaoPass->compute( *result );
#	if !C3D_DebugRawPass
		result = &m_horizontalBlur->blur( *result );
		result = &m_verticalBlur->blur( *result );
#	endif
#endif
		return *result;
	}

	void SsaoPass::accept( RenderTechniqueVisitor & visitor )
	{
		m_linearisePass->accept( visitor );
#if !C3D_DebugLinearisePass
		m_rawSsaoPass->accept( m_config, visitor );
#	if !C3D_DebugRawPass
		m_horizontalBlur->accept( true, m_config, visitor );
		m_verticalBlur->accept( false, m_config, visitor );
#	endif
#endif
	}

	TextureUnit const & SsaoPass::getResult()const
	{
#if C3D_DebugLinearisePass
		return m_linearisePass->getResult();
#elif C3D_DebugRawPass
		return m_rawSsaoPass->getResult();
#else
		return m_verticalBlur->getResult();
#endif
	}
}
