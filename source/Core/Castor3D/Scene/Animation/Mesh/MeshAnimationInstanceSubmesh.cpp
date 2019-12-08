#include "Castor3D/Scene/Animation/Mesh/MeshAnimationInstanceSubmesh.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Animation/Mesh/MeshAnimation.hpp"
#include "Castor3D/Mesh/Mesh.hpp"
#include "Castor3D/Mesh/Submesh.hpp"
#include "Castor3D/Mesh/SubmeshComponent/MorphComponent.hpp"
#include "Castor3D/Scene/Geometry.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/Animation/AnimatedMesh.hpp"
#include "Castor3D/Scene/Animation/Mesh/MeshAnimationInstance.hpp"

#include <ashespp/Buffer/VertexBuffer.hpp>

using namespace castor;

namespace castor3d
{
	//*************************************************************************************************

	namespace
	{
		std::ostream & operator<<( std::ostream & stream, castor::Point3f const & point )
		{
			stream << "[" << point[0] << ", " << point[1] << ", " << point[2] << "]";
			return stream;
		}

		inline BoundingBox doInterpolateBB( BoundingBox const & prv
			, BoundingBox const & cur
			, float const factor )
		{
			return BoundingBox
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

	MeshAnimationInstanceSubmesh::~MeshAnimationInstanceSubmesh()
	{
	}

	void MeshAnimationInstanceSubmesh::update( float factor
		, SubmeshAnimationBuffer const & prv
		, SubmeshAnimationBuffer const & cur )
	{
		if ( &cur != m_cur )
		{
			getOwner()->getAnimatedMesh().getMesh().getScene()->getEngine()->postEvent( makeFunctorEvent( EventType::ePreRender
				, [this, cur, prv]()
				{
					auto & vertexBuffer = m_animationObject.getSubmesh().getVertexBuffer();
					auto & animBuffer = m_animationObject.getComponent().getAnimationBuffer();

					if ( auto * buffer = vertexBuffer.lock( 0u, vertexBuffer.getCount(), 0u ) )
					{
						std::memcpy( buffer, prv.m_buffer.data(), vertexBuffer.getSize() );
						vertexBuffer.flush( 0u, vertexBuffer.getCount() );
						vertexBuffer.unlock();
					}

					if ( auto * buffer = animBuffer.lock( 0u, animBuffer.getCount(), 0u ) )
					{
						std::memcpy( buffer, prv.m_buffer.data(), animBuffer.getSize() );
						animBuffer.flush( 0u, animBuffer.getCount() );
						animBuffer.unlock();
					}
				} ) );
		}

		getOwner()->getAnimatedMesh().getGeometry().setBoundingBox( m_animationObject.getSubmesh()
			, doInterpolateBB( prv.m_boundingBox
				, cur.m_boundingBox
				, factor ) );
		m_cur = &cur;
		m_currentFactor = factor;
	}

	Submesh const & MeshAnimationInstanceSubmesh::getSubmesh()const
	{
		return m_animationObject.getSubmesh();
	}

	//*************************************************************************************************
}
