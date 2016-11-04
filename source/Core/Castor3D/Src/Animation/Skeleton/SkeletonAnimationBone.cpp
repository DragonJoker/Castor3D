#include "SkeletonAnimationBone.hpp"

#include "SkeletonAnimation.hpp"
#include "Mesh/Skeleton/Skeleton.hpp"
#include "Mesh/Skeleton/Bone.hpp"

using namespace Castor;

namespace Castor3D
{
	//*************************************************************************************************

	bool BinaryWriter< SkeletonAnimationBone >::DoWrite( SkeletonAnimationBone const & p_obj )
	{
		bool l_return = true;

		if ( l_return )
		{
			l_return = DoWriteChunk( p_obj.GetName(), ChunkType::eName, m_chunk );
		}

		if ( l_return )
		{
			l_return = BinaryWriter< SkeletonAnimationObject >{}.Write( p_obj, m_chunk );
		}

		return l_return;
	}

	//*************************************************************************************************

	bool BinaryParser< SkeletonAnimationBone >::DoParse( SkeletonAnimationBone & p_obj )
	{
		bool l_return = true;
		String l_name;
		BinaryChunk l_chunk;

		while ( l_return && DoGetSubChunk( l_chunk ) )
		{
			switch ( l_chunk.GetChunkType() )
			{
			case ChunkType::eName:
				l_return = DoParseChunk( l_name, l_chunk );

				if ( l_return )
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
				l_return = BinaryParser< SkeletonAnimationObject >{}.Parse( p_obj, l_chunk );
				break;
			}
		}

		return l_return;
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
