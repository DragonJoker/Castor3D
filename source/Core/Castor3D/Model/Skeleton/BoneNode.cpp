#include "Castor3D/Model/Skeleton/BoneNode.hpp"

#include "Castor3D/Model/Skeleton/Skeleton.hpp"
#include "Castor3D/Model/Mesh/Mesh.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/SkinComponent.hpp"

CU_ImplementSmartPtr( c3d, BoneNode )

namespace c3d
{
	BoneNode::BoneNode( String name
		, Skeleton & skeleton
		, Matrix4x4f inverseTransform
		, uint32_t id )
		: SkeletonNode{ SkeletonNodeType::eBone, c3d::move( name ), skeleton }
		, m_id{ id }
		, m_inverseTransform{ c3d::move( inverseTransform ) }
	{
	}

	BoundingBox BoneNode::computeBoundingBox( Mesh const & mesh
		, uint32_t boneIndex )const
	{
		auto constexpr rmax = std::numeric_limits< float >::max();
		auto constexpr rmin = std::numeric_limits< float >::lowest();
		Point3f min{ rmax, rmax, rmax };
		Point3f max{ rmin, rmin, rmin };

		for ( auto & submesh : mesh )
		{
			if ( auto component = submesh->hasComponent( SkinComponent::TypeName )
				? submesh->getComponent< SkinComponent >()
				: nullptr )
			{
				uint32_t i = 0u;

				for ( auto & boneData : component->getData().getData() )
				{
					auto it = std::find( boneData.m_ids.data.begin()
						, boneData.m_ids.data.end()
						, boneIndex );

					if ( it != boneData.m_ids.data.end() )
					{
						auto position = submesh->getPositions()[i];
						min[0] = std::min( min[0], position[0] );
						min[1] = std::min( min[1], position[1] );
						min[2] = std::min( min[2], position[2] );
						max[0] = std::max( max[0], position[0] );
						max[1] = std::max( max[1], position[1] );
						max[2] = std::max( max[2], position[2] );
					}
				}
			}
		}

		return BoundingBox{ min, max };
	}

	SkeletonNodeUPtr BoneNode::clone( Skeleton & parent )const
	{
		auto result = makeUnique< BoneNode >( getName(), parent, getInverseTransform(), getId() );
		doCloneInto( *result );
		return ptrRefCast< SkeletonNode >( result );
	}
}
