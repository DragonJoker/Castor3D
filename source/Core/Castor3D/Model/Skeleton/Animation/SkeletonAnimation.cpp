#include "Castor3D/Model/Skeleton/Animation/SkeletonAnimation.hpp"

#include "Castor3D/Model/Skeleton/Animation/SkeletonAnimationBone.hpp"
#include "Castor3D/Model/Skeleton/Animation/SkeletonAnimationNode.hpp"
#include "Castor3D/Model/Skeleton/Bone.hpp"

using namespace castor;

namespace castor3d
{
	//*************************************************************************************************

	namespace
	{
		castor::String const & getMovingTypeName( SkeletonAnimationObjectType type )
		{
			static std::map< SkeletonAnimationObjectType, String > const names
			{
				{ SkeletonAnimationObjectType::eNode, cuT( "Node_" ) },
				{ SkeletonAnimationObjectType::eBone, cuT( "Bone_" ) },
			};
			return names.at( type );
		}
	}

	//*************************************************************************************************

	SkeletonAnimation::SkeletonAnimation( Animable & animable
		, String const & name )
		: Animation{ AnimationType::eSkeleton, animable, name }
	{
	}

	SkeletonAnimation::~SkeletonAnimation()
	{
	}

	SkeletonAnimationObjectSPtr SkeletonAnimation::addObject( castor::String const & name
		, SkeletonAnimationObjectSPtr parent )
	{
		return addObject( std::make_shared< SkeletonAnimationNode >( *this, name ), parent );
	}

	SkeletonAnimationObjectSPtr SkeletonAnimation::addObject( BoneSPtr bone
		, SkeletonAnimationObjectSPtr parent )
	{
		auto result = std::make_shared< SkeletonAnimationBone >( *this );
		result->setBone( bone );
		return addObject( result, parent );
	}

	SkeletonAnimationObjectSPtr SkeletonAnimation::addObject( SkeletonAnimationObjectSPtr object
		, SkeletonAnimationObjectSPtr parent )
	{
		String name = getMovingTypeName( object->getType() ) + object->getName();
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

	bool SkeletonAnimation::hasObject( SkeletonAnimationObjectType type
		, castor::String const & name )const
	{
		return m_toMove.find( getMovingTypeName( type ) + name ) != m_toMove.end();
	}

	SkeletonAnimationObjectSPtr SkeletonAnimation::getObject( Bone const & bone )const
	{
		return getObject( SkeletonAnimationObjectType::eBone, bone.getName() );
	}

	SkeletonAnimationObjectSPtr SkeletonAnimation::getObject( String const & name )const
	{
		return getObject( SkeletonAnimationObjectType::eNode, name );
	}

	SkeletonAnimationObjectSPtr SkeletonAnimation::getObject( SkeletonAnimationObjectType type
		, castor::String const & name )const
	{
		SkeletonAnimationObjectSPtr result;
		auto it = m_toMove.find( getMovingTypeName( type ) + name );

		if ( it != m_toMove.end() )
		{
			result = it->second;
		}

		return result;
	}

	//*************************************************************************************************
}
