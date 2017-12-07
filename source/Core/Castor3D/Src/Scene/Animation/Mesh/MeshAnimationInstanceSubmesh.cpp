#include "MeshAnimationInstanceSubmesh.hpp"

#include "Animation/Mesh/MeshAnimation.hpp"
#include "MeshAnimationInstance.hpp"
#include "Mesh/Submesh.hpp"
#include "Mesh/SubmeshComponent/MorphComponent.hpp"
#include "Mesh/Buffer/VertexBuffer.hpp"
#include "Shader/Uniform/Uniform.hpp"

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
			auto & vertexBuffer = m_animationObject.getSubmesh().getVertexBuffer();
			auto & animBuffer = m_animationObject.getComponent().getAnimationBuffer();
			std::memcpy( vertexBuffer.getData(), prv.m_buffer.data(), vertexBuffer.getSize() );
			std::memcpy( animBuffer.getData(), cur.m_buffer.data(), animBuffer.getSize() );
			m_animationObject.getSubmesh().needsUpdate();
			m_animationObject.getComponent().needsUpdate();
		}

		m_animationObject.getSubmesh().updateContainers( doInterpolateBB( prv.m_boundingBox
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
