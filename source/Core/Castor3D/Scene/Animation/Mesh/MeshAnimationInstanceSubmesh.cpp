#include "Castor3D/Scene/Animation/Mesh/MeshAnimationInstanceSubmesh.hpp"

#include "Castor3D/Engine.hpp"
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

using namespace castor;

namespace castor3d
{
	//*************************************************************************************************

	namespace
	{
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

	void MeshAnimationInstanceSubmesh::update( float factor
		, SubmeshAnimationBuffer const & prv
		, SubmeshAnimationBuffer const & cur )
	{
		if ( &cur != m_cur )
		{
			getOwner()->getAnimatedMesh().getMesh().getScene()->getEngine()->postEvent( makeGpuFunctorEvent( EventType::ePreRender
				, [this, cur, prv]( RenderDevice const & device
					, QueueData const & queueData )
				{
					auto & vertexBuffer = m_animationObject.getSubmesh().getVertexBuffer();
					auto & animBuffer = m_animationObject.getComponent().getAnimationBuffer();

					if ( auto * buffer = vertexBuffer.lock( 0u, vertexBuffer.getCount(), 0u ) )
					{
						std::copy( prv.m_buffer.begin()
							, prv.m_buffer.end()
							, buffer );
						vertexBuffer.flush( 0u, vertexBuffer.getCount() );
						vertexBuffer.unlock();
					}

					if ( auto * buffer = animBuffer.lock( 0u, animBuffer.getCount(), 0u ) )
					{
						std::copy( prv.m_buffer.begin()
							, prv.m_buffer.end()
							, buffer );
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
