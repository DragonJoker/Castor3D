#include "SkeletonAnimationInstanceKeyFrame.hpp"

#include "Animation/Skeleton/SkeletonAnimationBone.hpp"
#include "Animation/Skeleton/SkeletonAnimationKeyFrame.hpp"
#include "Mesh/Mesh.hpp"
#include "Mesh/Submesh.hpp"
#include "Mesh/Vertex.hpp"
#include "Mesh/Skeleton/BonedVertex.hpp"
#include "Mesh/Skeleton/Skeleton.hpp"
#include "Mesh/SubmeshComponent/BonesComponent.hpp"
#include "Scene/Geometry.hpp"
#include "Scene/Animation/AnimatedSkeleton.hpp"
#include "Scene/Animation/Skeleton/SkeletonAnimationInstance.hpp"
#include "Scene/Animation/Skeleton/SkeletonAnimationInstanceObject.hpp"

using namespace castor;

namespace castor3d
{
	namespace
	{
		BoundingBox doComputeBoundingBox( Skeleton const & skeleton
			, Submesh const & submesh
			, SkeletonAnimationKeyFrame const & keyFrame )
		{
			real rmax = std::numeric_limits< real >::max();
			real rmin = std::numeric_limits< real >::lowest();
			Point3r min{ rmax, rmax, rmax };
			Point3r max{ rmin, rmin, rmin };

			if ( !submesh.hasComponent( BonesComponent::Name ) )
			{
				min = submesh.getBoundingBox().getMin();
				max = submesh.getBoundingBox().getMax();
			}
			else
			{
				auto component = submesh.getComponent< BonesComponent >();
				uint32_t index = 0u;

				for ( auto & boneData : component->getBonesData() )
				{
					Matrix4x4r transform{ 1.0_r };

					if ( boneData.m_weights.weight0[0] > 0 )
					{
						auto bone = *( skeleton.begin() + boneData.m_ids.id0[0] );
						auto it = keyFrame.find( *bone );
						REQUIRE( it != keyFrame.end() );
						transform = Matrix4x4r{ it->second * bone->getOffsetMatrix() * boneData.m_weights.weight0[0] };
					}

					for ( uint32_t i = 1; i < boneData.m_ids.id0.size(); ++i )
					{
						if ( boneData.m_weights.weight0[i] > 0 )
						{
							auto bone = *( skeleton.begin() + boneData.m_ids.id0[i] );
							auto it = keyFrame.find( *bone );
							REQUIRE( it != keyFrame.end() );
							transform += Matrix4x4r{ it->second * bone->getOffsetMatrix() * boneData.m_weights.weight0[i] };
						}
					}

					for ( uint32_t i = 1; i < boneData.m_ids.id1.size(); ++i )
					{
						if ( boneData.m_weights.weight1[i] > 0 )
						{
							auto bone = *( skeleton.begin() + boneData.m_ids.id1[i] );
							auto it = keyFrame.find( *bone );
							REQUIRE( it != keyFrame.end() );
							transform += Matrix4x4r{ it->second * bone->getOffsetMatrix() * boneData.m_weights.weight1[i] };
						}
					}

					auto & cposition = submesh.getPoint( index ).m_pos;
					Point4r position{ cposition[0], cposition[1], cposition[2], 1.0_r };
					position = transform * position;
					min[0] = std::min( min[0], position[0] );
					min[1] = std::min( min[1], position[1] );
					min[2] = std::min( min[2], position[2] );
					max[0] = std::max( max[0], position[0] );
					max[1] = std::max( max[1], position[1] );
					max[2] = std::max( max[2], position[2] );

					++index;
				}
			}

			ENSURE( !isNan( min[0] ) && !isNan( min[1] ) && !isNan( min[2] ) );
			ENSURE( !isNan( max[0] ) && !isNan( max[1] ) && !isNan( max[2] ) );
			ENSURE( !isInf( min[0] ) && !isInf( min[1] ) && !isInf( min[2] ) );
			ENSURE( !isInf( max[0] ) && !isInf( max[1] ) && !isInf( max[2] ) );
			ENSURE( min != Point3r( rmax, rmax, rmax ) );
			ENSURE( max != Point3r( rmin, rmin, rmin ) );
			return BoundingBox{ min, max };
		}
	}

	SkeletonAnimationInstanceKeyFrame::SkeletonAnimationInstanceKeyFrame( SkeletonAnimationInstance & skeletonAnimation
		, SkeletonAnimationKeyFrame const & keyFrame
		, AnimatedSkeleton & skeleton )
		: OwnedBy< SkeletonAnimationInstance >{ skeletonAnimation }
		, m_skeleton{ skeleton }
		, m_keyFrame{ keyFrame }
	{
		for ( auto & object : skeletonAnimation )
		{
			auto it = keyFrame.find( object->getObject() );
			REQUIRE( it != keyFrame.end() );
			m_objects.emplace_back( object.get(), it->second );

			if ( object->getObject().getType() == SkeletonAnimationObjectType::eBone )
			{
				auto bone = static_cast< SkeletonAnimationBone const & >( object->getObject() ).getBone();
				auto itb = std::find( skeleton.getSkeleton().begin()
					, skeleton.getSkeleton().end()
					, bone );
			}
		}

		for ( auto & submesh : skeleton.getMesh() )
		{
			m_boxes.emplace_back( submesh.get()
				, doComputeBoundingBox( skeleton.getSkeleton()
					, *submesh
					, keyFrame ) );
		}
	}

	void SkeletonAnimationInstanceKeyFrame::apply()
	{
		for ( auto object : m_objects )
		{
			object.first->update( object.second );
		}

		m_skeleton.getGeometry().updateContainers( m_boxes );
	}
}
