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
	AnimatedObject::AnimatedObject( Castor::String const & p_strName )
		:	Named( p_strName )
	{
	}

	AnimatedObject::~AnimatedObject()
	{
	}

	void AnimatedObject::Update( real p_rTslf )
	{
		std::for_each( m_mapAnimations.begin(), m_mapAnimations.end(), [&]( std::pair< String, AnimationSPtr > p_pair )
		{
			if ( p_pair.second->GetState() != Animation::eSTATE_STOPPED )
			{
				p_pair.second->Update( p_rTslf );
			}
		} );
	}

	void AnimatedObject::StartAnimation( String const & p_strName )
	{
		AnimationSPtr l_animation;
		AnimationPtrStrMapIt l_it = m_mapAnimations.find( p_strName );

		if ( l_it != m_mapAnimations.end() )
		{
			l_it->second->Play();
		}
	}

	void AnimatedObject::StopAnimation( String const & p_strName )
	{
		AnimationSPtr l_animation;
		AnimationPtrStrMapIt l_it = m_mapAnimations.find( p_strName );

		if ( l_it != m_mapAnimations.end() )
		{
			l_it->second->Stop();
		}
	}

	void AnimatedObject::PauseAnimation( String const & p_strName )
	{
		AnimationSPtr l_animation;
		AnimationPtrStrMapIt l_it = m_mapAnimations.find( p_strName );

		if ( l_it != m_mapAnimations.end() )
		{
			l_it->second->Pause();
		}
	}

	void AnimatedObject::StartAllAnimations()
	{
		std::for_each( m_mapAnimations.begin(), m_mapAnimations.end(), [&]( std::pair< String, AnimationSPtr > p_pair )
		{
			p_pair.second->Play();
		} );
	}

	void AnimatedObject::StopAllAnimations()
	{
		std::for_each( m_mapAnimations.begin(), m_mapAnimations.end(), [&]( std::pair< String, AnimationSPtr > p_pair )
		{
			p_pair.second->Stop();
		} );
	}

	void AnimatedObject::PauseAllAnimations()
	{
		std::for_each( m_mapAnimations.begin(), m_mapAnimations.end(), [&]( std::pair< String, AnimationSPtr > p_pair )
		{
			p_pair.second->Pause();
		} );
	}

	AnimationSPtr AnimatedObject::GetAnimation( Castor::String const & p_strName )
	{
		AnimationSPtr l_return;
		AnimationPtrStrMapIt l_it = m_mapAnimations.find( p_strName );

		if ( l_it != m_mapAnimations.end() )
		{
			l_return = l_it->second;
		}

		return l_return;
	}

	void AnimatedObject::SetGeometry( GeometrySPtr p_pObject )
	{
		m_mapAnimations.clear();
		DoSetGeometry( p_pObject );
	}

	void AnimatedObject::SetMesh( MeshSPtr p_pMesh )
	{
		m_mapAnimations.clear();
		DoSetMesh( p_pMesh );
	}

	void AnimatedObject::SetSkeleton( SkeletonSPtr p_pSkeleton )
	{
		m_mapAnimations.clear();
		DoSetSkeleton( p_pSkeleton );
	}

	void AnimatedObject::DoSetGeometry( GeometrySPtr p_pObject )
	{
		if ( p_pObject )
		{
			DoCopyAnimations( p_pObject );
			DoSetMesh( p_pObject->GetMesh() );
		}

		m_wpGeometry = p_pObject;
	}

	void AnimatedObject::DoSetMesh( MeshSPtr p_pMesh )
	{
		if ( p_pMesh )
		{
			DoCopyAnimations( p_pMesh );
			DoSetSkeleton( p_pMesh->GetSkeleton() );
		}

		m_wpMesh = p_pMesh;
	}

	void AnimatedObject::DoSetSkeleton( SkeletonSPtr p_pSkeleton )
	{
		if ( p_pSkeleton )
		{
			DoCopyAnimations( p_pSkeleton );
		}

		m_wpSkeleton = p_pSkeleton;
	}

	void AnimatedObject::DoCopyAnimations( AnimableSPtr p_pObject )
	{
		std::for_each( p_pObject->AnimationsBegin(), p_pObject->AnimationsEnd(), [&]( std::pair< String, AnimationSPtr > p_pair )
		{
			AnimationSPtr l_pAnimation = p_pair.second;
			MovingObjectPtrStrMap l_mapToMove;
			AnimationSPtr l_pClone;
			AnimationPtrStrMap::iterator l_it = m_mapAnimations.find( l_pAnimation->GetName() );

			if ( l_it == m_mapAnimations.end() )
			{
				m_mapAnimations.insert( std::make_pair( l_pAnimation->GetName(), std::make_shared< Animation >( l_pAnimation->GetName() ) ) );
				l_it = m_mapAnimations.find( l_pAnimation->GetName() );
			}

			l_pClone = l_it->second;
			std::for_each( l_pAnimation->Begin(), l_pAnimation->End(), [&]( std::pair< String, MovingObjectBaseSPtr > p_pair )
			{
				if ( l_mapToMove.find( p_pair.first ) == l_mapToMove.end() )
				{
					p_pair.second->Clone( l_mapToMove );
				}
			} );
			std::for_each( l_mapToMove.begin(), l_mapToMove.end(), [&]( std::pair< String, MovingObjectBaseSPtr > p_pair )
			{
				l_pClone->AddMovingObject( p_pair.second );
			} );
		} );
	}
}
