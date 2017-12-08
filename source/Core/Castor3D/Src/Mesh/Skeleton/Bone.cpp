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

	bool BinaryWriter< Bone >::doWrite( Bone const & p_obj )
	{
		bool result = true;

		if ( result )
		{
			result = doWriteChunk( p_obj.getName(), ChunkType::eName, m_chunk );
		}

		if ( result )
		{
			result = doWriteChunk( p_obj.getOffsetMatrix(), ChunkType::eBoneOffsetMatrix, m_chunk );
		}

		if ( p_obj.getParent() )
		{
			result = doWriteChunk( p_obj.getParent()->getName(), ChunkType::eBoneParentName, m_chunk );
		}

		return result;
	}

	//*************************************************************************************************

	bool BinaryParser< Bone >::doParse( Bone & p_obj )
	{
		bool result = true;
		BoneSPtr bone;
		String name;
		BinaryChunk chunk;
		auto & skeleton = p_obj.m_skeleton;

		while ( result && doGetSubChunk( chunk ) )
		{
			switch ( chunk.getChunkType() )
			{
			case ChunkType::eName:
				result = doParseChunk( name, chunk );

				if ( result )
				{
					p_obj.setName( name );
				}

				break;

			case ChunkType::eBoneOffsetMatrix:
				result = doParseChunk( p_obj.m_offset, chunk );
				break;

			case ChunkType::eBoneParentName:
				result = doParseChunk( name, chunk );

				if ( result )
				{
					auto parent = skeleton.findBone( name );

					if ( parent )
					{
						parent->addChild( p_obj.shared_from_this() );
						p_obj.setParent( parent );
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

	Bone::Bone( Skeleton & p_skeleton )
		: Named( cuEmptyString )
		, m_skeleton( p_skeleton )
	{
	}

	Bone::~Bone()
	{
	}

	void Bone::addChild( BoneSPtr p_bone )
	{
		if ( m_children.end() == m_children.find( p_bone->getName() ) )
		{
			m_children.insert( { p_bone->getName(), p_bone } );
		}
	}

	BoundingBox Bone::computeBoundingBox( Mesh const & mesh, uint32_t boneIndex )const
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

				for ( auto & data : component->getBonesData() )
				{
					auto boneData = BonedVertex::getBones( data );
					auto it = std::find( boneData.m_ids.begin()
						, boneData.m_ids.end()
						, boneIndex );

					if ( it != boneData.m_ids.end() )
					{
						Coords3r position;
						Vertex::getPosition( submesh->getPoint( i ), position );
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
