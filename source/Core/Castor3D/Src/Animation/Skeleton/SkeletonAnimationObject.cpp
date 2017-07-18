#include "SkeletonAnimationObject.hpp"

#include "SkeletonAnimation.hpp"

#include "SkeletonAnimationBone.hpp"
#include "SkeletonAnimationNode.hpp"

using namespace Castor;

namespace Castor3D
{
	namespace
	{
		using KeyFramed = std::array< double, 17 >;

		void DoConvert( std::vector< KeyFrame > const & p_in, std::vector< KeyFramed > & p_out )
		{
			p_out.resize( p_in.size() );
			auto l_it = p_out.begin();

			for ( auto & l_in : p_in )
			{
				auto & l_out = *l_it;
				size_t l_index{ 0u };
				auto const & l_transform = l_in.GetTransform();
				l_out[l_index++] = double( l_in.GetTimeIndex().count() / 1000.0 );
				l_out[l_index++] = l_transform[0][0];
				l_out[l_index++] = l_transform[0][1];
				l_out[l_index++] = l_transform[0][2];
				l_out[l_index++] = l_transform[0][3];
				l_out[l_index++] = l_transform[1][0];
				l_out[l_index++] = l_transform[1][1];
				l_out[l_index++] = l_transform[1][2];
				l_out[l_index++] = l_transform[1][3];
				l_out[l_index++] = l_transform[2][0];
				l_out[l_index++] = l_transform[2][1];
				l_out[l_index++] = l_transform[2][2];
				l_out[l_index++] = l_transform[2][3];
				l_out[l_index++] = l_transform[3][0];
				l_out[l_index++] = l_transform[3][1];
				l_out[l_index++] = l_transform[3][2];
				l_out[l_index++] = l_transform[3][3];
				++l_it;
			}
		}

		void DoConvert( std::vector< KeyFramed > const & p_in, std::vector< KeyFrame > & p_out )
		{
			p_out.resize( p_in.size() );
			auto l_it = p_out.begin();

			for ( auto & l_in : p_in )
			{
				size_t l_index{ 0u };
				std::chrono::milliseconds l_timeIndex{ int64_t( l_in[l_index++] * 1000.0 ) };
				Matrix4x4r l_transform{ &l_in[l_index] };
				( *l_it ) = KeyFrame{ l_timeIndex, l_transform };
				++l_it;
			}
		}
	}

	//*************************************************************************************************

	bool BinaryWriter< SkeletonAnimationObject >::DoWrite( SkeletonAnimationObject const & p_obj )
	{
		bool l_result = true;

		if ( l_result )
		{
			l_result = DoWriteChunk( p_obj.m_nodeTransform, ChunkType::eMovingTransform, m_chunk );
		}

		if ( l_result )
		{
			l_result = DoWriteChunk( real( p_obj.m_length.count() ) * 1000.0_r, ChunkType::eAnimLength, m_chunk );
		}

		if ( !p_obj.m_keyframes.empty() )
		{
			if ( l_result )
			{
				l_result = DoWriteChunk( uint32_t( p_obj.m_keyframes.size() ), ChunkType::eKeyframeCount, m_chunk );
			}

			if ( l_result )
			{
				std::vector< KeyFramed > l_keyFrames;
				DoConvert( p_obj.m_keyframes, l_keyFrames );
				l_result = DoWriteChunk( l_keyFrames, ChunkType::eKeyframes, m_chunk );
			}
		}

		for ( auto const & l_moving : p_obj.m_children )
		{
			switch ( l_moving->GetType() )
			{
			case SkeletonAnimationObjectType::eNode:
				l_result &= BinaryWriter< SkeletonAnimationNode >{}.Write( *std::static_pointer_cast< SkeletonAnimationNode >( l_moving ), m_chunk );
				break;

			case SkeletonAnimationObjectType::eBone:
				l_result &= BinaryWriter< SkeletonAnimationBone >{}.Write( *std::static_pointer_cast< SkeletonAnimationBone >( l_moving ), m_chunk );
				break;
			}
		}

		return l_result;
	}

	//*************************************************************************************************

