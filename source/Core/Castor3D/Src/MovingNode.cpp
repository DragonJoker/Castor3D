#include "MovingNode.hpp"

#include "Animation.hpp"

using namespace Castor;

namespace Castor3D
{
	//*************************************************************************************************

	MovingNode::BinaryParser::BinaryParser( Path const & p_path )
		: Castor3D::BinaryParser< MovingNode >( p_path )
	{
	}

	bool MovingNode::BinaryParser::Fill( MovingNode const & p_obj, BinaryChunk & p_chunk )const
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

	bool MovingNode::BinaryParser::Parse( MovingNode & p_obj, BinaryChunk & p_chunk )const
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
					l_return = DoParseChunk( p_obj.m_name, p_chunk );
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

	MovingNode::MovingNode( String const & p_name )
		: MovingObjectBase( eMOVING_OBJECT_TYPE_NODE )
		, m_name( p_name )
	{
	}

	MovingNode :: ~MovingNode()
	{
	}

	void MovingNode::DoApply()
	{
		m_finalTransform = m_nodeTransform;
	}

	MovingObjectBaseSPtr MovingNode::DoClone( Animation & p_animation )
	{
		auto l_return = std::make_shared< MovingNode >( m_name );
		p_animation.AddMovingObject( l_return, l_return );
		return l_return;
	}

	//*************************************************************************************************
}
