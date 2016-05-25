#include "Skeleton.hpp"

#include "Animation.hpp"
#include "Bone.hpp"

using namespace Castor;

namespace Castor3D
{
	//*************************************************************************************************

	Skeleton::BinaryWriter::BinaryWriter( Path const & p_path )
		: Castor3D::BinaryWriter< Skeleton >( p_path )
	{
	}

	bool Skeleton::BinaryWriter::DoWrite( Skeleton const & p_obj, BinaryChunk & p_chunk )const
	{
		bool l_return = true;
		BinaryChunk l_chunk( eCHUNK_TYPE_SKELETON );

		if ( l_return )
		{
			l_return = DoWriteChunk( p_obj.GetGlobalInverseTransform(), eCHUNK_TYPE_SKELETON_GLOBAL_INVERSE, l_chunk );
		}

		for ( auto && l_bone : p_obj.m_bones )
		{
			// Write only root bones, since bones write their children.
			if ( !l_bone->GetParent() )
			{
				l_return &= Bone::BinaryWriter{ m_path }.Write( *l_bone, l_chunk );
			}
		}

		if ( l_return )
		{
			l_return = Animable::BinaryWriter{ m_path }.Write( p_obj, l_chunk );
		}

		if ( l_return )
		{
			l_chunk.Finalise();
			p_chunk.AddSubChunk( l_chunk );
		}

		return l_return;
	}

	//*************************************************************************************************

	Skeleton::BinaryParser::BinaryParser( Path const & p_path )
		: Castor3D::BinaryParser< Skeleton >( p_path )
	{
	}

	bool Skeleton::BinaryParser::DoParse( Skeleton & p_obj, BinaryChunk & p_chunk )const
	{
		bool l_return = true;
		BoneSPtr l_bone;

		while ( p_chunk.CheckAvailable( 1 ) )
		{
			BinaryChunk l_chunk;
			l_return = p_chunk.GetSubChunk( l_chunk );

			if ( l_return )
			{
				switch ( l_chunk.GetChunkType() )
				{
				case eCHUNK_TYPE_SKELETON_GLOBAL_INVERSE:
					l_return = DoParseChunk( p_obj.m_globalInverse, l_chunk );
					break;

				case eCHUNK_TYPE_SKELETON_BONE:
					l_bone = std::make_shared< Bone >( p_obj );
					l_return = Bone::BinaryParser{ m_path }.Parse( *l_bone, l_chunk );

					if ( l_return )
					{
						p_obj.m_bones.push_back( l_bone );
					}

					break;

				default:
					l_return = Animable::BinaryParser{ m_path }.Parse( p_obj, l_chunk );
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

	Skeleton::Skeleton()
		: m_globalInverse( 1 )
	{
	}

	Skeleton::~Skeleton()
	{
		Animable::CleanupAnimations();
	}

	void Skeleton::AddBone( BoneSPtr p_bone )
	{
		m_bones.push_back( p_bone );
	}

	BoneSPtr Skeleton::FindBone( Castor::String const & p_name )const
	{
		auto l_it = std::find_if( begin(), end(), [&p_name]( BoneSPtr p_bone )
		{
			return p_bone->GetName() == p_name;
		} );

		BoneSPtr l_bone;

		if ( l_it != end() )
		{
			l_bone = *l_it;
		}

		return l_bone;
	}

	void Skeleton::SetBoneParent( BoneSPtr p_bone, BoneSPtr p_parent )
	{
		if ( std::find( begin(), end(), p_bone ) == end() )
		{
			CASTOR_EXCEPTION( "Skeleton::SetBoneParent - Child bone is not in the Skeleton's nodes" );
		}

		if ( std::find( begin(), end(), p_parent ) == end() )
		{
			CASTOR_EXCEPTION( "Skeleton::SetBoneParent - Parent bone is not in the Skeleton's nodes" );
		}

		p_parent->AddChild( p_bone );
		p_bone->SetParent( p_parent );
	}
}
