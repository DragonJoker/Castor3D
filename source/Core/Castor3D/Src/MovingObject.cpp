#include "MovingObject.hpp"

#include "Animation.hpp"
#include "MovableObject.hpp"
#include "SceneNode.hpp"

using namespace Castor;

namespace Castor3D
{
	//*************************************************************************************************

	MovingObject::BinaryParser::BinaryParser( Path const & p_path )
		: Castor3D::BinaryParser< MovingObject >( p_path )
	{
	}

	bool MovingObject::BinaryParser::Fill( MovingObject const & p_obj, BinaryChunk & p_chunk )const
	{
		bool l_return = true;
		BinaryChunk l_chunk( eCHUNK_TYPE_MOVING_BONE );

		if ( l_return )
		{
			l_return = DoFillChunk( p_obj.GetName(), eCHUNK_TYPE_NAME, l_chunk );
		}

		if ( l_return )
		{
			l_return = MovingObjectBase::BinaryParser( m_path ).Fill( p_obj, l_chunk );
		}

		if ( l_return )
		{
			l_chunk.Finalise();
			p_chunk.AddSubChunk( l_chunk );
		}

		return l_return;
	}

	bool MovingObject::BinaryParser::Parse( MovingObject & p_obj, BinaryChunk & p_chunk )const
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
					l_return = MovingObjectBase::BinaryParser( m_path ).Parse( p_obj, l_chunk );
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

	MovingObject::MovingObject()
		: MovingObjectBase( eMOVING_OBJECT_TYPE_OBJECT )
	{
	}

	MovingObject::~MovingObject()
	{
	}

	String const & MovingObject::GetName()const
	{
		return GetObject()->GetName();
	}

	void MovingObject::DoApply()
	{
		m_finalTransform = m_cumulativeTransform;
	}

	MovingObjectBaseSPtr MovingObject::DoClone( Animation & p_animation )
	{
		auto l_return = std::make_shared< MovingObject >();
		l_return->m_object = m_object;
		p_animation.AddMovingObject( l_return, l_return );
		return l_return;
	}

	//*************************************************************************************************
}
