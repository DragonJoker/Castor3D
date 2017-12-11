#include "SkeletonAnimationKeyFrame.hpp"

#include "Animation/Skeleton/SkeletonAnimation.hpp"
#include "Animation/Skeleton/SkeletonAnimationBone.hpp"

using namespace castor;

namespace castor3d
{
	namespace
	{
		template< typename T, typename U >
		SquareMatrix< T, 4 > & doRotate( SquareMatrix< T, 4 > & matrix
			, QuaternionT< U > const & orientation )
		{
			SquareMatrix< T, 4 > rotate;
			auto const qxx( orientation.quat.x * orientation.quat.x );
			auto const qyy( orientation.quat.y * orientation.quat.y );
			auto const qzz( orientation.quat.z * orientation.quat.z );
			auto const qxz( orientation.quat.x * orientation.quat.z );
			auto const qxy( orientation.quat.x * orientation.quat.y );
			auto const qyz( orientation.quat.y * orientation.quat.z );
			auto const qwx( orientation.quat.w * orientation.quat.x );
			auto const qwy( orientation.quat.w * orientation.quat.y );
			auto const qwz( orientation.quat.w * orientation.quat.z );

			rotate[0][0] = T( 1 - 2 * ( qyy + qzz ) );
			rotate[0][1] = T( 2 * ( qxy - qwz ) );
			rotate[0][2] = T( 2 * ( qxz + qwy ) );
			rotate[0][3] = T( 0 );

			rotate[1][0] = T( 2 * ( qxy + qwz ) );
			rotate[1][1] = T( 1 - 2 * ( qxx + qzz ) );
			rotate[1][2] = T( 2 * ( qyz - qwx ) );
			rotate[1][3] = T( 0 );

			rotate[2][0] = T( 2 * ( qxz - qwy ) );
			rotate[2][1] = T( 2 * ( qyz + qwx ) );
			rotate[2][2] = T( 1 - 2 * ( qxx + qyy ) );
			rotate[3][3] = T( 0 );

			rotate[3][0] = T( 0 );
			rotate[3][1] = T( 0 );
			rotate[3][2] = T( 0 );
			rotate[3][3] = T( 1 );

			return matrix *= rotate;
		}

		template< typename T, typename U >
		void doConvert( SquareMatrix< T, 4 > const & in
			, SquareMatrix< U, 4 > & out )
		{
			out = in;
		}
	}

	//*************************************************************************************************

	bool BinaryWriter< SkeletonAnimationKeyFrame >::doWrite( SkeletonAnimationKeyFrame const & obj )
	{
		bool result = true;

		if ( result )
		{
			result = doWriteChunk( double( obj.getTimeIndex().count() ) / 1000.0, ChunkType::eSkeletonAnimationKeyFrameTime, m_chunk );
		}

		for ( auto & it : obj.m_transforms )
		{
			if ( result )
			{
				result = doWriteChunk( uint8_t( it.first->getType() ), ChunkType::eSkeletonAnimationKeyFrameObjectType, m_chunk );
			}

			if ( result )
			{
				result = doWriteChunk( it.first->getName(), ChunkType::eSkeletonAnimationKeyFrameObjectName, m_chunk );
			}

			if ( result )
			{
				SquareMatrix< double, 4 > matrix{ it.second };
				result = doWriteChunk( matrix, ChunkType::eSkeletonAnimationKeyFrameObjectTransform, m_chunk );
			}
		}

		return result;
	}

	//*************************************************************************************************

