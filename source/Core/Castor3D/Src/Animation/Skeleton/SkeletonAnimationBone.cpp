#include "SkeletonAnimationBone.hpp"

#include "SkeletonAnimation.hpp"
#include "Mesh/Skeleton/Skeleton.hpp"

using namespace Castor;

namespace Castor3D
{
	//*************************************************************************************************

	bool BinaryWriter< SkeletonAnimationBone >::DoWrite( SkeletonAnimationBone const & p_obj )
	{
		bool l_result = true;

		if ( l_result )
		{
			l_result = DoWriteChunk( p_obj.GetName(), ChunkType::eName, m_chunk );
		}

		if ( l_result )
		{
			l_result = BinaryWriter< SkeletonAnimationObject >{}.Write( p_obj, m_chunk );
		}

		return l_result;
	}

	//*************************************************************************************************

	bool BinaryParser< SkeletonAnimationBone >::DoParse( SkeletonAnimationBone & p_obj )
	{
		bool l_result = true;
		String l_name;
		BinaryChunk l_chunk;

		while ( l_result && DoGetSubChunk( l_chunk ) )
		{
			switch ( l_chunk.GetChunkType() )
			{
			case ChunkType::eName:
				l_result = DoParseChunk( l_name, l_chunk );

				if ( l_result )
				{
					auto l_bone = static_cast< Skeleton * >( p_obj.GetOwner()->GetOwner() )->FindBone( l_name );

					if ( l_bone )
					{
						p_obj.SetBone( l_bone );
					}
					else
					{
						Logger::LogError( cuT( "Couldn't find bone " ) + l_name + cuT( " in skeleton" ) );
					}
				}

				break;

			case ChunkType::eAnimationObject:
				l_result = BinaryParser< SkeletonAnimationObject >{}.Parse( p_obj, l_chunk );
				break;
			}
		}

		return l_result;
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
