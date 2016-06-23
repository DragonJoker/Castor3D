#include "Skeleton.hpp"

#include "Engine.hpp"

#include "Animation/Skeleton/SkeletonAnimation.hpp"
#include "Event/Frame/InitialiseEvent.hpp"
#include "Bone.hpp"

using namespace Castor;

namespace Castor3D
{
	//*************************************************************************************************

	bool BinaryWriter< Skeleton >::DoWrite( Skeleton const & p_obj )
	{
		bool l_return = true;

		if ( l_return )
		{
			l_return = DoWriteChunk( p_obj.GetGlobalInverseTransform(), eCHUNK_TYPE_SKELETON_GLOBAL_INVERSE, m_chunk );
		}

		for ( auto l_bone : p_obj.m_bones )
		{
			l_return &= BinaryWriter< Bone >{}.Write( *l_bone, m_chunk );
		}

		for ( auto const & l_it : p_obj.m_animations )
		{
			l_return = BinaryWriter< Animation >{}.Write( *l_it.second, m_chunk );
		}

		return l_return;
	}

	//*************************************************************************************************

	bool BinaryParser< Skeleton >::DoParse( Skeleton & p_obj )
	{
		bool l_return = true;
		BoneSPtr l_bone;
		BinaryChunk l_chunk;
		SkeletonAnimationUPtr l_animation;

		while ( l_return && DoGetSubChunk( l_chunk ) )
		{
			switch ( l_chunk.GetChunkType() )
			{
			case eCHUNK_TYPE_SKELETON_GLOBAL_INVERSE:
				l_return = DoParseChunk( p_obj.m_globalInverse, l_chunk );
				break;

			case eCHUNK_TYPE_SKELETON_BONE:
				l_bone = std::make_shared< Bone >( p_obj );
				l_return = BinaryParser< Bone >{}.Parse( *l_bone, l_chunk );

				if ( l_return )
				{
					p_obj.m_bones.push_back( l_bone );
				}

				break;

			case eCHUNK_TYPE_ANIMATION:
				l_animation = std::make_unique< SkeletonAnimation >( p_obj );
				l_return = BinaryParser< Animation >{}.Parse( *l_animation, l_chunk );

				if ( l_return )
				{
					p_obj.m_animations.insert( { l_animation->GetName(), std::move( l_animation ) } );
				}

				break;
			}
		}

		return l_return;
	}

	//*************************************************************************************************

	Skeleton::Skeleton( Scene & p_scene )
		: Animable{ p_scene }
		, m_globalInverse{ 1 }
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

	SkeletonAnimation & Skeleton::CreateAnimation( Castor::String const & p_name )
	{
		if ( !HasAnimation( p_name ) )
		{
			DoAddAnimation( std::make_shared< SkeletonAnimation >( *this, p_name ) );
		}

		return DoGetAnimation< SkeletonAnimation >( p_name );
	}
}
