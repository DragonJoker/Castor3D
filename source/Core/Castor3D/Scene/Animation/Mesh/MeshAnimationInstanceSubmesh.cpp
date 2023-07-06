#include "Castor3D/Scene/Animation/Mesh/MeshAnimationInstanceSubmesh.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Animation/Interpolator.hpp"
#include "Castor3D/Event/Frame/GpuFunctorEvent.hpp"
#include "Castor3D/Model/Mesh/Mesh.hpp"
#include "Castor3D/Model/Mesh/Animation/MeshAnimation.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/MorphComponent.hpp"
#include "Castor3D/Scene/Geometry.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/Animation/AnimatedMesh.hpp"
#include "Castor3D/Scene/Animation/Mesh/MeshAnimationInstance.hpp"

#include <ashespp/Buffer/VertexBuffer.hpp>

namespace castor3d
{
	//*************************************************************************************************

	namespace mshanminstsm
	{
		static castor::BoundingBox doInterpolateBB( castor::BoundingBox const & prv
			, castor::BoundingBox const & cur
			, Interpolator< castor::Point3f > const & interpolator
			, float const factor )
		{
			return castor::BoundingBox
			{
				interpolator.interpolate( prv.getMin(), prv.getMin(), factor ),
				interpolator.interpolate( prv.getMax(), prv.getMax(), factor ),
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
		, std::vector< float > const & prv
		, std::vector< float > const & cur
		, castor::BoundingBox const & prvbb
		, castor::BoundingBox const & curbb )
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
			auto interpolator = makeInterpolator< float >( getOwner()->getInterpolation() );
			auto prvIt = prv.begin();
			auto curIt = cur.begin();
			m_cur.resize( cur.size() );
			auto resIt = m_cur.begin();

			for ( size_t i = 0u; i < m_cur.size(); ++i )
			{
				*resIt = interpolator->interpolate( *prvIt, *curIt, factor );
				++curIt;
				++prvIt;
				++resIt;
			}
		}

		auto interpolator = makeInterpolator< castor::Point3f >( getOwner()->getInterpolation() );
		getOwner()->getAnimatedMesh().getGeometry().setBoundingBox( m_animationObject.getSubmesh()
			, mshanminstsm::doInterpolateBB( prvbb
				, curbb
				, *interpolator
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
