#include "MeshAnimationInstanceSubmesh.hpp"

#include "Engine.hpp"
#include "Animation/Mesh/MeshAnimation.hpp"
#include "MeshAnimationInstance.hpp"
#include "Mesh/Mesh.hpp"
#include "Mesh/Submesh.hpp"
#include "Mesh/SubmeshComponent/MorphComponent.hpp"
#include "Scene/Geometry.hpp"
#include "Scene/Scene.hpp"
#include "Scene/Animation/AnimatedMesh.hpp"

#include <Buffer/VertexBuffer.hpp>

using namespace castor;

namespace castor3d
{
	//*************************************************************************************************

	namespace
	{
		std::ostream & operator<<( std::ostream & stream, Point3r const & point )
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
				Point3r{ prv.getMin() * real( 1.0 - factor ) + cur.getMin() * factor },
				Point3r{ prv.getMax() * real( 1.0 - factor ) + cur.getMax() * factor }
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

					if ( auto * buffer = vertexBuffer.lock( 0u, vertexBuffer.getCount(), renderer::MemoryMapFlag::eWrite ) )
					{
						std::memcpy( buffer, prv.m_buffer.data(), vertexBuffer.getSize() );
						vertexBuffer.flush( 0u, vertexBuffer.getCount() );
						vertexBuffer.unlock();
					}

					if ( auto * buffer = animBuffer.lock( 0u, animBuffer.getCount(), renderer::MemoryMapFlag::eWrite ) )
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
