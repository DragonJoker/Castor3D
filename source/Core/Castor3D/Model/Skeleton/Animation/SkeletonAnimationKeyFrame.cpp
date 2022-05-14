#include "Castor3D/Model/Skeleton/Animation/SkeletonAnimationKeyFrame.hpp"

#include "Castor3D/Model/Skeleton/Animation/SkeletonAnimation.hpp"
#include "Castor3D/Model/Skeleton/Animation/SkeletonAnimationBone.hpp"

#include <CastorUtils/Math/SquareMatrix.hpp>
#include <CastorUtils/Math/Quaternion.hpp>

namespace castor3d
{
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
		auto it = find( object );

		if ( it == m_transforms.end() )
		{
			auto parent = object.getParent();

			if ( parent && find( *parent ) == m_transforms.end() )
			{
				addAnimationObject( *parent
					, castor::Point3f{}
					, castor::Quaternion::identity()
					, castor::Point3f{ 1.0f, 1.0f, 1.0f } );
			}

			auto & ins = m_transforms.emplace_back();
			ins.object = &object;
			ins.translate = translate;
			ins.rotate = rotate;
			ins.scale = scale;
		}
	}

	bool SkeletonAnimationKeyFrame::hasObject( SkeletonAnimationObject const & object )const
	{
		return m_transforms.end() != std::find_if( m_transforms.begin()
			, m_transforms.end()
			, [&object]( ObjectTransform const & lookup )
			{
				return lookup.object == &object;
			} );
	}

	TransformArray::const_iterator SkeletonAnimationKeyFrame::find( SkeletonAnimationObject const & object )const
	{
		return std::find_if( m_transforms.begin()
			, m_transforms.end()
			, [&object]( ObjectTransform const & lookup )
			{
				return lookup.object == &object;
			} );
	}

	TransformArray::const_iterator SkeletonAnimationKeyFrame::find( BoneNode const & bone )const
	{
		return std::find_if( m_transforms.begin()
			, m_transforms.end()
			, [&bone]( ObjectTransform const & lookup )
		{
			return lookup.object->getType() == SkeletonNodeType::eBone
				&& static_cast< SkeletonAnimationBone const & >( *lookup.object ).getBone() == &bone;
		} );
	}

	TransformArray::iterator SkeletonAnimationKeyFrame::find( SkeletonAnimationObject const & object )
	{
		return std::find_if( m_transforms.begin()
			, m_transforms.end()
			, [&object]( ObjectTransform const & lookup )
			{
				return lookup.object == &object;
			} );
	}

	TransformArray::iterator SkeletonAnimationKeyFrame::find( BoneNode const & bone )
	{
		return std::find_if( m_transforms.begin()
			, m_transforms.end()
			, [&bone]( ObjectTransform const & lookup )
		{
			return lookup.object->getType() == SkeletonNodeType::eBone
				&& static_cast< SkeletonAnimationBone const & >( *lookup.object ).getBone() == &bone;
		} );
	}

	void SkeletonAnimationKeyFrame::initialise()
	{
		for ( auto & transform : m_transforms )
		{
			transform.cumulative = castor::Matrix4x4f{ 1.0f };
		}

		for ( auto & transform : m_transforms )
		{
			auto parent = transform.object->getParent();
			castor::Matrix4x4f transformMtx{ 1.0f };
			castor::matrix::setTranslate( transformMtx, transform.translate );
			castor::matrix::rotate( transformMtx, transform.rotate );
			castor::matrix::scale( transformMtx, transform.scale );

			if ( parent )
			{
				auto it = find( *parent );
				CU_Ensure( it != end() );
				transform.cumulative = it->cumulative * transformMtx;
			}
			else
			{
				transform.cumulative = transformMtx;
			}
		}
	}

	//*************************************************************************************************
}
