#include "SkeletonAnimationObject.hpp"

#include "SkeletonAnimation.hpp"

#include "SkeletonAnimationBone.hpp"
#include "SkeletonAnimationNode.hpp"

using namespace castor;

namespace castor3d
{
	namespace
	{
		using KeyFramed = std::array< double, 17 >;

		void doConvert( std::vector< KeyFrame > const & in
			, std::vector< KeyFramed > & out )
		{
			out.resize( in.size() );
			auto it = out.begin();

			for ( auto & kf : in )
			{
				auto & out = *it;
				size_t index{ 0u };
				auto const & transform = kf.getTransform();
				out[index++] = double( kf.getTimeIndex().count() / 1000.0 );
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

		void doConvert( std::vector< KeyFramed > const & in
			, std::vector< KeyFrame > & out )
		{
			out.resize( in.size() );
			auto it = out.begin();

			for ( auto & kf : in )
			{
				size_t index{ 0u };
				Milliseconds timeIndex{ int64_t( kf[index++] * 1000.0 ) };
				Matrix4x4r transform{ &kf[index] };
				( *it ) = KeyFrame{ timeIndex, transform };
				++it;
			}
		}
	}

	//*************************************************************************************************

	bool BinaryWriter< SkeletonAnimationObject >::doWrite( SkeletonAnimationObject const & obj )
	{
		bool result = true;

		if ( result )
		{
			result = doWriteChunk( obj.m_nodeTransform, ChunkType::eMovingTransform, m_chunk );
		}

		if ( result )
		{
			result = doWriteChunk( real( obj.m_length.count() ) * 1000.0_r, ChunkType::eAnimLength, m_chunk );
		}

		if ( !obj.m_keyframes.empty() )
		{
			if ( result )
			{
				result = doWriteChunk( uint32_t( obj.m_keyframes.size() ), ChunkType::eKeyframeCount, m_chunk );
			}

			if ( result )
			{
				std::vector< KeyFramed > keyFrames;
				doConvert( obj.m_keyframes, keyFrames );
				result = doWriteChunk( keyFrames, ChunkType::eKeyframes, m_chunk );
			}
		}

		for ( auto const & moving : obj.m_children )
		{
			switch ( moving->getType() )
			{
			case SkeletonAnimationObjectType::eNode:
				result &= BinaryWriter< SkeletonAnimationNode >{}.write( *std::static_pointer_cast< SkeletonAnimationNode >( moving ), m_chunk );
				break;

			case SkeletonAnimationObjectType::eBone:
				result &= BinaryWriter< SkeletonAnimationBone >{}.write( *std::static_pointer_cast< SkeletonAnimationBone >( moving ), m_chunk );
				break;
			}
		}

		return result;
	}

	//*************************************************************************************************

	bool BinaryParser< SkeletonAnimationObject >::doParse( SkeletonAnimationObject & obj )
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

		while ( result && doGetSubChunk( chunk ) )
		{
			switch ( chunk.getChunkType() )
			{
			case ChunkType::eMovingTransform:
				result = doParseChunk( obj.m_nodeTransform, chunk );
				break;

			case ChunkType::eKeyframeCount:
				result = doParseChunk( count, chunk );

				if ( result )
				{
					keyframes.resize( count );
				}

				break;

			case ChunkType::eKeyframes:
				result = doParseChunk( keyframes, chunk );

				if ( result )
				{
					doConvert( keyframes, obj.m_keyframes );
				}

				break;

			case ChunkType::eAnimLength:
				result = doParseChunk( length, chunk );
				obj.m_length = Milliseconds{ int64_t( length / 1000 ) };
				break;

			case ChunkType::eSkeletonAnimationBone:
				bone = std::make_shared< SkeletonAnimationBone >( *obj.getOwner() );
				result = BinaryParser< SkeletonAnimationBone >{}.parse( *bone, chunk );

				if ( result )
				{
					obj.addChild( bone );
					obj.getOwner()->addObject( bone, obj.shared_from_this() );
				}

				break;

			case ChunkType::eSkeletonAnimationNode:
				node = std::make_shared< SkeletonAnimationNode >( *obj.getOwner() );
				result = BinaryParser< SkeletonAnimationNode >{}.parse( *node, chunk );

				if ( result )
				{
					obj.addChild( node );
					obj.getOwner()->addObject( node, obj.shared_from_this() );
				}

				break;
			}
		}

		return result;
	}

	//*************************************************************************************************

	SkeletonAnimationObject::SkeletonAnimationObject( SkeletonAnimation & animation
		, SkeletonAnimationObjectType type )
		: OwnedBy< SkeletonAnimation >{ animation }
		, m_type{ type }
	{
	}

	SkeletonAnimationObject::~SkeletonAnimationObject()
	{
	}

	void SkeletonAnimationObject::addChild( SkeletonAnimationObjectSPtr object )
	{
		REQUIRE( object.get() != this );
		object->m_parent = shared_from_this();
		m_children.push_back( object );
	}

	KeyFrame & SkeletonAnimationObject::addKeyFrame( Milliseconds const & from
		, Point3r const & translate
		, Quaternion const & rotate
		, Point3r const & scale )
	{
		auto it = std::find_if( m_keyframes.begin()
			, m_keyframes.end()
			, [&from]( KeyFrame & keyframe )
			{
				return keyframe.getTimeIndex() >= from;
			} );

		if ( it == m_keyframes.end() )
		{
			m_length = from;
			it = m_keyframes.insert( m_keyframes.end(), KeyFrame{ from, translate, rotate, scale } );
		}
		else if ( it->getTimeIndex() > from )
		{
			if ( it != m_keyframes.begin() )
			{
				--it;
			}

			it = m_keyframes.insert( it, KeyFrame{ from, translate, rotate, scale } );
		}

		return *it;
	}

	void SkeletonAnimationObject::removeKeyFrame( Milliseconds const & time )
	{
		auto it = std::find_if( m_keyframes.begin()
			, m_keyframes.end()
			, [&time]( KeyFrame const & keyframe )
		{
			return keyframe.getTimeIndex() == time;
		} );

		if ( it != m_keyframes.end() )
		{
			m_keyframes.erase( it );
		}
	}

	//*************************************************************************************************
}
