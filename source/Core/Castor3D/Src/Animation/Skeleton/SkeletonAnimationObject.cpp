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
			auto it = p_out.begin();

			for ( auto & in : p_in )
			{
				auto & out = *it;
				size_t index{ 0u };
				auto const & transform = in.GetTransform();
				out[index++] = double( in.GetTimeIndex().count() / 1000.0 );
				out[index++] = transform[0][0];
				out[index++] = transform[0][1];
				out[index++] = transform[0][2];
				out[index++] = transform[0][3];
				out[index++] = transform[1][0];
				out[index++] = transform[1][1];
				out[index++] = transform[1][2];
				out[index++] = transform[1][3];
				out[index++] = transform[2][0];
				out[index++] = transform[2][1];
				out[index++] = transform[2][2];
				out[index++] = transform[2][3];
				out[index++] = transform[3][0];
				out[index++] = transform[3][1];
				out[index++] = transform[3][2];
				out[index++] = transform[3][3];
				++it;
			}
		}

		void DoConvert( std::vector< KeyFramed > const & p_in, std::vector< KeyFrame > & p_out )
		{
			p_out.resize( p_in.size() );
			auto it = p_out.begin();

			for ( auto & in : p_in )
			{
				size_t index{ 0u };
				std::chrono::milliseconds timeIndex{ int64_t( in[index++] * 1000.0 ) };
				Matrix4x4r transform{ &in[index] };
				( *it ) = KeyFrame{ timeIndex, transform };
				++it;
			}
		}
	}

	//*************************************************************************************************

	bool BinaryWriter< SkeletonAnimationObject >::DoWrite( SkeletonAnimationObject const & p_obj )
	{
		bool result = true;

		if ( result )
		{
			result = DoWriteChunk( p_obj.m_nodeTransform, ChunkType::eMovingTransform, m_chunk );
		}

		if ( result )
		{
			result = DoWriteChunk( real( p_obj.m_length.count() ) * 1000.0_r, ChunkType::eAnimLength, m_chunk );
		}

		if ( !p_obj.m_keyframes.empty() )
		{
			if ( result )
			{
				result = DoWriteChunk( uint32_t( p_obj.m_keyframes.size() ), ChunkType::eKeyframeCount, m_chunk );
			}

			if ( result )
			{
				std::vector< KeyFramed > keyFrames;
				DoConvert( p_obj.m_keyframes, keyFrames );
				result = DoWriteChunk( keyFrames, ChunkType::eKeyframes, m_chunk );
			}
		}

		for ( auto const & moving : p_obj.m_children )
		{
			switch ( moving->GetType() )
			{
			case SkeletonAnimationObjectType::eNode:
				result &= BinaryWriter< SkeletonAnimationNode >{}.Write( *std::static_pointer_cast< SkeletonAnimationNode >( moving ), m_chunk );
				break;

			case SkeletonAnimationObjectType::eBone:
				result &= BinaryWriter< SkeletonAnimationBone >{}.Write( *std::static_pointer_cast< SkeletonAnimationBone >( moving ), m_chunk );
				break;
			}
		}

		return result;
	}

	//*************************************************************************************************

	bool BinaryParser< SkeletonAnimationObject >::DoParse( SkeletonAnimationObject & p_obj )
	{
		bool result = true;
		Matrix4x4r transform;
		std::vector< KeyFramed > keyframes;
		SkeletonAnimationNodeSPtr node;
		SkeletonAnimationObjectSPtr object;
		SkeletonAnimationBoneSPtr bone;
		BinaryChunk chunk;
		uint32_t count{ 0 };
		real length{ 0.0_r };

		while ( result && DoGetSubChunk( chunk ) )
		{
			switch ( chunk.GetChunkType() )
			{
			case ChunkType::eMovingTransform:
				result = DoParseChunk( p_obj.m_nodeTransform, chunk );
				break;

			case ChunkType::eKeyframeCount:
				result = DoParseChunk( count, chunk );

				if ( result )
				{
					keyframes.resize( count );
				}

				break;

			case ChunkType::eKeyframes:
				result = DoParseChunk( keyframes, chunk );

				if ( result )
				{
					DoConvert( keyframes, p_obj.m_keyframes );
				}

				break;

			case ChunkType::eAnimLength:
				result = DoParseChunk( length, chunk );
				p_obj.m_length = std::chrono::milliseconds{ int64_t( length / 1000 ) };
				break;

			case ChunkType::eSkeletonAnimationBone:
				bone = std::make_shared< SkeletonAnimationBone >( *p_obj.GetOwner() );
				result = BinaryParser< SkeletonAnimationBone >{}.Parse( *bone, chunk );

				if ( result )
				{
					p_obj.AddChild( bone );
					p_obj.GetOwner()->AddObject( bone, p_obj.shared_from_this() );
				}

				break;

			case ChunkType::eSkeletonAnimationNode:
				node = std::make_shared< SkeletonAnimationNode >( *p_obj.GetOwner() );
				result = BinaryParser< SkeletonAnimationNode >{}.Parse( *node, chunk );

				if ( result )
				{
					p_obj.AddChild( node );
					p_obj.GetOwner()->AddObject( node, p_obj.shared_from_this() );
				}

				break;
			}
		}

		return result;
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
		auto it = std::find_if( m_keyframes.begin(), m_keyframes.end(), [&p_from]( KeyFrame & p_keyframe )
		{
			return p_keyframe.GetTimeIndex() >= p_from;
		} );

		if ( it == m_keyframes.end() )
		{
			m_length = p_from;
			it = m_keyframes.insert( m_keyframes.end(), KeyFrame{ p_from, p_translate, p_rotate, p_scale } );
		}
		else if ( it->GetTimeIndex() > p_from )
		{
			if ( it != m_keyframes.begin() )
			{
				--it;
			}

			it = m_keyframes.insert( it, KeyFrame{ p_from, p_translate, p_rotate, p_scale } );
		}

		return *it;
	}

	void SkeletonAnimationObject::RemoveKeyFrame( std::chrono::milliseconds const & p_time )
	{
		auto it = std::find_if( m_keyframes.begin(), m_keyframes.end(), [&p_time]( KeyFrame const & p_keyframe )
		{
			return p_keyframe.GetTimeIndex() == p_time;
		} );

		if ( it != m_keyframes.end() )
		{
			m_keyframes.erase( it );
		}
	}

	//*************************************************************************************************
}
