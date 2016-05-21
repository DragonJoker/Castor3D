#include "SkeletonAnimationObject.hpp"

#include "Animation.hpp"

#include "Scene/MovableObject.hpp"
#include "Scene/SceneNode.hpp"

using namespace Castor;

namespace Castor3D
{
	//*************************************************************************************************

	SkeletonAnimationObject::BinaryParser::BinaryParser( Path const & p_path )
		: Castor3D::BinaryParser< SkeletonAnimationObject >( p_path )
	{
	}

	bool SkeletonAnimationObject::BinaryParser::Fill( SkeletonAnimationObject const & p_obj, BinaryChunk & p_chunk )const
	{
		bool l_return = true;
		BinaryChunk l_chunk( eCHUNK_TYPE_MOVING_BONE );

		if ( l_return )
		{
			l_return = DoFillChunk( p_obj.GetName(), eCHUNK_TYPE_NAME, l_chunk );
		}

		if ( l_return )
		{
			l_return = AnimationObject::BinaryParser( m_path ).Fill( p_obj, l_chunk );
		}

		if ( l_return )
		{
			l_chunk.Finalise();
			p_chunk.AddSubChunk( l_chunk );
		}

		return l_return;
	}

	bool SkeletonAnimationObject::BinaryParser::Parse( SkeletonAnimationObject & p_obj, BinaryChunk & p_chunk )const
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
					l_return = AnimationObject::BinaryParser( m_path ).Parse( p_obj, l_chunk );
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

	SkeletonAnimationObject::SkeletonAnimationObject()
		: AnimationObject( eANIMATION_OBJECT_TYPE_OBJECT )
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
		auto l_return = std::make_shared< SkeletonAnimationObject >();
		l_return->m_object = m_object;
		p_animation.AddObject( l_return, l_return );
		return l_return;
	}

	//*************************************************************************************************
}
