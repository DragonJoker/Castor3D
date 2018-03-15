#include "Bone.hpp"

#include "BonedVertex.hpp"
#include "Skeleton.hpp"
#include "Mesh/Mesh.hpp"
#include "Mesh/Submesh.hpp"
#include "Mesh/Vertex.hpp"
#include "Mesh/SubmeshComponent/BonesComponent.hpp"

using namespace castor;

namespace castor3d
{
	//*************************************************************************************************

	bool BinaryWriter< Bone >::doWrite( Bone const & obj )
	{
		bool result = true;

		if ( result )
		{
			result = doWriteChunk( obj.getName(), ChunkType::eName, m_chunk );
		}

		if ( result )
		{
			result = doWriteChunk( obj.getOffsetMatrix(), ChunkType::eBoneOffsetMatrix, m_chunk );
		}

		if ( obj.getParent() )
		{
			result = doWriteChunk( obj.getParent()->getName(), ChunkType::eBoneParentName, m_chunk );
		}

		return result;
	}

	//*************************************************************************************************

	bool BinaryParser< Bone >::doParse( Bone & obj )
	{
		bool result = true;
		BoneSPtr bone;
		String name;
		BinaryChunk chunk;
		auto & skeleton = obj.m_skeleton;

		while ( result && doGetSubChunk( chunk ) )
		{
			switch ( chunk.getChunkType() )
			{
			case ChunkType::eName:
				result = doParseChunk( name, chunk );

				if ( result )
				{
					obj.setName( name );
				}

				break;

			case ChunkType::eBoneOffsetMatrix:
				result = doParseChunk( obj.m_offset, chunk );
				break;

			case ChunkType::eBoneParentName:
				result = doParseChunk( name, chunk );

				if ( result )
				{
					auto parent = skeleton.findBone( name );

					if ( parent )
					{
						parent->addChild( obj.shared_from_this() );
						obj.setParent( parent );
					}
					else
					{
						result = false;
					}
				}

				break;
			}
		}

		return result;
	}

	//*************************************************************************************************

	Bone::Bone( Skeleton & skeleton
		, Matrix4x4r const & offset )
		: Named{ cuEmptyString }
		, m_skeleton{ skeleton }
		, m_offset{ offset }
		, m_absoluteOffset{ skeleton.getGlobalInverseTransform() }
	{
	}

	Bone::~Bone()
	{
	}

	void Bone::addChild( BoneSPtr bone )
	{
		if ( m_children.end() == m_children.find( bone->getName() ) )
		{
			m_children.emplace( bone->getName(), bone );
		}
	}

	void Bone::setParent( BoneSPtr bone )
	{
		m_parent = bone;
		m_absoluteOffset = m_offset * m_parent->m_absoluteOffset;
	}

	BoundingBox Bone::computeBoundingBox( Mesh const & mesh
		, uint32_t boneIndex )const
	{
		real rmax = std::numeric_limits< real >::max();
		real rmin = std::numeric_limits< real >::lowest();
		Point3r min{ rmax, rmax, rmax };
		Point3r max{ rmin, rmin, rmin };

		for ( auto & submesh : mesh )
		{
			if ( submesh->hasComponent( BonesComponent::Name ) )
			{
				auto component = submesh->getComponent< BonesComponent >();
				uint32_t i = 0u;

				for ( auto & boneData : component->getBonesData() )
				{
					auto it = std::find( boneData.m_ids.data.begin()
						, boneData.m_ids.data.end()
						, boneIndex );

					if ( it != boneData.m_ids.data.end() )
					{
						auto position = submesh->getPoint( i ).pos;
						min[0] = std::min( min[0], position[0] );
						min[1] = std::min( min[1], position[1] );
						min[2] = std::min( min[2], position[2] );
						max[0] = std::max( max[0], position[0] );
						max[1] = std::max( max[1], position[1] );
						max[2] = std::max( max[2], position[2] );
					}
				}
			}
		}

		return BoundingBox{ min, max };
	}
}
