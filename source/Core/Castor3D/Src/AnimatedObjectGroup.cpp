#include "AnimatedObjectGroup.hpp"

#include "AnimatedObject.hpp"
#include "Animation.hpp"
#include "MovableObject.hpp"

#include <Logger.hpp>

using namespace Castor;

#ifdef GetObject
#	undef GetObject
#endif

namespace Castor3D
{
	AnimatedObjectGroup::BinaryLoader::BinaryLoader()
		: Loader< AnimatedObjectGroup, eFILE_TYPE_BINARY, BinaryFile >( File::eOPEN_MODE_DUMMY )
	{
	}

	bool AnimatedObjectGroup::BinaryLoader::operator()( AnimatedObjectGroup & p_group, BinaryFile & p_file, Scene * p_scene )
	{
		m_scene = p_scene;
		return operator()( p_group, p_file );
	}

	bool AnimatedObjectGroup::BinaryLoader::operator()( AnimatedObjectGroup & p_group, BinaryFile & p_file )
	{
		Collection< AnimatedObjectGroup, String > l_collection;
		bool l_result = false;

		if ( !l_result || l_collection.has( p_group.GetName() ) )
		{
			Logger::LogWarning( cuT( "Can't add AnimatedObjectGroup [" ) + p_group.GetName() + cuT( "]" ) );
		}
		else
		{
			Logger::LogInfo( cuT( "AnimatedObjectGroup [" ) + p_group.GetName() +  + cuT( "] added" ) );
		}

		return l_result;
	}

	//*************************************************************************************************

	AnimatedObjectGroup::TextLoader::TextLoader( File::eENCODING_MODE p_encodingMode )
		: Loader< AnimatedObjectGroup, eFILE_TYPE_TEXT, TextFile >( File::eOPEN_MODE_DUMMY, p_encodingMode )
	{
	}

	bool AnimatedObjectGroup::TextLoader::operator()( AnimatedObjectGroup const & p_group, TextFile & p_file )
	{
		bool l_return = p_file.WriteText( cuT( "animated_object_group " ) + p_group.GetName() + cuT( "\n{\n" ) ) > 0;

		for ( auto l_it : p_group.GetAnimations() )
		{
			l_return &= p_file.WriteText( cuT( "\tanimation " ) + l_it.second->GetName() + cuT( "\n" ) ) > 0;
		}

		for ( auto l_it : p_group.GetObjects() )
		{
			l_return &= p_file.WriteText( cuT( "\tanimated_object " ) + l_it.second->GetMovableObject()->GetName() + cuT( "\n" ) ) > 0;
		}

		if ( l_return )
		{
			l_return = p_file.WriteText( cuT( "}\n\n" ) ) > 0;
		}

		return l_return;
	}

	//*************************************************************************************************

	AnimatedObjectGroup::AnimatedObjectGroup()
		: Named( cuT( "" ) )
	{
		m_timer.TimeS();
	}

	AnimatedObjectGroup::AnimatedObjectGroup( SceneSPtr p_scene, String const & p_name )
		: Named( p_name )
		, m_scene( p_scene )
	{
		m_timer.TimeS();
	}

	AnimatedObjectGroup::~AnimatedObjectGroup()
	{
		m_objects.clear();
		m_playingAnimations.clear();
		m_animations.clear();
	}

	AnimatedObjectSPtr AnimatedObjectGroup::AddObject( MovableObjectSPtr p_object )
	{
		AnimatedObjectSPtr l_return;

		if ( p_object && m_objects.find( p_object->GetName() ) == m_objects.end() )
		{
			l_return = std::make_shared< AnimatedObject >( p_object, &m_animations );
			m_objects.insert( std::make_pair( p_object->GetName(), l_return ) );
		}

		return l_return;
	}

	bool AnimatedObjectGroup::AddObject( AnimatedObjectSPtr p_object )
	{
		bool l_return = p_object && p_object->GetMovableObject() && m_objects.find( p_object->GetMovableObject()->GetName() ) == m_objects.end();

		if ( l_return )
		{
			m_objects.insert( std::make_pair( p_object->GetMovableObject()->GetName(), p_object ) );
		}

		return l_return;
	}

	void AnimatedObjectGroup::AddAnimation( AnimationSPtr p_animation )
	{
		if ( p_animation && m_animations.find( p_animation->GetName() ) == m_animations.end() )
		{
			m_animations.insert( std::make_pair( p_animation->GetName(), p_animation ) );
		}
	}

	void AnimatedObjectGroup::Update()
	{
		real l_tslf = real( m_timer.TimeS() );

		for ( auto l_it : m_animations )
		{
			l_it.second->Update( l_tslf );
		}
	}

	void AnimatedObjectGroup::StartAnimation( String const & p_name )
	{
		auto l_it = m_animations.find( p_name );

		if ( l_it != m_animations.end() )
		{
			l_it->second->Play();
		}
	}

	void AnimatedObjectGroup::StopAnimation( String const & p_name )
	{
		auto l_it = m_animations.find( p_name );

		if ( l_it != m_animations.end() )
		{
			l_it->second->Stop();
		}
	}

	void AnimatedObjectGroup::PauseAnimation( String const & p_name )
	{
		auto l_it = m_animations.find( p_name );

		if ( l_it != m_animations.end() )
		{
			l_it->second->Pause();
		}
	}

	void AnimatedObjectGroup::StartAllAnimations()
	{
		for ( auto l_it : m_animations )
		{
			l_it.second->Play();
		}
	}

	void AnimatedObjectGroup::StopAllAnimations()
	{
		for ( auto l_it : m_animations )
		{
			l_it.second->Stop();
		}
	}

	void AnimatedObjectGroup::PauseAllAnimations()
	{
		for ( auto l_it : m_animations )
		{
			l_it.second->Pause();
		}
	}
}
