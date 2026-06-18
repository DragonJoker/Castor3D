#include "Castor3D/Scene/Animation/Mesh/MeshAnimationInstanceSubmesh.hpp"

#include "Castor3D/Animation/Interpolator.hpp"
#include "Castor3D/Model/Mesh/Animation/MeshAnimation.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Scene/Geometry.hpp"
#include "Castor3D/Scene/Animation/AnimatedMesh.hpp"
#include "Castor3D/Scene/Animation/Mesh/MeshAnimationInstance.hpp"

namespace c3d
{
	//*************************************************************************************************

	namespace mshanminstsm
	{
		static BoundingBox doInterpolateBB( BoundingBox const & prv
			, BoundingBox const & cur
			, Interpolator< Point3f > const & interpolator
			, float const factor )
		{
			return BoundingBox
			{
				interpolator.interpolate( prv.getMin(), cur.getMin(), factor ),
				interpolator.interpolate( prv.getMax(), cur.getMax(), factor ),
			};
		}
	}

	//*************************************************************************************************

	MeshAnimationInstanceSubmesh::MeshAnimationInstanceSubmesh( MeshAnimationInstance & animationInstance
		, MeshAnimationSubmesh & animationObject )
		: OwnedBy< MeshAnimationInstance >{ animationInstance }
		, m_animationObject{ animationObject }
	{
	}

	void MeshAnimationInstanceSubmesh::update( float factor
		, Vector< float > const & prv
		, Vector< float > const & cur
		, BoundingBox const & prvbb
		, BoundingBox const & curbb )
	{
		CU_Require( prv.size() == cur.size() );

		if ( factor <= 0.0 )
		{
			m_cur = prv;
		}
		else if ( factor >= 1.0 )
		{
			m_cur = cur;
		}
		else
		{
			if ( !m_floatInterpolator
				|| m_floatInterpolator->getType() != getOwner()->getInterpolation() )
				m_floatInterpolator = makeInterpolator< float >( getOwner()->getInterpolation() );
			auto prvIt = prv.begin();
			auto curIt = cur.begin();
			m_cur.resize( cur.size() );
			auto resIt = m_cur.begin();

			for ( size_t i = 0u; i < m_cur.size(); ++i )
			{
				*resIt = m_floatInterpolator->interpolate( *prvIt, *curIt, factor );
				++curIt;
				++prvIt;
				++resIt;
			}
		}

		if ( !m_vecInterpolator
			|| m_vecInterpolator->getType() != getOwner()->getInterpolation() )
			m_vecInterpolator = makeInterpolator< Point3f >( getOwner()->getInterpolation() );
		getOwner()->getAnimatedMesh().getGeometry().setBoundingBox( m_animationObject.getSubmesh()
			, mshanminstsm::doInterpolateBB( prvbb
				, curbb
				, *m_vecInterpolator
				, factor ) );
	}

	void MeshAnimationInstanceSubmesh::clear()
	{
		std::fill_n( m_cur.begin()
			, m_cur.size()
			, 0.0f );
	}

	Submesh const & MeshAnimationInstanceSubmesh::getSubmesh()const
	{
		return m_animationObject.getSubmesh();
	}

	//*************************************************************************************************
}