	bool BinaryParser< SkeletonAnimationKeyFrame >::doParse( SkeletonAnimationKeyFrame & obj )
	{
		bool result = true;
		SquareMatrix< double, 4 > matrix;
		double time{ 0.0 };
		String name;
		uint8_t type;
		BinaryChunk chunk;

		while ( result && doGetSubChunk( chunk ) )
		{
			switch ( chunk.getChunkType() )
			{
			case ChunkType::eSkeletonAnimationKeyFrameTime:
				result = doParseChunk( time, chunk );
				obj.doSetTimeIndex( Milliseconds{ int64_t( time * 1000 ) } );
				break;

			case ChunkType::eSkeletonAnimationKeyFrameObjectType:
				result = doParseChunk( type, chunk );
				break;

			case ChunkType::eSkeletonAnimationKeyFrameObjectName:
				result = doParseChunk( name, chunk );
				break;

			case ChunkType::eSkeletonAnimationKeyFrameObjectTransform:
				result = doParseChunk( matrix, chunk );

				if ( result )
				{
					obj.addAnimationObject( *obj.getOwner()->getObject( SkeletonAnimationObjectType( type )
							, name )
						, Matrix4x4r{ matrix } );
				}

				break;
			}
		}

		return result;
	}

	//*************************************************************************************************

	SkeletonAnimationKeyFrame::SkeletonAnimationKeyFrame( SkeletonAnimation & skeletonAnimation
		, Milliseconds const & timeIndex )
		: AnimationKeyFrame{ timeIndex }
		, OwnedBy< SkeletonAnimation >{ skeletonAnimation }
	{
	}

	void SkeletonAnimationKeyFrame::addAnimationObject( SkeletonAnimationObject & object
		, Point3r const & translate
		, Quaternion const & rotate
		, Point3r const & scale )
	{
		Matrix4x4r transform{ 1.0_r };
		matrix::translate( transform, translate );
		doRotate( transform, rotate );
		matrix::scale( transform, scale );
		addAnimationObject( object, transform );
	}

	void SkeletonAnimationKeyFrame::addAnimationObject( SkeletonAnimationObject & object
		, castor::Matrix4x4r const & transform )
	{
		auto findTransform = [this]( SkeletonAnimationObject & object )
		{
			return std::find_if( m_transforms.begin()
				, m_transforms.end()
				, [&object]( auto const & lookup )
				{
					return lookup.first == &object;
				} );
		};
		auto it = findTransform( object );

		if ( it == m_transforms.end() )
		{
			auto parent = object.getParent();

			if ( parent && findTransform( *parent ) == m_transforms.end() )
			{
				addAnimationObject( *parent, parent->getNodeTransform() );
			}

			m_transforms.emplace_back( &object, transform );
		}
	}

	bool SkeletonAnimationKeyFrame::hasObject( SkeletonAnimationObject const & object )const
	{
		return m_transforms.end() != std::find_if( m_transforms.begin()
			, m_transforms.end()
			, [&object]( auto const & lookup )
			{
				return lookup.first == &object;
			} );
	}

	TransformArray::const_iterator SkeletonAnimationKeyFrame::find( SkeletonAnimationObject const & object )const
	{
		return std::find_if( m_cumulative.begin()
			, m_cumulative.end()
			, [&object]( ObjectTransform const & lookup )
			{
				return lookup.first == &object;
			} );
	}

	TransformArray::const_iterator SkeletonAnimationKeyFrame::find( Bone const & bone )const
	{
		return std::find_if( m_cumulative.begin()
			, m_cumulative.end()
			, [&bone]( ObjectTransform const & lookup )
		{
			return lookup.first->getType() == SkeletonAnimationObjectType::eBone
				&& static_cast< SkeletonAnimationBone const & >( *lookup.first ).getBone().get() == &bone;
		} );
	}

	void SkeletonAnimationKeyFrame::initialise()
	{
		m_cumulative.clear();

		for ( auto & transform : m_transforms )
		{
			auto parent = transform.first->getParent();

			if ( parent )
			{
				auto it = find( *parent );
				ENSURE( it != end() );
				m_cumulative.emplace_back( transform.first, it->second * transform.second );
			}
			else
			{
				m_cumulative.push_back( transform );
			}
		}
	}

	//*************************************************************************************************
}
