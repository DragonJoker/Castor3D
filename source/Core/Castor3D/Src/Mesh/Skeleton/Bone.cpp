#include "Bone.hpp"

#include "Skeleton.hpp"

using namespace Castor;

namespace Castor3D
{
	//*************************************************************************************************

	bool BinaryWriter< Bone >::DoWrite( Bone const & p_obj )
	{
		bool l_return = true;
		BinaryChunk l_chunk( eCHUNK_TYPE_SKELETON_BONE );

		if ( l_return )
		{
			l_return = DoWriteChunk( p_obj.GetName(), eCHUNK_TYPE_NAME, m_chunk );
		}

		if ( l_return )
		{
			l_return = DoWriteChunk( p_obj.GetOffsetMatrix(), eCHUNK_TYPE_BONE_OFFSET_MATRIX, m_chunk );
		}

		if ( l_return )
		{
			l_return = DoWriteChunk( p_obj.GetFinalTransformation(), eCHUNK_TYPE_BONE_FINAL_TRANSFORM, m_chunk );
		}

		for ( auto && l_bone : p_obj.m_children )
		{
			l_return &= BinaryWriter< Bone >{}.Write( *l_bone, m_chunk );
		}

		return l_return;
	}

	//*************************************************************************************************

	bool BinaryParser< Bone >::DoParse( Bone & p_obj )
	{
		bool l_return = true;
		BoneSPtr l_bone;
		String l_name;
		BinaryChunk l_chunk;

		while ( l_return && DoGetSubChunk( l_chunk ) )
		{
			switch ( l_chunk.GetChunkType() )
			{
			case eCHUNK_TYPE_NAME:
				l_return = DoParseChunk( l_name, l_chunk );

				if ( l_return )
				{
					p_obj.SetName( l_name );
				}

				break;

			case eCHUNK_TYPE_BONE_OFFSET_MATRIX:
				l_return = DoParseChunk( p_obj.m_offset, l_chunk );
				break;

			case eCHUNK_TYPE_BONE_FINAL_TRANSFORM:
				l_return = DoParseChunk( p_obj.m_finalTransformation, l_chunk );
				break;

			case eCHUNK_TYPE_SKELETON_BONE:
				l_bone = std::make_shared< Bone >( p_obj.m_skeleton );
				l_return = BinaryParser< Bone >{}.Parse( *l_bone, l_chunk );

				if ( l_return )
				{
					p_obj.m_children.push_back( l_bone );
					l_bone->SetParent( p_obj.shared_from_this() );
					p_obj.m_skeleton.AddBone( l_bone );
				}

				break;
			}
		}

		return l_return;
	}

	//*************************************************************************************************

	Bone::Bone( Skeleton & p_skeleton )
		: m_skeleton( p_skeleton )
	{
		m_finalTransformation.set_identity();
	}

	Bone::~Bone()
	{
	}

	void Bone::AddChild( BoneSPtr p_bone )
	{
		if ( m_children.end() == std::find( m_children.begin(), m_children.end(), p_bone ) )
		{
			m_children.push_back( p_bone );
		}
	}
}
