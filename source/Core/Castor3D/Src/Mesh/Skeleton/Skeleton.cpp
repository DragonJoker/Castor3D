#include "Skeleton.hpp"

#include "Engine.hpp"

#include "Animation/Skeleton/SkeletonAnimation.hpp"

using namespace castor;

namespace castor3d
{
	//*************************************************************************************************

	bool BinaryWriter< Skeleton >::doWrite( Skeleton const & p_obj )
	{
		bool result = true;

		if ( result )
		{
			result = doWriteChunk( p_obj.getGlobalInverseTransform(), ChunkType::eSkeletonGlobalInverse, m_chunk );
		}

		for ( auto bone : p_obj.m_bones )
		{
			result &= BinaryWriter< Bone >{}.write( *bone, m_chunk );
		}

		for ( auto const & it : p_obj.m_animations )
		{
			result = BinaryWriter< Animation >{}.write( *it.second, m_chunk );
		}

		return result;
	}

	//*************************************************************************************************

	bool BinaryParser< Skeleton >::doParse( Skeleton & p_obj )
	{
		bool result = true;
		BoneSPtr bone;
		BinaryChunk chunk;
		SkeletonAnimationUPtr animation;

		while ( result && doGetSubChunk( chunk ) )
		{
			switch ( chunk.getChunkType() )
			{
			case ChunkType::eSkeletonGlobalInverse:
				result = doParseChunk( p_obj.m_globalInverse, chunk );
				break;

			case ChunkType::eSkeletonBone:
				bone = std::make_shared< Bone >( p_obj );
				result = BinaryParser< Bone >{}.parse( *bone, chunk );

				if ( result )
				{
					p_obj.m_bones.push_back( bone );
				}

				break;

			case ChunkType::eAnimation:
				animation = std::make_unique< SkeletonAnimation >( p_obj );
				result = BinaryParser< Animation >{}.parse( *animation, chunk );

				if ( result )
				{
					p_obj.m_animations.insert( { animation->getName(), std::move( animation ) } );
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
		Animable::cleanupAnimations();
	}

	BoneSPtr Skeleton::createBone( String const & p_name )
	{
		auto bone = std::make_shared< Bone >( *this );
		bone->setName( p_name );
		m_bones.push_back( bone );
		return bone;
	}

	BoneSPtr Skeleton::findBone( castor::String const & p_name )const
	{
		auto it = std::find_if( begin(), end(), [&p_name]( BoneSPtr p_bone )
		{
			return p_bone->getName() == p_name;
		} );

		BoneSPtr bone;

		if ( it != end() )
		{
			bone = *it;
		}

		return bone;
	}

	void Skeleton::setBoneParent( BoneSPtr p_bone, BoneSPtr p_parent )
	{
		if ( std::find( begin(), end(), p_bone ) == end() )
		{
			CASTOR_EXCEPTION( "Skeleton::setBoneParent - Child bone is not in the Skeleton's nodes" );
		}

		if ( std::find( begin(), end(), p_parent ) == end() )
		{
			CASTOR_EXCEPTION( "Skeleton::setBoneParent - Parent bone is not in the Skeleton's nodes" );
		}

		p_parent->addChild( p_bone );
		p_bone->setParent( p_parent );
	}

	SkeletonAnimation & Skeleton::createAnimation( castor::String const & p_name )
	{
		if ( !hasAnimation( p_name ) )
		{
			doAddAnimation( std::make_shared< SkeletonAnimation >( *this, p_name ) );
		}

		return doGetAnimation< SkeletonAnimation >( p_name );
	}

	void Skeleton::computeContainers( Mesh & mesh )
	{
		auto it = m_boxes.find( &mesh );

		if ( it == m_boxes.end() )
		{
			uint32_t index = 0u;
			std::vector< BoundingBox > boxes;
			boxes.reserve( m_bones.size() );

			for ( auto & bone : m_bones )
			{
				boxes.emplace_back( bone->computeBoundingBox( mesh, index ) );
				++index;
			}
		}
	}
}
