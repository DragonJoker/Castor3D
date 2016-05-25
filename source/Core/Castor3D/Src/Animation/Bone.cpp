#include "Bone.hpp"

#include "Skeleton.hpp"

using namespace Castor;

namespace Castor3D
{
	//*************************************************************************************************

	Bone::BinaryWriter::BinaryWriter( Path const & p_path )
		: Castor3D::BinaryWriter< Bone >( p_path )
	{
	}

	bool Bone::BinaryWriter::DoWrite( Bone const & p_obj, BinaryChunk & p_chunk )const
	{
		bool l_return = true;
		BinaryChunk l_chunk( eCHUNK_TYPE_SKELETON_BONE );

		if ( l_return )
		{
			l_return = DoWriteChunk( p_obj.GetOffsetMatrix(), eCHUNK_TYPE_BONE_OFFSET_MATRIX, l_chunk );
		}

		for ( auto && l_bone : p_obj.m_children )
		{
			l_return &= Bone::BinaryWriter{ m_path }.Write( *l_bone, l_chunk );
		}

		if ( l_return )
		{
			l_chunk.Finalise();
			p_chunk.AddSubChunk( l_chunk );
		}

		return l_return;
	}

	//*************************************************************************************************

	Bone::BinaryParser::BinaryParser( Path const & p_path )
		: Castor3D::BinaryParser< Bone >( p_path )
	{
	}

	bool Bone::BinaryParser::DoParse( Bone & p_obj, BinaryChunk & p_chunk )const
	{
		bool l_return = true;
		BoneSPtr l_bone;
		String l_name;

		while ( p_chunk.CheckAvailable( 1 ) )
		{
			BinaryChunk l_chunk;
			l_return = p_chunk.GetSubChunk( l_chunk );

			if ( l_return )
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

				case eCHUNK_TYPE_SKELETON_BONE:
					l_bone = std::make_shared< Bone >( p_obj.m_skeleton );
					l_return = Bone::BinaryParser{ m_path }.Parse( *l_bone, l_chunk );

					if ( l_return )
					{
						p_obj.m_children.push_back( l_bone );
						l_bone->SetParent( p_obj.shared_from_this() );
						p_obj.m_skeleton.AddBone( l_bone );
					}

					break;
				}
			}

			if ( !l_return )
			{
				p_chunk.EndParse();
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
