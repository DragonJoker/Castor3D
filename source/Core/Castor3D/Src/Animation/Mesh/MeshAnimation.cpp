#include "MeshAnimation.hpp"

#include "Engine.hpp"

#include "Mesh/Submesh.hpp"

using namespace castor;

namespace castor3d
{
	//*************************************************************************************************

	bool BinaryWriter< MeshAnimation >::doWrite( MeshAnimation const & p_obj )
	{
		bool result = true;

		for ( auto const & submesh : p_obj.m_submeshes )
		{
			result &= doWriteChunk( submesh.getSubmesh().getId(), ChunkType::eMeshAnimationSubmeshID, m_chunk );
			result &= BinaryWriter< MeshAnimationSubmesh >{}.write( submesh, m_chunk );
		}

		return result;
	}

	//*************************************************************************************************

	bool BinaryParser< MeshAnimation >::doParse( MeshAnimation & p_obj )
	{
		bool result = true;
		MeshAnimationSubmeshUPtr submeshAnim;
		SubmeshSPtr submesh;
		BinaryChunk chunk;
		uint32_t id{ 0u };

		while ( result && doGetSubChunk( chunk ) )
		{
			switch ( chunk.getChunkType() )
			{
			case ChunkType::eMeshAnimationSubmeshID:
				result = doParseChunk( id, chunk );

				if ( result )
				{
					submesh = static_cast< Mesh & >( *p_obj.getOwner() ).getSubmesh( id );
				}

				break;

			case ChunkType::eMeshAnimationSubmesh:
				if ( submesh )
				{
					MeshAnimationSubmesh submeshAnim{ p_obj, *submesh };
					result = BinaryParser< MeshAnimationSubmesh >{}.parse( submeshAnim, chunk );

					if ( result )
					{
						p_obj.addChild( std::move( submeshAnim ) );
					}
				}

				break;
			}
		}

		return result;
	}

	//*************************************************************************************************

	MeshAnimation::MeshAnimation( Animable & p_animable, String const & p_name )
		: Animation{ AnimationType::eMesh, p_animable, p_name }
	{
	}

	MeshAnimation::~MeshAnimation()
	{
	}

	void MeshAnimation::addChild( MeshAnimationSubmesh && p_object )
	{
		m_submeshes.push_back( std::move( p_object ) );
	}

	void MeshAnimation::doUpdateLength()
	{
		for ( auto const & submesh : m_submeshes )
		{
			m_length = std::max( m_length, submesh.getLength() );
		}
	}

	//*************************************************************************************************
}
