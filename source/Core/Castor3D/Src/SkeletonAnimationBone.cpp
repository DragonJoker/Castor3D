#include "SkeletonAnimationBone.hpp"

#include "Animation.hpp"
#include "Bone.hpp"

using namespace Castor;

namespace Castor3D
{
	//*************************************************************************************************

	SkeletonAnimationBone::BinaryParser::BinaryParser( Path const & p_path )
		: Castor3D::BinaryParser< SkeletonAnimationBone >( p_path )
	{
	}

	bool SkeletonAnimationBone::BinaryParser::Fill( SkeletonAnimationBone const & p_obj, BinaryChunk & p_chunk )const
	{
		bool l_return = true;
		BinaryChunk l_chunk( eCHUNK_TYPE_MOVING_BONE );

		if ( l_return )
		{
			l_return = DoFillChunk( p_obj.GetName(), eCHUNK_TYPE_NAME, l_chunk );
		}

		if ( l_return )
		{
			l_return = AnimationObjectBase::BinaryParser( m_path ).Fill( p_obj, l_chunk );
		}

		if ( l_return )
		{
			l_chunk.Finalise();
			p_chunk.AddSubChunk( l_chunk );
		}

		return l_return;
	}

	bool SkeletonAnimationBone::BinaryParser::Parse( SkeletonAnimationBone & p_obj, BinaryChunk & p_chunk )const
	{
		bool l_return = true;
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
					l_return = DoParseChunk( l_name, p_chunk );
					// TODO Find bone somewhere.
					break;

				default:
					l_return = AnimationObjectBase::BinaryParser( m_path ).Parse( p_obj, l_chunk );
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

	SkeletonAnimationBone::SkeletonAnimationBone()
		: AnimationObjectBase( eANIMATION_OBJECT_TYPE_BONE )
	{
	}

	SkeletonAnimationBone :: ~SkeletonAnimationBone()
	{
	}

	String const & SkeletonAnimationBone::GetName()const
	{
		return GetBone()->GetName();
	}

	void SkeletonAnimationBone::DoApply()
	{
		BoneSPtr l_bone = GetBone();

		if ( l_bone )
		{
			m_finalTransform = m_cumulativeTransform * l_bone->GetOffsetMatrix();
		}
	}

	AnimationObjectBaseSPtr SkeletonAnimationBone::DoClone( Animation & p_animation )
	{
		auto l_return = std::make_shared< SkeletonAnimationBone >();
		l_return->m_bone = m_bone;
		p_animation.AddObject( l_return, l_return );
		return l_return;
	}

	//*************************************************************************************************
}
