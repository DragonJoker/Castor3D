#include "Castor3D/Model/Skeleton/Skeleton.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Model/Skeleton/Bone.hpp"
#include "Castor3D/Model/Skeleton/Animation/SkeletonAnimation.hpp"

namespace castor3d
{
	Skeleton::Skeleton( Scene & scene )
		: Animable{ scene }
		, m_globalInverse{ 1 }
	{
	}

	Skeleton::~Skeleton()
	{
		Animable::cleanupAnimations();
	}

	BoneSPtr Skeleton::createBone( castor::String const & name
		, castor::Matrix4x4f const & offset )
	{
		auto bone = std::make_shared< Bone >( *this, offset );
		bone->setName( name );
		m_bones.push_back( bone );
		return bone;
	}

	BoneSPtr Skeleton::findBone( castor::String const & name )const
	{
		auto it = std::find_if( begin(), end(), [&name]( BoneSPtr bone )
		{
			return bone->getName() == name;
		} );

		BoneSPtr bone;

		if ( it != end() )
		{
			bone = *it;
		}

		return bone;
	}

	void Skeleton::setBoneParent( BoneSPtr bone, BoneSPtr parent )
	{
		if ( std::find( begin(), end(), bone ) == end() )
		{
			CU_Exception( "Skeleton::setBoneParent - Child bone is not in the Skeleton's nodes" );
		}

		if ( std::find( begin(), end(), parent ) == end() )
		{
			CU_Exception( "Skeleton::setBoneParent - Parent bone is not in the Skeleton's nodes" );
		}

		parent->addChild( bone );
		bone->setParent( parent );
	}

	SkeletonAnimation & Skeleton::createAnimation( castor::String const & name )
	{
		if ( !hasAnimation( name ) )
		{
			doAddAnimation( std::make_unique< SkeletonAnimation >( *this, name ) );
		}

		return doGetAnimation< SkeletonAnimation >( name );
	}

	void Skeleton::computeContainers( Mesh & mesh )
	{
		auto it = m_boxes.find( &mesh );

		if ( it == m_boxes.end() )
		{
			uint32_t index = 0u;
			std::vector< castor::BoundingBox > boxes;
			boxes.reserve( m_bones.size() );

			for ( auto & bone : m_bones )
			{
				boxes.emplace_back( bone->computeBoundingBox( mesh, index ) );
				++index;
			}
		}
	}
}
