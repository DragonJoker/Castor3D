#include "Skeleton.hpp"

#include "Engine.hpp"

#include "Animation/Skeleton/SkeletonAnimation.hpp"

using namespace Castor;

namespace Castor3D
{
	//*************************************************************************************************

	bool BinaryWriter< Skeleton >::DoWrite( Skeleton const & p_obj )
	{
		bool result = true;

		if ( result )
		{
			result = DoWriteChunk( p_obj.GetGlobalInverseTransform(), ChunkType::eSkeletonGlobalInverse, m_chunk );
		}

		for ( auto bone : p_obj.m_bones )
		{
			result &= BinaryWriter< Bone >{}.Write( *bone, m_chunk );
		}

		for ( auto const & it : p_obj.m_animations )
		{
			result = BinaryWriter< Animation >{}.Write( *it.second, m_chunk );
		}

		return result;
	}

	//*************************************************************************************************

	bool BinaryParser< Skeleton >::DoParse( Skeleton & p_obj )
	{
		bool result = true;
		BoneSPtr bone;
		BinaryChunk chunk;
		SkeletonAnimationUPtr animation;

		while ( result && DoGetSubChunk( chunk ) )
		{
			switch ( chunk.GetChunkType() )
			{
			case ChunkType::eSkeletonGlobalInverse:
				result = DoParseChunk( p_obj.m_globalInverse, chunk );
				break;

			case ChunkType::eSkeletonBone:
				bone = std::make_shared< Bone >( p_obj );
				result = BinaryParser< Bone >{}.Parse( *bone, chunk );

				if ( result )
				{
					p_obj.m_bones.push_back( bone );
				}

				break;

			case ChunkType::eAnimation:
				animation = std::make_unique< SkeletonAnimation >( p_obj );
				result = BinaryParser< Animation >{}.Parse( *animation, chunk );

				if ( result )
				{
					p_obj.m_animations.insert( { animation->GetName(), std::move( animation ) } );
				}

				break;
			}
		}

		return result;
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

	BoneSPtr Skeleton::CreateBone( String const & p_name )
	{
		auto bone = std::make_shared< Bone >( *this );
		bone->SetName( p_name );
		m_bones.push_back( bone );
		return bone;
	}

	BoneSPtr Skeleton::FindBone( Castor::String const & p_name )const
	{
		auto it = std::find_if( begin(), end(), [&p_name]( BoneSPtr p_bone )
		{
			return p_bone->GetName() == p_name;
		} );

		BoneSPtr bone;

		if ( it != end() )
		{
			bone = *it;
		}

		return bone;
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
