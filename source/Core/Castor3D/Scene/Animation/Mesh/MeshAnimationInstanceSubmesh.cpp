#include "Castor3D/Scene/Animation/Mesh/MeshAnimationInstanceSubmesh.hpp"

#include "Castor3D/Engine.hpp"
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
			if ( m_animationObject.getComponent().isReady() )
			{
				auto & offsets = m_animationObject.getSubmesh().getBufferOffsets();
				auto & animBuffer = m_animationObject.getComponent().getAnimationBuffer();

				std::copy( prv.buffer.begin()
					, prv.buffer.end()
					, offsets.getVertexData< InterleavedVertex >().begin() );
				offsets.vtxBuffer->markDirty( offsets.getVertexOffset()
					, offsets.getVertexCount< InterleavedVertex >() * sizeof( InterleavedVertex )
					, VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT
					, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT );

				std::copy( cur.buffer.begin()
					, cur.buffer.end()
					, animBuffer.getData().begin() );
				animBuffer.buffer->markDirty( animBuffer.getOffset()
					, animBuffer.getSize()
					, VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT
					, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT );
			}
		}

		getOwner()->getAnimatedMesh().getGeometry().setBoundingBox( m_animationObject.getSubmesh()
			, doInterpolateBB( prv.boundingBox
				, cur.boundingBox
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
