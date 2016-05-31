#include "SkeletonAnimationObject.hpp"

#include "Animation.hpp"

#include "Scene/MovableObject.hpp"
#include "Scene/SceneNode.hpp"

using namespace Castor;

namespace Castor3D
{
	//*************************************************************************************************

	bool BinaryWriter< SkeletonAnimationObject >::DoWrite( SkeletonAnimationObject const & p_obj )
	{
		bool l_return = true;

		if ( l_return )
		{
			l_return = DoWriteChunk( p_obj.GetName(), eCHUNK_TYPE_NAME, m_chunk );
		}

		if ( l_return )
		{
			l_return = BinaryWriter< AnimationObject >{}.Write( p_obj, m_chunk );
		}

		return l_return;
	}

	//*************************************************************************************************

	bool BinaryParser< SkeletonAnimationObject >::DoParse( SkeletonAnimationObject & p_obj )
	{
		bool l_return = true;
		String l_name;
		BinaryChunk l_chunk;

		while ( l_return && DoGetSubChunk( l_chunk ) )
		{
			switch ( l_chunk.GetChunkType() )
			{
			case eCHUNK_TYPE_NAME:
				l_return = DoParseChunk( l_name, l_chunk );
				// TODO Find object somewhere.
				break;

			case eCHUNK_TYPE_ANIMATION_OBJECT:
				l_return = BinaryParser< AnimationObject >{}.Parse( p_obj, l_chunk );
				break;
			}
		}

		return l_return;
	}

	//*************************************************************************************************

	SkeletonAnimationObject::SkeletonAnimationObject( Animation & p_animation )
		: AnimationObject{ p_animation, eANIMATION_OBJECT_TYPE_OBJECT }
	{
	}

	SkeletonAnimationObject::~SkeletonAnimationObject()
	{
	}

	String const & SkeletonAnimationObject::GetName()const
	{
		return GetObject()->GetName();
	}

	void SkeletonAnimationObject::DoApply()
	{
		m_finalTransform = m_cumulativeTransform;
	}

	AnimationObjectSPtr SkeletonAnimationObject::DoClone( Animation & p_animation )
	{
		auto l_return = std::make_shared< SkeletonAnimationObject >( p_animation );
		l_return->m_object = m_object;
		p_animation.AddObject( l_return, l_return );
		return l_return;
	}

	//*************************************************************************************************
}
