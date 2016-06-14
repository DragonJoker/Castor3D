#include "MeshAnimation.hpp"

#include "Engine.hpp"

#include "Event/Frame/InitialiseEvent.hpp"
#include "Mesh/Submesh.hpp"
#include "Animation/Mesh/MeshAnimationSubmesh.hpp"

#include "Scene/Geometry.hpp"

using namespace Castor;

namespace Castor3D
{
	//*************************************************************************************************

	bool BinaryWriter< MeshAnimation >::DoWrite( MeshAnimation const & p_obj )
	{
		bool l_return = true;

		for ( auto l_submesh : p_obj.m_submeshes )
		{
			l_return &= DoWriteChunk( l_submesh->GetSubmesh().GetId(), eCHUNK_TYPE_MESH_ANIMATION_SUBMESH_ID, m_chunk );
			l_return &= BinaryWriter< MeshAnimationSubmesh >{}.Write( *l_submesh, m_chunk );
		}

		return l_return;
	}

	//*************************************************************************************************

	bool BinaryParser< MeshAnimation >::DoParse( MeshAnimation & p_obj )
	{
		bool l_return = true;
		MeshAnimationSubmeshSPtr l_submeshAnim;
		SubmeshSPtr l_submesh;
		BinaryChunk l_chunk;
		uint32_t l_id{ 0u };

		while ( l_return && DoGetSubChunk( l_chunk ) )
		{
			switch ( l_chunk.GetChunkType() )
			{
			case eCHUNK_TYPE_MESH_ANIMATION_SUBMESH_ID:
				l_return = DoParseChunk( l_id, l_chunk );

				if ( l_return )
				{
					l_submesh = static_cast< Mesh & >( *p_obj.GetOwner() ).GetSubmesh( l_id );
				}

				break;

			case eCHUNK_TYPE_MESH_ANIMATION_SUBMESH:
				if ( l_submesh )
				{
					l_submeshAnim = std::make_shared< MeshAnimationSubmesh >( p_obj, *l_submesh );
					l_return = BinaryParser< MeshAnimationSubmesh >{}.Parse( *l_submeshAnim, l_chunk );

					if ( l_return )
					{
						p_obj.AddChild( l_submeshAnim );
					}
				}
				break;
			}
		}

		return l_return;
	}

	//*************************************************************************************************

	MeshAnimation::MeshAnimation( Animable & p_animable, String const & p_name )
		: Animation{ AnimationType::Mesh, p_animable, p_name }
	{
	}

	MeshAnimation::~MeshAnimation()
	{
	}

	void MeshAnimation::AddChild( MeshAnimationSubmeshSPtr p_object )
	{
		m_submeshes.push_back( p_object );
	}

	bool MeshAnimation::DoInitialise()
	{
		for ( auto l_it : m_submeshes )
		{
			m_length = std::max( m_length, l_it->GetLength() );
		}

		return true;
	}

	//*************************************************************************************************
}
