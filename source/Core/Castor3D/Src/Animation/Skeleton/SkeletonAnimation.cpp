#include "SkeletonAnimation.hpp"

#include "Engine.hpp"

#include "Mesh/Skeleton/Bone.hpp"
#include "SkeletonAnimationBone.hpp"
#include "SkeletonAnimationKeyFrame.hpp"
#include "SkeletonAnimationNode.hpp"

#include "Scene/Geometry.hpp"

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
		std::shared_ptr< SkeletonAnimationBone > result = std::make_shared< SkeletonAnimationBone >( *this );
		result->setBone( bone );
		auto added = addObject( result, parent );
		return result;
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
			Logger::logWarning( cuT( "This object was already added: [" ) + name + cuT( "]" ) );
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
		return getObject( SkeletonAnimationObjectType::eNode, bone.getName() );
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
