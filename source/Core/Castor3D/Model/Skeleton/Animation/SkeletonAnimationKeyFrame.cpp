#include "Castor3D/Model/Skeleton/Animation/SkeletonAnimationKeyFrame.hpp"

#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/SkinComponent.hpp"
#include "Castor3D/Model/Skeleton/BoneNode.hpp"
#include "Castor3D/Model/Skeleton/Skeleton.hpp"
#include "Castor3D/Model/Skeleton/Animation/SkeletonAnimation.hpp"
#include "Castor3D/Model/Skeleton/Animation/SkeletonAnimationBone.hpp"

#include <CastorUtils/Math/SquareMatrix.hpp>
#include <CastorUtils/Math/Quaternion.hpp>
#include <CastorUtils/Miscellaneous/Hash.hpp>

CU_ImplementSmartPtr( castor3d, SkeletonAnimationKeyFrame )

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
		if ( auto it = find( object );
			it == m_transforms.end() )
		{
			if ( auto parent = object.getParent();
				parent && find( *parent ) == m_transforms.end() )
			{
				addAnimationObject( *parent
					, castor::Point3f{}
					, castor::Quaternion::identity()
					, castor::Point3f{ 1.0f, 1.0f, 1.0f } );
			}

			auto & ins = m_transforms.emplace_back();
			ins.object = &object;
			ins.transform.translate = translate;
			ins.transform.rotate = rotate;
			ins.transform.scale = scale;
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
			castor::matrix::setTranslate( transformMtx, transform.transform.translate );
			castor::matrix::rotate( transformMtx, transform.transform.rotate );
			castor::matrix::scale( transformMtx, transform.transform.scale );

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

	SubmeshBoundingBoxList const & SkeletonAnimationKeyFrame::computeBoundingBoxes( Mesh const & mesh
		, Skeleton const & skeleton )const
	{
		auto hash = std::hash< Mesh const * >{}( &mesh );
		hash = castor::hashCombinePtr( hash, skeleton );
		auto [rit, inserted] = m_boxes.try_emplace( hash );

		if ( inserted )
		{
			float constexpr rmax = std::numeric_limits< float >::max();
			float constexpr rmin = std::numeric_limits< float >::lowest();

			for ( auto & submesh : mesh )
			{
				castor::Point3f min{ rmax, rmax, rmax };
				castor::Point3f max{ rmin, rmin, rmin };

				if ( !submesh->hasComponent( SkinComponent::TypeName ) )
				{
					min = submesh->getBoundingBox().getMin();
					max = submesh->getBoundingBox().getMax();
				}
				else
				{
					auto component = submesh->getComponent< SkinComponent >();
					auto & positions = submesh->getPositions();
					uint32_t index = 0u;

					for ( auto & boneData : component->getData().getData() )
					{
						castor::Matrix4x4f transform{ 1.0 };

						if ( boneData.m_weights[0] > 0 )
						{
							auto bone = *( skeleton.getBones().begin() + boneData.m_ids[0] );
							auto it = find( *bone );

							if ( it != end() )
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
								auto it = find( *bone );

								if ( it != end() )
								{
									transform += castor::Matrix4x4f{ it->cumulative * bone->getInverseTransform() * boneData.m_weights[i] };
								}
								else
								{
									transform += castor::Matrix4x4f{ bone->getInverseTransform() * boneData.m_weights[0] };
								}
							}
						}

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
				rit->second.emplace_back( submesh.get()
					, castor::BoundingBox{ min, max } );
			}
		}

		return rit->second;
	}

	//*************************************************************************************************
}
