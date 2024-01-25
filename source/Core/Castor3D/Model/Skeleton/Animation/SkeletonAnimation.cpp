#include "Castor3D/Model/Skeleton/Animation/SkeletonAnimation.hpp"

#include "Castor3D/Miscellaneous/Logger.hpp"
#include "Castor3D/Model/Skeleton/Animation/SkeletonAnimationBone.hpp"
#include "Castor3D/Model/Skeleton/Animation/SkeletonAnimationNode.hpp"
#include "Castor3D/Model/Skeleton/BoneNode.hpp"
#include "Castor3D/Animation/Animable.hpp"

CU_ImplementSmartPtr( castor3d, SkeletonAnimation )

namespace castor3d
{
	//*************************************************************************************************

	namespace sklanm
	{
		static castor::Map< SkeletonNodeType, castor::String > const MovingTypeNames
		{
			{ SkeletonNodeType::eNode, cuT( "Node_" ) },
			{ SkeletonNodeType::eBone, cuT( "Bone_" ) },
		};

		static castor::String const & getMovingTypeName( SkeletonNodeType type )
		{
			return MovingTypeNames.at( type );
		}
	}

	//*************************************************************************************************

	SkeletonAnimation::SkeletonAnimation( Animable & animable
		, castor::String const & name )
		: Animation{ *animable.getOwner(), AnimationType::eSkeleton, animable, name }
	{
	}

	SkeletonAnimationObjectRPtr SkeletonAnimation::addObject( SkeletonNode & node
		, SkeletonAnimationObject const * parent )
	{
		auto result = castor::makeUnique< SkeletonAnimationNode >( *this );
		result->setNode( node );
		return addObject( castor::ptrRefCast< SkeletonAnimationObject >( result ), parent );
	}

	SkeletonAnimationObjectRPtr SkeletonAnimation::addObject( BoneNode & bone
		, SkeletonAnimationObject const * parent )
	{
		auto result = castor::makeUnique< SkeletonAnimationBone >( *this );
		result->setBone( bone );
		return addObject( castor::ptrRefCast< SkeletonAnimationObject >( result ), parent );
	}

	SkeletonAnimationObjectRPtr SkeletonAnimation::addObject( SkeletonAnimationObjectUPtr object
		, SkeletonAnimationObject const * parent )
	{
		castor::String name = sklanm::getMovingTypeName( object->getType() ) + object->getName();
		auto it = m_toMove.find( name );
		SkeletonAnimationObjectRPtr result{};

		if ( it == m_toMove.end() )
		{
			result = object.get();
			m_toMove.try_emplace( name, castor::move( object ) );

			if ( !parent )
			{
				m_rootObjects.push_back( result );
			}
		}
		else
		{
			log::warn << cuT( "This object was already added: [" ) << name << cuT( "]" ) << std::endl;
			result = it->second.get();
		}

		return result;
	}

	bool SkeletonAnimation::hasObject( SkeletonNodeType type
		, castor::String const & name )const
	{
		return m_toMove.find( sklanm::getMovingTypeName( type ) + name ) != m_toMove.end();
	}

	SkeletonAnimationObjectRPtr SkeletonAnimation::getObject( SkeletonNode const & node )const
	{
		return getObject( SkeletonNodeType::eNode, node.getName() );
	}

	SkeletonAnimationObjectRPtr SkeletonAnimation::getObject( BoneNode const & bone )const
	{
		return getObject( SkeletonNodeType::eBone, bone.getName() );
	}

	SkeletonAnimationObjectRPtr SkeletonAnimation::getObject( SkeletonNodeType type
		, castor::String const & name )const
	{
		SkeletonAnimationObjectRPtr result{};

		if ( auto it = m_toMove.find( sklanm::getMovingTypeName( type ) + name );
			it != m_toMove.end() )
		{
			result = it->second.get();
		}

		return result;
	}

	//*************************************************************************************************
}
