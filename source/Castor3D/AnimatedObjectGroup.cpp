#include "Castor3D/PrecompiledHeader.hpp"

#include "Castor3D/AnimatedObjectGroup.hpp"
#include "Castor3D/AnimatedObject.hpp"
#include "Castor3D/Animation.hpp"
#include "Castor3D/MovableObject.hpp"
#include "Castor3D/Mesh.hpp"
#include "Castor3D/Skeleton.hpp"

using namespace Castor3D;
using namespace Castor;

#ifdef GetObject
#	undef GetObject
#endif

//*************************************************************************************************

AnimatedObjectGroup::BinaryLoader :: BinaryLoader()
	:	Loader< AnimatedObjectGroup, eFILE_TYPE_BINARY, BinaryFile >( File::eOPEN_MODE_DUMMY )
{
}

bool AnimatedObjectGroup::BinaryLoader :: operator ()( AnimatedObjectGroup & p_group, BinaryFile & p_file, Scene * p_pScene)
{
	m_pScene = p_pScene;
	return operator ()( p_group, p_file );
}

bool AnimatedObjectGroup::BinaryLoader :: operator ()( AnimatedObjectGroup & p_group, BinaryFile & p_file)
{
	Collection<AnimatedObjectGroup, String> l_collection;
	bool l_bResult = false;

	if( ! l_bResult || l_collection.has( p_group.GetName() ) )
	{
		Logger::LogMessage( cuT( "Can't add AnimatedObjectGroup [" ) + p_group.GetName() + cuT( "]" ) );
	}
	else
	{
		Logger::LogMessage( cuT( "AnimatedObjectGroup [" ) + p_group.GetName() +  + cuT( "] added" ) );
	}

	return l_bResult;
}

//*************************************************************************************************

AnimatedObjectGroup::TextLoader :: TextLoader( File::eENCODING_MODE p_eEncodingMode )
	:	Loader< AnimatedObjectGroup, eFILE_TYPE_TEXT, TextFile >( File::eOPEN_MODE_DUMMY, p_eEncodingMode )
{
}

bool AnimatedObjectGroup::TextLoader :: operator ()( AnimatedObjectGroup const & p_group, TextFile & p_file)
{
	bool l_bReturn = p_file.WriteText( cuT( "animated_object_group " ) + p_group.GetName() + cuT( "\n{\n" ) ) > 0;

	for( StrSet::const_iterator l_it = p_group.AnimationsBegin(); l_it != p_group.AnimationsEnd() && l_bReturn; ++l_it )
	{
		l_bReturn = p_file.WriteText( cuT( "\tanimation " ) + *l_it + cuT( "\n" ) ) > 0;
	}

	for( AnimatedObjectPtrStrMap::const_iterator l_it = p_group.ObjectsBegin(); l_it != p_group.ObjectsEnd() && l_bReturn; ++l_it )
	{
		l_bReturn = p_file.WriteText( cuT( "\tanimated_object " ) + l_it->first + cuT( "\n" ) ) > 0;
	}

	if( l_bReturn )
	{
		l_bReturn = p_file.WriteText( cuT( "}\n\n" ) ) > 0;
	}

	return l_bReturn;
}

//*************************************************************************************************

AnimatedObjectGroup :: AnimatedObjectGroup()
	:	Named		( cuT( "" )	)
	,	m_pScene	( NULL		)
{
	m_timer.TimeS();
}

AnimatedObjectGroup :: AnimatedObjectGroup( Scene * p_pScene, String const & p_strName )
	:	Named		( p_strName	)
	,	m_pScene	( p_pScene	)
{
	m_timer.TimeS();
}

AnimatedObjectGroup :: ~AnimatedObjectGroup()
{
	m_mapObjects.clear();
	m_setAnimations.clear();
}

