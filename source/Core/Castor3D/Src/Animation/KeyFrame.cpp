#include "KeyFrame.hpp"

using namespace Castor;

namespace Castor3D
{
	//*************************************************************************************************

	KeyFrame::BinaryWriter::BinaryWriter( Path const & p_path )
		: Castor3D::BinaryWriter< KeyFrame >( p_path )
	{
	}

	bool KeyFrame::BinaryWriter::DoWrite( KeyFrame const & p_obj, BinaryChunk & p_chunk )const
	{
		bool l_return = true;
		BinaryChunk l_chunk( eCHUNK_TYPE_KEYFRAME );

		if ( l_return )
		{
			l_return &= DoWriteChunk( p_obj.GetTimeIndex(), eCHUNK_TYPE_KEYFRAME_TIME, l_chunk );
		}

		if ( l_return )
		{
			l_return &= DoWriteChunk( p_obj.GetScale(), eCHUNK_TYPE_KEYFRAME_SCALE, l_chunk );
		}

		if ( l_return )
		{
			l_return &= DoWriteChunk( p_obj.GetTranslate(), eCHUNK_TYPE_KEYFRAME_TRANSLATE, l_chunk );
		}

		if ( l_return )
		{
			l_return &= DoWriteChunk( p_obj.GetRotate(), eCHUNK_TYPE_KEYFRAME_ROTATE, l_chunk );
		}

		if ( l_return )
		{
			p_chunk.AddSubChunk( l_chunk );
		}

		return l_return;
	}

	//*************************************************************************************************

	KeyFrame::BinaryParser::BinaryParser( Path const & p_path )
		: Castor3D::BinaryParser< KeyFrame >( p_path )
	{
	}

	bool KeyFrame::BinaryParser::DoParse( KeyFrame & p_obj, BinaryChunk & p_chunk )const
	{
		bool l_return = true;
		Matrix4x4r l_transform;
		Point3r l_point;
		Quaternion l_quat;
		real l_time;

		while ( p_chunk.CheckAvailable( 1 ) )
		{
			BinaryChunk l_chunk;
			l_return = p_chunk.GetSubChunk( l_chunk );

			if ( l_return )
			{
				switch ( p_chunk.GetChunkType() )
				{
				case eCHUNK_TYPE_KEYFRAME_TIME:
					l_return = DoParseChunk( l_time, p_chunk );
					p_obj.SetTimeIndex( l_time );
					break;

				case eCHUNK_TYPE_KEYFRAME_SCALE:
					l_return = DoParseChunk( l_point, p_chunk );
					p_obj.SetScale( l_point );
					break;

				case eCHUNK_TYPE_KEYFRAME_TRANSLATE:
					l_return = DoParseChunk( l_point, p_chunk );
					p_obj.SetTranslate( l_point );
					break;

				case eCHUNK_TYPE_KEYFRAME_ROTATE:
					l_return = DoParseChunk( l_quat, p_chunk );
					p_obj.SetRotate( l_quat );
					break;

				default:
					l_return = KeyFrame::BinaryParser( m_path ).Parse( p_obj, p_chunk );
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
}
