#include "Castor3D/Render/Viewport.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Render/RenderSystem.hpp"

CU_ImplementSmartPtr( c3d, Viewport )

namespace c3d
{
	namespace viewport
	{
		static Matrix4x4f getSafeBandedOrtho( RenderSystem const & renderSystem
			, Size const & size
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
			Point2d bandRatio{ width / size.getWidth(), height / size.getHeight() };
			return renderSystem.getOrtho( float( left * bandRatio->x )
				, float( right * bandRatio->x )
				, float( bottom * bandRatio->y )
				, float( top * bandRatio->y )
				, nearZ
				, farZ );
		}

		static Matrix4x4f getSafeBandedFrustum( RenderSystem const & renderSystem
			, Size const & size
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
			Point2d bandRatio{ width / size.getWidth(), height / size.getHeight() };
			return renderSystem.getFrustum( float( left * bandRatio->x )
				, float( right * bandRatio->x )
				, float( bottom * bandRatio->y )
				, float( top * bandRatio->y )
				, nearZ
				, farZ );
		}

		static Matrix4x4f getSafeBandedPerspective( RenderSystem const & renderSystem
			, Size const & size
			, Angle fovY
			, float aspect
			, float nearZ
			, float farZ )
		{
			auto opp = size.getHeight();
			auto adj = opp / ( fovY / 2.0f ).tand();
			auto halfOpp = opp / 2;
			auto bandSize = double( getSafeBandSize( size ) );
			auto halfHeight = halfOpp + bandSize;
			auto halfWidth = std::ceil( aspect * float( halfOpp ) ) + bandSize;
			return renderSystem.getPerspective( fovY + ( c3d::atanf( ( bandSize * 2.85f / 4.0f ) / adj ) )
				, float( halfWidth / halfHeight )
				, nearZ
				, farZ );
		}

		static Matrix4x4f getSafeBandedInfinitePerspective( RenderSystem const & renderSystem
			, Size const & size
			, Angle fovY
			, float aspect
			, float nearZ )
		{
			auto opp = size.getHeight();
			auto adj = opp / ( fovY / 2.0f ).tand();
			auto halfOpp = opp / 2;
			auto bandSize = double( getSafeBandSize( size ) );
			auto halfHeight = halfOpp + bandSize;
			auto halfWidth = std::ceil( aspect * float( halfOpp ) ) + bandSize;
			return renderSystem.getInfinitePerspective( fovY + ( c3d::atanf( ( bandSize * 2.85f / 4.0f ) / adj ) )
				, float( halfWidth / halfHeight )
				, nearZ );
		}
	}

	const Array< String, size_t( ViewportType::eCount ) > Viewport::TypeName
	{
		cuT( "undefined" ),
		cuT( "ortho" ),
		cuT( "perspective" ),
		cuT( "frustum" )
	};

	Viewport::Viewport( Engine const & engine
		, ViewportType type
		, Angle const & fovY
		, float aspect
		, float left
		, float right
		, float bottom
		, float top
		, float nearZ
		, float farZ )
		: m_engine{ engine }
		, m_left{ m_modified, left }
		, m_right{ m_modified, right }
		, m_top{ m_modified, top }
		, m_bottom{ m_modified, bottom }
		, m_far{ m_modified, farZ }
		, m_near{ m_modified, nearZ }
		, m_fovY{ m_modified, fovY }
		, m_ratio{ m_modified, aspect }
		, m_type{ m_modified, type }
		, m_position{ m_modified }
	{
		if ( m_type != ViewportType::eOrtho && m_near == 0.0f )
		{
			m_near = float( 0.1 ); // not zero or we have a Z fight (due to depth buffer precision)
		}
	}

	Viewport::Viewport( Viewport const & rhs )
		: m_engine{ rhs.m_engine }
		, m_modified{ rhs.m_modified }
		, m_left{ m_modified, rhs.m_left.value() }
		, m_right{ m_modified, rhs.m_right.value() }
		, m_top{ m_modified, rhs.m_top.value() }
		, m_bottom{ m_modified, rhs.m_bottom.value() }
		, m_far{ m_modified, rhs.m_far.value() }
		, m_near{ m_modified, rhs.m_near.value() }
		, m_fovY{ m_modified, rhs.m_fovY.value() }
		, m_ratio{ m_modified, rhs.m_ratio.value() }
		, m_type{ m_modified, rhs.m_type.value() }
		, m_position{ m_modified, rhs.m_position.value() }
		, m_projection{ rhs.m_projection }
	{
	}

