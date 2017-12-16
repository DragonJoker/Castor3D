#include "Skeleton.hpp"

#include "Engine.hpp"

#include "Animation/Skeleton/SkeletonAnimation.hpp"

using namespace castor;

namespace castor3d
{
	//*************************************************************************************************

	bool BinaryWriter< Skeleton >::doWrite( Skeleton const & obj )
	{
		bool result = true;

		if ( result )
		{
			result = doWriteChunk( obj.getGlobalInverseTransform(), ChunkType::eSkeletonGlobalInverse, m_chunk );
		}

		for ( auto bone : obj.m_bones )
		{
			result &= BinaryWriter< Bone >{}.write( *bone, m_chunk );
		}

		for ( auto const & it : obj.m_animations )
		{
			result = BinaryWriter< Animation >{}.write( *it.second, m_chunk );
		}

		return result;
	}

	//*************************************************************************************************

	bool BinaryParser< Skeleton >::doParse( Skeleton & obj )
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
				result = doParseChunk( obj.m_globalInverse, chunk );
				break;

			case ChunkType::eSkeletonBone:
				bone = std::make_shared< Bone >( obj, Matrix4x4r{ 1.0_r } );
				result = BinaryParser< Bone >{}.parse( *bone, chunk );

				if ( result )
				{
					obj.m_bones.push_back( bone );
				}

				break;

			case ChunkType::eAnimation:
				animation = std::make_unique< SkeletonAnimation >( obj );
				result = BinaryParser< Animation >{}.parse( *animation, chunk );

				if ( result )
				{
					obj.m_animations.insert( { animation->getName(), std::move( animation ) } );
				}

				break;
			}
		}

		return result;
	}

	//*************************************************************************************************

	Skeleton::Skeleton( Scene & scene )
		: Animable{ scene }
		, m_globalInverse{ 1 }
	{
	}

	Skeleton::~Skeleton()
	{
		Animable::cleanupAnimations();
	}

	BoneSPtr Skeleton::createBone( String const & name
		, Matrix4x4r const & offset )
	{
		auto bone = std::make_shared< Bone >( *this, offset );
		bone->setName( name );
		m_bones.push_back( bone );
		return bone;
	}

	BoneSPtr Skeleton::findBone( castor::String const & name )const
	{
		auto it = std::find_if( begin(), end(), [&name]( BoneSPtr bone )
		{
			return bone->getName() == name;
		} );

		BoneSPtr bone;

		if ( it != end() )
		{
			bone = *it;
		}

		return bone;
	}

	void Skeleton::setBoneParent( BoneSPtr bone, BoneSPtr parent )
	{
		if ( std::find( begin(), end(), bone ) == end() )
		{
			CASTOR_EXCEPTION( "Skeleton::setBoneParent - Child bone is not in the Skeleton's nodes" );
		}

		if ( std::find( begin(), end(), parent ) == end() )
		{
			CASTOR_EXCEPTION( "Skeleton::setBoneParent - Parent bone is not in the Skeleton's nodes" );
		}

		parent->addChild( bone );
		bone->setParent( parent );
	}

	SkeletonAnimation & Skeleton::createAnimation( castor::String const & name )
	{
		if ( !hasAnimation( name ) )
		{
			doAddAnimation( std::make_shared< SkeletonAnimation >( *this, name ) );
		}

		return doGetAnimation< SkeletonAnimation >( name );
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
