#include "AnimatedObject.hpp"
#include "AnimatedObjectGroup.hpp"
#include "Animation.hpp"
#include "Geometry.hpp"
#include "Scene.hpp"
#include "Mesh.hpp"
#include "Skeleton.hpp"

using namespace Castor;

namespace Castor3D
{
	AnimatedObject::AnimatedObject( Castor::String const & p_name )
		: Named( p_name )
	{
	}

	AnimatedObject::~AnimatedObject()
	{
	}

	void AnimatedObject::Update( real p_tslf )
	{
		for ( auto & l_it : m_animations )
		{
			if ( l_it.second->GetState() != Animation::eSTATE_STOPPED )
			{
				l_it.second->Update( p_tslf );
			}
		}
	}

	void AnimatedObject::StartAnimation( String const & p_name )
	{
		auto l_it = m_animations.find( p_name );

		if ( l_it != m_animations.end() )
		{
			l_it->second->Play();
		}
	}

	void AnimatedObject::StopAnimation( String const & p_name )
	{
		auto l_it = m_animations.find( p_name );

		if ( l_it != m_animations.end() )
		{
			l_it->second->Stop();
		}
	}

	void AnimatedObject::PauseAnimation( String const & p_name )
	{
		auto l_it = m_animations.find( p_name );

		if ( l_it != m_animations.end() )
		{
			l_it->second->Pause();
		}
	}

	void AnimatedObject::StartAllAnimations()
	{
		for ( auto & l_it : m_animations )
		{
			l_it.second->Play();
		}
	}

	void AnimatedObject::StopAllAnimations()
	{
		for ( auto & l_it : m_animations )
		{
			l_it.second->Stop();
		}
	}

	void AnimatedObject::PauseAllAnimations()
	{
		for ( auto & l_it : m_animations )
		{
			l_it.second->Pause();
		}
	}

	AnimationSPtr AnimatedObject::GetAnimation( Castor::String const & p_name )
	{
		AnimationSPtr l_return;
		auto l_it = m_animations.find( p_name );

		if ( l_it != m_animations.end() )
		{
			l_return = l_it->second;
		}

		return l_return;
	}

	void AnimatedObject::SetGeometry( GeometrySPtr p_object )
	{
		m_animations.clear();
		DoSetGeometry( p_object );
	}

	void AnimatedObject::SetMesh( MeshSPtr p_mesh )
	{
		m_animations.clear();
		DoSetMesh( p_mesh );
	}

	void AnimatedObject::SetSkeleton( SkeletonSPtr p_skeleton )
	{
		m_animations.clear();
		DoSetSkeleton( p_skeleton );
	}

	void AnimatedObject::DoSetGeometry( GeometrySPtr p_object )
	{
		if ( p_object )
		{
			DoCopyAnimations( p_object );
			DoSetMesh( p_object->GetMesh() );
		}

		m_geometry = p_object;
	}

	void AnimatedObject::DoSetMesh( MeshSPtr p_mesh )
	{
		if ( p_mesh )
		{
			DoCopyAnimations( p_mesh );
			DoSetSkeleton( p_mesh->GetSkeleton() );
		}

		m_mesh = p_mesh;
	}

	void AnimatedObject::DoSetSkeleton( SkeletonSPtr p_skeleton )
	{
		if ( p_skeleton )
		{
			DoCopyAnimations( p_skeleton );
		}

		m_skeleton = p_skeleton;
	}

	void AnimatedObject::DoCopyAnimations( AnimableSPtr p_object )
	{
		for ( auto l_it : p_object->GetAnimations() )
		{
			AnimationSPtr l_animation = l_it.second;
			MovingObjectPtrStrMap l_toMove;
			auto l_it = m_animations.insert( std::make_pair( l_animation->GetName(), std::make_shared< Animation >( l_animation->GetName() ) ) ).first;
			auto l_clone = l_it->second;

			for ( auto l_it : *l_animation )
			{
				if ( l_toMove.find( l_it.first ) == l_toMove.end() )
				{
					l_it.second->Clone( l_toMove );
				}
			}

			for ( auto l_it : l_toMove )
			{
				l_clone->AddMovingObject( l_it.second );
			}
		}
	}
}
