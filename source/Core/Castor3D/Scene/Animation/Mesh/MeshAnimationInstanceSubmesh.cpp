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
		, SubmeshAnimationBuffer const & prv
		, SubmeshAnimationBuffer const & cur )
	{
		if ( &cur != m_cur )
		{
			auto & offsets = m_animationObject.getSubmesh().getBufferOffsets();

			if ( offsets.hasData( SubmeshFlag::eMorphPositions ) )
			{
				auto & prev = offsets.getBufferChunk( SubmeshFlag::ePositions );
				std::copy( prv.positions.begin()
					, prv.positions.end()
					, prev.getData< castor::Point3f >().begin() );
				prev.markDirty();

				auto & curr = offsets.getBufferChunk( SubmeshFlag::eMorphPositions );
				std::copy( cur.positions.begin()
					, cur.positions.end()
					, curr.getData< castor::Point3f >().begin() );
				curr.markDirty();
			}

			if ( offsets.hasData( SubmeshFlag::eMorphNormals ) )
			{
				auto & prev = offsets.getBufferChunk( SubmeshFlag::eNormals );
				std::copy( prv.normals.begin()
					, prv.normals.end()
					, prev.getData< castor::Point3f >().begin() );
				prev.markDirty();

				auto & curr = offsets.getBufferChunk( SubmeshFlag::eMorphNormals );
				std::copy( cur.normals.begin()
					, cur.normals.end()
					, curr.getData< castor::Point3f >().begin() );
				curr.markDirty();
			}

			if ( offsets.hasData( SubmeshFlag::eMorphTangents ) )
			{
				auto & prev = offsets.getBufferChunk( SubmeshFlag::eTangents );
				std::copy( prv.tangents.begin()
					, prv.tangents.end()
					, prev.getData< castor::Point3f >().begin() );
				prev.markDirty();

				auto & curr = offsets.getBufferChunk( SubmeshFlag::eMorphTangents );
				std::copy( cur.tangents.begin()
					, cur.tangents.end()
					, curr.getData< castor::Point3f >().begin() );
				curr.markDirty();
			}

			if ( offsets.hasData( SubmeshFlag::eMorphTexcoords ) )
			{
				auto & prev = offsets.getBufferChunk( SubmeshFlag::eTexcoords );
				std::copy( prv.texcoords.begin()
					, prv.texcoords.end()
					, prev.getData< castor::Point3f >().begin() );
				prev.markDirty();

				auto & curr = offsets.getBufferChunk( SubmeshFlag::eMorphTexcoords );
				std::copy( cur.texcoords.begin()
					, cur.texcoords.end()
					, curr.getData< castor::Point3f >().begin() );
				curr.markDirty();
			}
		}

		getOwner()->getAnimatedMesh().getGeometry().setBoundingBox( m_animationObject.getSubmesh()
			, mshanminstsm::doInterpolateBB( prv.boundingBox
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
