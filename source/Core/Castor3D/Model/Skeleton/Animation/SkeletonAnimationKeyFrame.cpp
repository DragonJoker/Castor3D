#include "Castor3D/Model/Skeleton/Animation/SkeletonAnimationKeyFrame.hpp"

#include "Castor3D/Model/Skeleton/Animation/SkeletonAnimation.hpp"
#include "Castor3D/Model/Skeleton/Animation/SkeletonAnimationBone.hpp"

#include <CastorUtils/Math/SquareMatrix.hpp>
#include <CastorUtils/Math/Quaternion.hpp>

namespace castor3d
{
	namespace
	{
		template< typename T, typename U >
		castor::SquareMatrix< T, 4 > & doRotate( castor::SquareMatrix< T, 4 > & matrix
			, castor::QuaternionT< U > const & orientation )
		{
			castor::SquareMatrix< T, 4 > rotate;
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
		void doConvert( castor::SquareMatrix< T, 4 > const & in
			, castor::SquareMatrix< U, 4 > & out )
		{
			out = in;
		}
	}

	//*************************************************************************************************

	SkeletonAnimationKeyFrame::SkeletonAnimationKeyFrame( SkeletonAnimation & skeletonAnimation
		, castor::Milliseconds const & timeIndex )
		: AnimationKeyFrame{ timeIndex }
		, OwnedBy< SkeletonAnimation >{ skeletonAnimation }
	{
	}

	void SkeletonAnimationKeyFrame::addAnimationObject( SkeletonAnimationObject & object
		, castor::Point3f const & translate
		, castor::Quaternion const & rotate
		, castor::Point3f const & scale )
	{
		castor::Matrix4x4f transform{ 1.0f };
		castor::matrix::translate( transform, translate );
		doRotate( transform, rotate );
		castor::matrix::scale( transform, scale );
		addAnimationObject( object, transform );
	}

	void SkeletonAnimationKeyFrame::addAnimationObject( SkeletonAnimationObject & object
		, castor::Matrix4x4f const & transform )
	{
		auto findTransform = [this]( SkeletonAnimationObject & obj )
		{
			return std::find_if( m_transforms.begin()
				, m_transforms.end()
				, [&obj]( auto const & lookup )
				{
					return lookup.first == &obj;
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
				CU_Ensure( it != end() );
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
