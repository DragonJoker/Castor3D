#include "KeyFrame.hpp"

using namespace Castor;

namespace Castor3D
{
	//*************************************************************************************************

	bool BinaryWriter< KeyFrame >::DoWrite( KeyFrame const & p_obj )
	{
		bool l_return = true;

		if ( l_return )
		{
			l_return &= DoWriteChunk( p_obj.GetTimeIndex(), eCHUNK_TYPE_KEYFRAME_TIME, m_chunk );
		}

		if ( l_return )
		{
			l_return &= DoWriteChunk( p_obj.GetScale(), eCHUNK_TYPE_KEYFRAME_SCALE, m_chunk );
		}

		if ( l_return )
		{
			l_return &= DoWriteChunk( p_obj.GetTranslate(), eCHUNK_TYPE_KEYFRAME_TRANSLATE, m_chunk );
		}

		if ( l_return )
		{
			l_return &= DoWriteChunk( p_obj.GetRotate(), eCHUNK_TYPE_KEYFRAME_ROTATE, m_chunk );
		}

		return l_return;
	}

	//*************************************************************************************************

	bool BinaryParser< KeyFrame >::DoParse( KeyFrame & p_obj )
	{
		bool l_return = true;
		Matrix4x4r l_transform;
		Point3r l_point;
		Quaternion l_quat;
		real l_time;
		BinaryChunk l_chunk;

		while ( l_return && DoGetSubChunk( l_chunk ) )
		{
			switch ( l_chunk.GetChunkType() )
			{
			case eCHUNK_TYPE_KEYFRAME_TIME:
				l_return = DoParseChunk( l_time, l_chunk );
				p_obj.SetTimeIndex( l_time );
				break;

			case eCHUNK_TYPE_KEYFRAME_SCALE:
				l_return = DoParseChunk( l_point, l_chunk );
				p_obj.SetScale( l_point );
				break;

			case eCHUNK_TYPE_KEYFRAME_TRANSLATE:
				l_return = DoParseChunk( l_point, l_chunk );
				p_obj.SetTranslate( l_point );
				break;

			case eCHUNK_TYPE_KEYFRAME_ROTATE:
				l_return = DoParseChunk( l_quat, l_chunk );
				p_obj.SetRotate( l_quat );
				break;
			}
		}

		return l_return;
	}

	//*************************************************************************************************
}
