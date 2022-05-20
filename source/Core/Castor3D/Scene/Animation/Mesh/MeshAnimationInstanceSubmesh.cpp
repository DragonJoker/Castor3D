#include "Castor3D/Scene/Animation/Mesh/MeshAnimationInstanceSubmesh.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Animation/Interpolator.hpp"
#include "Castor3D/Event/Frame/GpuFunctorEvent.hpp"
#include "Castor3D/Miscellaneous/StagingData.hpp"
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
			, float const factor )
		{
			return castor::BoundingBox
			{
				castor::Point3f{ prv.getMin() * float( 1.0 - factor ) + cur.getMin() * factor },
				castor::Point3f{ prv.getMax() * float( 1.0 - factor ) + cur.getMax() * factor }
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
			castor3d::InterpolatorT< float, castor3d::InterpolatorType::eLinear > interpolator;
			auto prvIt = prv.begin();
			auto curIt = cur.begin();
			m_cur.resize( cur.size() );
			auto resIt = m_cur.begin();

			for ( size_t i = 0u; i < m_cur.size(); ++i )
			{
				*resIt = interpolator.interpolate( *prvIt, *curIt, factor );
				++curIt;
				++prvIt;
				++resIt;
			}
		}

		getOwner()->getAnimatedMesh().getGeometry().setBoundingBox( m_animationObject.getSubmesh()
			, mshanminstsm::doInterpolateBB( prvbb, curbb, factor ) );
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
