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
				, [this, prv]( RenderDevice const & device
					, QueueData const & queueData )
				{
					if ( m_animationObject.getComponent().isReady() )
					{
						auto & offsets = m_animationObject.getSubmesh().getBufferOffsets();
						auto & vertexBuffer = offsets.getVertexBuffer();
						auto & animBuffer = m_animationObject.getComponent().getAnimationBuffer();
						auto & staging = m_animationObject.getComponent().getStagingBuffer();
						auto & commandBuffer = m_animationObject.getComponent().getCommandBuffer();
						commandBuffer.begin( VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT );
						staging.uploadBufferData( commandBuffer
							, reinterpret_cast< uint8_t const * >( prv.m_buffer.data() )
							, prv.m_buffer.size() * sizeof( InterleavedVertex )
							, vertexBuffer );
						commandBuffer.end();
						queueData.queue->submit( commandBuffer );

						if ( auto * buffer = animBuffer.lock() )
						{
							std::copy( prv.m_buffer.begin()
								, prv.m_buffer.end()
								, buffer );
							animBuffer.flush();
							animBuffer.unlock();
						}
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