AnimatedObjectSPtr AnimatedObjectGroup :: CreateObject( String const & p_strName )
{
	AnimatedObjectSPtr l_pReturn;

	if( m_mapObjects.find( p_strName ) == m_mapObjects.end() )
	{
		l_pReturn = std::make_shared< AnimatedObject >( p_strName );
		m_mapObjects.insert( std::make_pair( p_strName, l_pReturn ) );
	}

	return l_pReturn;
}

bool AnimatedObjectGroup :: AddObject( AnimatedObjectSPtr p_pObject )
{
	bool l_bReturn = p_pObject && m_mapObjects.find( p_pObject->GetName() ) == m_mapObjects.end();

	if( l_bReturn )
	{
		m_mapObjects.insert( std::make_pair( p_pObject->GetName(), p_pObject ) );
	}

	return l_bReturn;
}

void AnimatedObjectGroup :: AddAnimation( String const & p_strName )
{
	if( m_setAnimations.find( p_strName ) == m_setAnimations.end() )
	{
		m_setAnimations.insert( p_strName );
	}
}

void AnimatedObjectGroup :: Update()
{
	real l_rTslf = real( m_timer.TimeS() );

	std::for_each( m_mapObjects.begin(), m_mapObjects.end(), [&]( std::pair< String, AnimatedObjectSPtr > p_pair )
	{
		p_pair.second->Update( l_rTslf );
	} );
}

void AnimatedObjectGroup :: SetAnimationLooped( Castor::String const & p_strName, bool p_bLooped )
{
	if( m_setAnimations.find( p_strName ) != m_setAnimations.end() )
	{
		std::for_each( m_mapObjects.begin(), m_mapObjects.end(), [&]( std::pair< String, AnimatedObjectSPtr > p_pair )
		{
			AnimationSPtr l_pAnim = p_pair.second->GetAnimation( p_strName );
			
			if( l_pAnim )
			{
				l_pAnim->SetLooped( p_bLooped );
			}
		} );
	}
}

void AnimatedObjectGroup :: StartAnimation( String const & p_strName )
{
	StrSet::iterator l_it = m_setAnimations.find( p_strName );

	if( l_it != m_setAnimations.end() )
	{
		std::for_each( m_mapObjects.begin(), m_mapObjects.end(), [&]( std::pair< String, AnimatedObjectSPtr > p_pair )
		{
			p_pair.second->StartAnimation( p_strName );
		} );
	}
}

void AnimatedObjectGroup :: StopAnimation( String const & p_strName )
{
	StrSet::iterator l_it = m_setAnimations.find( p_strName );

	if( l_it != m_setAnimations.end() )
	{
		std::for_each( m_mapObjects.begin(), m_mapObjects.end(), [&]( std::pair< String, AnimatedObjectSPtr > p_pair )
		{
			p_pair.second->StopAnimation( p_strName );
		} );
	}
}

void AnimatedObjectGroup :: PauseAnimation(  String const & p_strName )
{
	StrSet::iterator l_it = m_setAnimations.find( p_strName );

	if( l_it != m_setAnimations.end() )
	{
		std::for_each( m_mapObjects.begin(), m_mapObjects.end(), [&]( std::pair< String, AnimatedObjectSPtr > p_pair )
		{
			p_pair.second->PauseAnimation( p_strName );
		} );
	}
}

void AnimatedObjectGroup :: StartAllAnimations()
{
	std::for_each( m_mapObjects.begin(), m_mapObjects.end(), [&]( std::pair< String, AnimatedObjectSPtr > p_pair )
	{
		p_pair.second->StartAllAnimations();
	} );
}

void AnimatedObjectGroup :: StopAllAnimations()
{
	std::for_each( m_mapObjects.begin(), m_mapObjects.end(), [&]( std::pair< String, AnimatedObjectSPtr > p_pair )
	{
		p_pair.second->StopAllAnimations();
	} );
}

void AnimatedObjectGroup :: PauseAllAnimations()
{
	std::for_each( m_mapObjects.begin(), m_mapObjects.end(), [&]( std::pair< String, AnimatedObjectSPtr > p_pair )
	{
		p_pair.second->PauseAllAnimations();
	} );
}

//*************************************************************************************************
