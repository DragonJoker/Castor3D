#include "Castor3D/Model/Skeleton/Bone.hpp"

#include "Castor3D/Model/Skeleton/Skeleton.hpp"
#include "Castor3D/Model/Mesh/Mesh.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/BonesComponent.hpp"

using namespace castor;

namespace castor3d
{
	Bone::Bone( Skeleton & skeleton
		, castor::Matrix4x4f const & offset )
		: Named{ cuEmptyString }
		, m_skeleton{ skeleton }
		, m_offset{ offset }
		, m_absoluteOffset{ skeleton.getGlobalInverseTransform() }
	{
	}

	Bone::~Bone()
	{
	}

	void Bone::addChild( BoneSPtr bone )
	{
		if ( m_children.end() == m_children.find( bone->getName() ) )
		{
			m_children.emplace( bone->getName(), bone );
		}
	}

	void Bone::setParent( BoneSPtr bone )
	{
		m_parent = bone;
		m_absoluteOffset = m_offset * m_parent->m_absoluteOffset;
	}

	BoundingBox Bone::computeBoundingBox( Mesh const & mesh
		, uint32_t boneIndex )const
	{
		auto rmax = std::numeric_limits< float >::max();
		auto rmin = std::numeric_limits< float >::lowest();
		castor::Point3f min{ rmax, rmax, rmax };
		castor::Point3f max{ rmin, rmin, rmin };

		for ( auto & submesh : mesh )
		{
			if ( submesh->hasComponent( BonesComponent::Name ) )
			{
				auto component = submesh->getComponent< BonesComponent >();
				uint32_t i = 0u;

				for ( auto & boneData : component->getBonesData() )
				{
					auto it = std::find( boneData.m_ids.data.begin()
						, boneData.m_ids.data.end()
						, boneIndex );

					if ( it != boneData.m_ids.data.end() )
					{
						auto position = submesh->getPoint( i ).pos;
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
}