	Viewport::Viewport( Viewport && rhs )noexcept
		: m_engine{ rhs.m_engine }
		, m_modified{ rhs.m_modified }
		, m_left{ m_modified, rhs.m_left.value() }
		, m_right{ m_modified, rhs.m_right.value() }
		, m_top{ m_modified, rhs.m_top.value() }
		, m_bottom{ m_modified, rhs.m_bottom.value() }
		, m_far{ m_modified, rhs.m_far.value() }
		, m_near{ m_modified, rhs.m_near.value() }
		, m_fovY{ m_modified, rhs.m_fovY.value() }
		, m_ratio{ m_modified, rhs.m_ratio.value() }
		, m_type{ m_modified, rhs.m_type.value() }
		, m_position{ m_modified, rhs.m_position.value() }
		, m_projection{ c3d::move( rhs.m_projection ) }
	{
	}

	Viewport::Viewport( Engine const & engine )
		: Viewport{ engine, ViewportType::eOrtho, Angle{}, 1, 0, 1, 0, 1, 0, 1 }
	{
	}

	bool Viewport::update()
	{
		bool result = false;

		if ( isModified() )
		{
			m_projection = getRescaledProjection( 1.0f );
			m_modified = false;
			result = true;
		}

		return result;
	}

	void Viewport::cloneInto( Viewport & output )const
	{
		output.m_left = m_left;
		output.m_right = m_right;
		output.m_top = m_top;
		output.m_bottom = m_bottom;
		output.m_far = m_far;
		output.m_near = m_near;
		output.m_fovY = m_fovY;
		output.m_ratio = m_ratio;
		output.m_type = m_type;
		output.m_position = m_position;
		output.m_projection = m_projection;
		output.m_modified = true;
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

	void Viewport::setInfinitePerspective( Angle const & fovY
		, float aspect
		, float nearZ )
	{
		m_type = ViewportType::eInfinitePerspective;
		m_fovY = fovY;
		m_ratio = aspect;
		m_left = 0;
		m_right = 1;
		m_bottom = 0;
		m_top = 1;
		m_near = nearZ;
		m_far = std::numeric_limits< float >::infinity();
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

	Matrix4x4f Viewport::getRescaledProjection( float scale )const
	{
		switch ( m_type )
		{
		case ViewportType::eOrtho:
			return m_engine.getRenderSystem()->getOrtho( m_left
				, m_right
				, m_bottom * scale
				, m_top * scale
				, m_near * scale
				, m_far * scale );
		case ViewportType::ePerspective:
			return m_engine.getRenderSystem()->getPerspective( m_fovY
				, m_ratio
				, m_near * scale
				, m_far * scale );
		case ViewportType::eInfinitePerspective:
			return m_engine.getRenderSystem()->getInfinitePerspective( m_fovY
				, m_ratio
				, m_near * scale );
		default:
			return m_engine.getRenderSystem()->getFrustum( m_left
				, m_right
				, m_bottom * scale
				, m_top * scale
				, m_near * scale
				, m_far * scale );
		}
	}

	Matrix4x4f Viewport::getRescaledSafeBandedProjection( Size const & renderSize, float scale )const
	{
		switch ( m_type )
		{
		case ViewportType::eOrtho:
			return viewport::getSafeBandedOrtho( *m_engine.getRenderSystem()
				, renderSize
				, m_left * scale
				, m_right * scale
				, m_bottom * scale
				, m_top * scale
				, m_near * scale
				, m_far * scale );
		case ViewportType::ePerspective:
			return viewport::getSafeBandedPerspective( *m_engine.getRenderSystem()
				, renderSize
				, m_fovY
				, m_ratio
				, m_near * scale
				, m_far * scale );
		case ViewportType::eInfinitePerspective:
			return viewport::getSafeBandedInfinitePerspective( *m_engine.getRenderSystem()
				, renderSize
				, m_fovY
				, m_ratio
				, m_near * scale );
		default:
			return viewport::getSafeBandedFrustum( *m_engine.getRenderSystem()
				, renderSize
				, m_left * scale
				, m_right * scale
				, m_bottom * scale
				, m_top * scale
				, m_near * scale
				, m_far * scale );
		}
	}
}
