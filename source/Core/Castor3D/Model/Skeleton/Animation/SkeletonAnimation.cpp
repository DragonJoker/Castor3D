#include "Castor3D/Model/Skeleton/Animation/SkeletonAnimation.hpp"

#include "Castor3D/Miscellaneous/Logger.hpp"
#include "Castor3D/Model/Skeleton/Animation/SkeletonAnimationBone.hpp"
#include "Castor3D/Model/Skeleton/Animation/SkeletonAnimationNode.hpp"
#include "Castor3D/Model/Skeleton/BoneNode.hpp"
#include "Castor3D/Animation/Animable.hpp"

namespace castor3d
{
	//*************************************************************************************************

	namespace sklanm
	{
		static castor::String const & getMovingTypeName( SkeletonNodeType type )
		{
			static std::map< SkeletonNodeType, castor::String > const names
			{
				{ SkeletonNodeType::eNode, cuT( "Node_" ) },
				{ SkeletonNodeType::eBone, cuT( "Bone_" ) },
			};
			return names.at( type );
		}
	}

	//*************************************************************************************************

	SkeletonAnimation::SkeletonAnimation( Animable & animable
		, castor::String const & name )
		: Animation{ *animable.getOwner(), AnimationType::eSkeleton, animable, name }
	{
	}

	SkeletonAnimationObjectSPtr SkeletonAnimation::addObject( SkeletonNode & node
		, SkeletonAnimationObjectSPtr parent )
	{
		auto result = std::make_shared< SkeletonAnimationNode >( *this );
		result->setNode( node );
		return addObject( result, parent );
	}

	SkeletonAnimationObjectSPtr SkeletonAnimation::addObject( BoneNode & bone
		, SkeletonAnimationObjectSPtr parent )
	{
		auto result = std::make_shared< SkeletonAnimationBone >( *this );
		result->setBone( bone );
		return addObject( result, parent );
	}

	SkeletonAnimationObjectSPtr SkeletonAnimation::addObject( SkeletonAnimationObjectSPtr object
		, SkeletonAnimationObjectSPtr parent )
	{
		castor::String name = sklanm::getMovingTypeName( object->getType() ) + object->getName();
		auto it = m_toMove.find( name );
		SkeletonAnimationObjectSPtr result;

		if ( it == m_toMove.end() )
		{
			m_toMove.emplace( name, object );

			if ( !parent )
			{
				m_arrayMoving.push_back( object );
			}

			result = object;
		}
		else
		{
			log::warn << cuT( "This object was already added: [" ) << name << cuT( "]" ) << std::endl;
			result = it->second;
		}

		return result;
	}

	bool SkeletonAnimation::hasObject( SkeletonNodeType type
		, castor::String const & name )const
	{
		return m_toMove.find( sklanm::getMovingTypeName( type ) + name ) != m_toMove.end();
	}

	SkeletonAnimationObjectSPtr SkeletonAnimation::getObject( SkeletonNode const & node )const
	{
		return getObject( SkeletonNodeType::eNode, node.getName() );
	}

	SkeletonAnimationObjectSPtr SkeletonAnimation::getObject( BoneNode const & bone )const
	{
		return getObject( SkeletonNodeType::eBone, bone.getName() );
	}

	SkeletonAnimationObjectSPtr SkeletonAnimation::getObject( SkeletonNodeType type
		, castor::String const & name )const
	{
		SkeletonAnimationObjectSPtr result;
		auto it = m_toMove.find( sklanm::getMovingTypeName( type ) + name );

		if ( it != m_toMove.end() )
		{
			result = it->second;
		}

		return result;
	}

	//*************************************************************************************************
}
