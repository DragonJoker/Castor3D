#include "SkeletonAnimationBone.hpp"

#include "SkeletonAnimation.hpp"
#include "Mesh/Skeleton/Skeleton.hpp"

using namespace Castor;

namespace Castor3D
{
	//*************************************************************************************************

	bool BinaryWriter< SkeletonAnimationBone >::DoWrite( SkeletonAnimationBone const & p_obj )
	{
		bool result = true;

		if ( result )
		{
			result = DoWriteChunk( p_obj.GetName(), ChunkType::eName, m_chunk );
		}

		if ( result )
		{
			result = BinaryWriter< SkeletonAnimationObject >{}.Write( p_obj, m_chunk );
		}

		return result;
	}

	//*************************************************************************************************

	bool BinaryParser< SkeletonAnimationBone >::DoParse( SkeletonAnimationBone & p_obj )
	{
		bool result = true;
		String name;
		BinaryChunk chunk;

		while ( result && DoGetSubChunk( chunk ) )
		{
			switch ( chunk.GetChunkType() )
			{
			case ChunkType::eName:
				result = DoParseChunk( name, chunk );

				if ( result )
				{
					auto bone = static_cast< Skeleton * >( p_obj.GetOwner()->GetOwner() )->FindBone( name );

					if ( bone )
					{
						p_obj.SetBone( bone );
					}
					else
					{
						Logger::LogError( cuT( "Couldn't find bone " ) + name + cuT( " in skeleton" ) );
					}
				}

				break;

			case ChunkType::eAnimationObject:
				result = BinaryParser< SkeletonAnimationObject >{}.Parse( p_obj, chunk );
				break;
			}
		}

		return result;
	}

	//*************************************************************************************************

	SkeletonAnimationBone::SkeletonAnimationBone( SkeletonAnimation & p_animation )
		: SkeletonAnimationObject{ p_animation, SkeletonAnimationObjectType::eBone }
	{
	}

	SkeletonAnimationBone :: ~SkeletonAnimationBone()
	{
	}

	String const & SkeletonAnimationBone::GetName()const
	{
		return GetBone()->GetName();
	}

	//*************************************************************************************************
}
