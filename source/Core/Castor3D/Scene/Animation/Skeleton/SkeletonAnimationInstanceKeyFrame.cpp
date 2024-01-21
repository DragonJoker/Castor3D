#include "Castor3D/Scene/Animation/Skeleton/SkeletonAnimationInstanceKeyFrame.hpp"

#include "Castor3D/Miscellaneous/Logger.hpp"
#include "Castor3D/Model/Skeleton/Animation/SkeletonAnimationBone.hpp"
#include "Castor3D/Model/Skeleton/Animation/SkeletonAnimationKeyFrame.hpp"
#include "Castor3D/Model/Mesh/Mesh.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Model/Vertex.hpp"
#include "Castor3D/Model/Skeleton/BoneNode.hpp"
#include "Castor3D/Model/Skeleton/BonedVertex.hpp"
#include "Castor3D/Model/Skeleton/Skeleton.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/SkinComponent.hpp"
#include "Castor3D/Scene/Geometry.hpp"
#include "Castor3D/Scene/Animation/AnimatedSkeleton.hpp"
#include "Castor3D/Scene/Animation/Skeleton/SkeletonAnimationInstance.hpp"
#include "Castor3D/Scene/Animation/Skeleton/SkeletonAnimationInstanceObject.hpp"

namespace castor3d
{
	namespace sklanminstkf
	{
		static castor::BoundingBox doComputeBoundingBox( Skeleton const & skeleton
			, Submesh const & submesh
			, SkeletonAnimationKeyFrame const & keyFrame )
		{
			float constexpr rmax = std::numeric_limits< float >::max();
			float constexpr rmin = std::numeric_limits< float >::lowest();
			castor::Point3f min{ rmax, rmax, rmax };
			castor::Point3f max{ rmin, rmin, rmin };

			if ( !submesh.hasComponent( SkinComponent::TypeName ) )
			{
				min = submesh.getBoundingBox().getMin();
				max = submesh.getBoundingBox().getMax();
			}
			else
			{
				auto component = submesh.getComponent< SkinComponent >();
				uint32_t index = 0u;

				for ( auto & boneData : component->getData().getData() )
				{
					castor::Matrix4x4f transform{ 1.0 };

					if ( boneData.m_weights[0] > 0 )
					{
						auto bone = *( skeleton.getBones().begin() + boneData.m_ids[0] );
						auto it = keyFrame.find( *bone );

						if ( it != keyFrame.end() )
						{
							transform = castor::Matrix4x4f{ it->cumulative * bone->getInverseTransform() * boneData.m_weights[0] };
						}
						else
						{
							transform = castor::Matrix4x4f{ bone->getInverseTransform() * boneData.m_weights[0] };
						}
					}

					for ( uint32_t i = 1; i < boneData.m_ids.size(); ++i )
					{
						if ( boneData.m_weights[i] > 0 )
						{
							auto bone = *( skeleton.getBones().begin() + boneData.m_ids[i] );
							auto it = keyFrame.find( *bone );

							if ( it != keyFrame.end() )
							{
								transform += castor::Matrix4x4f{ it->cumulative * bone->getInverseTransform() * boneData.m_weights[i] };
							}
							else
							{
								transform += castor::Matrix4x4f{ bone->getInverseTransform() * boneData.m_weights[0] };
							}
						}
					}

					auto & positions = submesh.getPositions();
					auto & cposition = positions[index];
					castor::Point4f position{ cposition[0], cposition[1], cposition[2], 1.0f };
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

			CU_Ensure( !std::isnan( min[0] ) && !std::isnan( min[1] ) && !std::isnan( min[2] ) );
			CU_Ensure( !std::isnan( max[0] ) && !std::isnan( max[1] ) && !std::isnan( max[2] ) );
			CU_Ensure( !std::isinf( min[0] ) && !std::isinf( min[1] ) && !std::isinf( min[2] ) );
			CU_Ensure( !std::isinf( max[0] ) && !std::isinf( max[1] ) && !std::isinf( max[2] ) );
			CU_Ensure( min != castor::Point3f( rmax, rmax, rmax ) );
			CU_Ensure( max != castor::Point3f( rmin, rmin, rmin ) );
			return castor::BoundingBox{ min, max };
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
			m_objects.emplace_back( object.get()
				, ( it != keyFrame.end()
					? it->cumulative
					: castor::Matrix4x4f{ castor::Matrix4x4f::getIdentity() } ) );
		}

		for ( auto & submesh : skeleton.getMesh() )
		{
			m_boxes.emplace_back( submesh.get()
				, sklanminstkf::doComputeBoundingBox( skeleton.getSkeleton()
					, *submesh
					, keyFrame ) );
		}
	}

	void SkeletonAnimationInstanceKeyFrame::apply()
	{
		for ( auto const & [object, transform] : m_objects )
		{
			object->update( transform );
		}

		m_skeleton.getGeometry().updateContainers( m_boxes );
	}
}