	bool BinaryParser< SkeletonAnimationObject >::DoParse( SkeletonAnimationObject & p_obj )
	{
		bool l_result = true;
		Matrix4x4r l_transform;
		std::vector< KeyFramed > l_keyframes;
		SkeletonAnimationNodeSPtr l_node;
		SkeletonAnimationObjectSPtr l_object;
		SkeletonAnimationBoneSPtr l_bone;
		BinaryChunk l_chunk;
		uint32_t l_count{ 0 };
		real l_length{ 0.0_r };

		while ( l_result && DoGetSubChunk( l_chunk ) )
		{
			switch ( l_chunk.GetChunkType() )
			{
			case ChunkType::eMovingTransform:
				l_result = DoParseChunk( p_obj.m_nodeTransform, l_chunk );
				break;

			case ChunkType::eKeyframeCount:
				l_result = DoParseChunk( l_count, l_chunk );

				if ( l_result )
				{
					l_keyframes.resize( l_count );
				}

				break;

			case ChunkType::eKeyframes:
				l_result = DoParseChunk( l_keyframes, l_chunk );

				if ( l_result )
				{
					DoConvert( l_keyframes, p_obj.m_keyframes );
				}

				break;

			case ChunkType::eAnimLength:
				l_result = DoParseChunk( l_length, l_chunk );
				p_obj.m_length = std::chrono::milliseconds{ int64_t( l_length / 1000 ) };
				break;

			case ChunkType::eSkeletonAnimationBone:
				l_bone = std::make_shared< SkeletonAnimationBone >( *p_obj.GetOwner() );
				l_result = BinaryParser< SkeletonAnimationBone >{}.Parse( *l_bone, l_chunk );

				if ( l_result )
				{
					p_obj.AddChild( l_bone );
					p_obj.GetOwner()->AddObject( l_bone, p_obj.shared_from_this() );
				}

				break;

			case ChunkType::eSkeletonAnimationNode:
				l_node = std::make_shared< SkeletonAnimationNode >( *p_obj.GetOwner() );
				l_result = BinaryParser< SkeletonAnimationNode >{}.Parse( *l_node, l_chunk );

				if ( l_result )
				{
					p_obj.AddChild( l_node );
					p_obj.GetOwner()->AddObject( l_node, p_obj.shared_from_this() );
				}

				break;
			}
		}

		return l_result;
	}

	//*************************************************************************************************

	SkeletonAnimationObject::SkeletonAnimationObject( SkeletonAnimation & p_animation, SkeletonAnimationObjectType p_type )
		: OwnedBy< SkeletonAnimation >{ p_animation }
		, m_type{ p_type }
	{
	}

	SkeletonAnimationObject::~SkeletonAnimationObject()
	{
	}

	void SkeletonAnimationObject::AddChild( SkeletonAnimationObjectSPtr p_object )
	{
		REQUIRE( p_object.get() != this );
		p_object->m_parent = shared_from_this();
		m_children.push_back( p_object );
	}

	KeyFrame & SkeletonAnimationObject::AddKeyFrame( std::chrono::milliseconds const & p_from
		, Point3r const & p_translate
		, Quaternion const & p_rotate
		, Point3r const & p_scale )
	{
		auto l_it = std::find_if( m_keyframes.begin(), m_keyframes.end(), [&p_from]( KeyFrame & p_keyframe )
		{
			return p_keyframe.GetTimeIndex() >= p_from;
		} );

		if ( l_it == m_keyframes.end() )
		{
			m_length = p_from;
			l_it = m_keyframes.insert( m_keyframes.end(), KeyFrame{ p_from, p_translate, p_rotate, p_scale } );
		}
		else if ( l_it->GetTimeIndex() > p_from )
		{
			if ( l_it != m_keyframes.begin() )
			{
				--l_it;
			}

			l_it = m_keyframes.insert( l_it, KeyFrame{ p_from, p_translate, p_rotate, p_scale } );
		}

		return *l_it;
	}

	void SkeletonAnimationObject::RemoveKeyFrame( std::chrono::milliseconds const & p_time )
	{
		auto l_it = std::find_if( m_keyframes.begin(), m_keyframes.end(), [&p_time]( KeyFrame const & p_keyframe )
		{
			return p_keyframe.GetTimeIndex() == p_time;
		} );

		if ( l_it != m_keyframes.end() )
		{
			m_keyframes.erase( l_it );
		}
	}

	//*************************************************************************************************
}
