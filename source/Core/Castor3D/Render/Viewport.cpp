#include "Castor3D/Render/Viewport.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Render/RenderSystem.hpp"

CU_ImplementCUSmartPtr( castor3d, Viewport )

using namespace castor;

namespace castor3d
{
	namespace
	{
		castor::Matrix4x4f getSafeBandedOrtho( RenderSystem const & renderSystem
			, castor::Size const & size
			, float left
			, float right
			, float bottom
			, float top
			, float nearZ
			, float farZ )
		{
			auto bandsSize = double( getSafeBandsSize( size ) );
			auto width = size.getWidth() + bandsSize;
			auto height = size.getHeight() + bandsSize;
			castor::Point2d bandRatio{ width / size.getWidth(), height / size.getHeight() };
			return renderSystem.getOrtho( left * bandRatio->x
				, right * bandRatio->x
				, bottom * bandRatio->y
				, top * bandRatio->y
				, nearZ
				, farZ );
		}

		castor::Matrix4x4f getSafeBandedFrustum( RenderSystem const & renderSystem
			, castor::Size const & size
			, float left
			, float right
			, float bottom
			, float top
			, float nearZ
			, float farZ )
		{
			auto bandsSize = double( getSafeBandsSize( size ) );
			auto width = size.getWidth() + bandsSize;
			auto height = size.getHeight() + bandsSize;
			castor::Point2d bandRatio{ width / size.getWidth(), height / size.getHeight() };
			return renderSystem.getFrustum( left * bandRatio->x
				, right * bandRatio->x
				, bottom * bandRatio->y
				, top * bandRatio->y
				, nearZ
				, farZ );
		}

		castor::Matrix4x4f getSafeBandedPerspective( RenderSystem const & renderSystem
			, castor::Size const & size
			, Angle fovY
			, float aspect
			, float nearZ
			, float farZ )
		{
			auto opp = size.getHeight() / 2.0;
			auto adj = opp / fovY.tand();
			auto bandSize = double( getSafeBandSize( size ) );
			auto halfHeight = opp + bandSize;
			auto halfWidth = std::ceil( aspect * opp ) + bandSize;
			return renderSystem.getPerspective( fovY + ( castor::atanf( ( bandSize * 2.85f / 4.0f ) / adj ) )
				, halfWidth / halfHeight
				, nearZ
				, farZ );
		}
	}

	const std::array< String, size_t( ViewportType::eCount ) > Viewport::TypeName
	{
		cuT( "ortho" ),
		cuT( "perspective" ),
		cuT( "frustum" )
	};

	Viewport::Viewport( Engine const & engine
		, ViewportType type
		, castor::Angle const & fovY
		, float aspect
		, float left
		, float right
		, float bottom
		, float top
		, float nearZ
		, float farZ )
		: m_engine{ engine }
		, m_type{ type }
		, m_size{}
		, m_modified{ true }
		, m_fovY{ m_modified, fovY }
		, m_ratio{ m_modified, aspect }
		, m_left{ m_modified, left }
		, m_right{ m_modified, right }
		, m_bottom{ m_modified, bottom }
		, m_top{ m_modified, top }
		, m_near{ m_modified, nearZ }
		, m_far{ m_modified, farZ }
		, m_viewport{ 1u, 1u, 0, 0 }
		, m_scissor{ 0, 0, 1u, 1u }
	{
		if ( m_type != ViewportType::eOrtho && !m_near )
		{
			m_near = float( 0.1 ); // not zero or we have a Z fight (due to depth buffer precision)
		}
	}

	Viewport::Viewport( Engine const & engine )
		: Viewport{ engine, ViewportType::eOrtho, Angle{}, 1, 0, 1, 0, 1, 0, 1 }
	{
	}

	Viewport::~Viewport()
	{
	}

	bool Viewport::update()
	{
		bool result = false;

		if ( isModified() )
		{
			switch ( m_type )
			{
			case castor3d::ViewportType::eOrtho:
				m_projection = m_engine.getRenderSystem()->getOrtho( m_left
					, m_right
					, m_bottom
					, m_top
					, m_near
					, m_far );
				m_safeBandedProjection = getSafeBandedOrtho( *m_engine.getRenderSystem()
					, m_size
					, m_left
					, m_right
					, m_bottom
					, m_top
					, m_near
					, m_far );
				break;

			case castor3d::ViewportType::ePerspective:
				m_projection = m_engine.getRenderSystem()->getPerspective( m_fovY
					, m_ratio
					, m_near
					, m_far );
				m_safeBandedProjection = getSafeBandedPerspective( *m_engine.getRenderSystem()
					, m_size
					, m_fovY
					, m_ratio
					, m_near
					, m_far );
				break;

			case castor3d::ViewportType::eFrustum:
				m_projection = m_engine.getRenderSystem()->getFrustum( m_left
					, m_right
					, m_bottom
					, m_top
					, m_near
					, m_far );
				m_safeBandedProjection = getSafeBandedFrustum( *m_engine.getRenderSystem()
					, m_size
					, m_left
					, m_right
					, m_bottom
					, m_top
					, m_near
					, m_far );
				break;

			default:
				break;
			}

			m_modified = false;
			result = true;
		}

		return result;
	}

	void Viewport::setPerspective( Angle const & fovY
		, float aspect
		, float nearZ
		, float farZ )
	{
		m_type = ViewportType::ePerspective;
		m_fovY = fovY;
		m_ratio = aspect;
		m_left = 0;
		m_right = 1;
		m_bottom = 0;
		m_top = 1;
		m_near = nearZ;
		m_far = farZ;
		m_modified = true;
	}

	void Viewport::setFrustum( float left
		, float right
		, float bottom
		, float top
		, float nearZ
		, float farZ )
	{
		m_type = ViewportType::eFrustum;
		m_fovY = Angle{};
		m_ratio = 0;
		m_left = left;
		m_right = right;
		m_bottom = bottom;
		m_top = top;
		m_near = nearZ;
		m_far = farZ;
		m_modified = true;
	}

	void Viewport::setOrtho( float left
		, float right
		, float bottom
		, float top
		, float nearZ
		, float farZ )
	{
		m_type = ViewportType::eOrtho;
		m_fovY = Angle{};
		m_ratio = 0;
		m_left = left;
		m_right = right;
		m_bottom = bottom;
		m_top = top;
		m_near = nearZ;
		m_far = farZ;
		m_modified = true;
	}

	void Viewport::resize( const castor::Size & value )
	{
		m_size = value;
		m_viewport = VkViewport{ 0.0f, 0.0f, float( m_size[0] ), float( m_size[1] ), 0.0f, 1.0f };
		m_scissor = VkRect2D{ { 0, 0 }, m_size[0], m_size[1] };
	}

	float Viewport::getProjectionScale()const
	{
		float const scale = std::abs( 2.0f * ( getFovY() * 0.5f ).tan() );
		return std::abs( getHeight() / scale );
	}
}
