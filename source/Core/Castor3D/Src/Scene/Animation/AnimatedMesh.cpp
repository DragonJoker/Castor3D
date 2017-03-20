#include "AnimatedMesh.hpp"

#include "Animation/Animable.hpp"
#include "Animation/Mesh/MeshAnimation.hpp"
#include "Mesh/Mesh.hpp"
#include "Scene/Animation/Mesh/MeshAnimationInstance.hpp"
#include "Scene/Animation/Mesh/MeshAnimationInstanceSubmesh.hpp"

using namespace Castor;

namespace Castor3D
{
	AnimatedMesh::AnimatedMesh( String const & p_name, Mesh & p_mesh )
		: AnimatedObject{ p_name }
		, m_mesh{ p_mesh }
	{
	}

	AnimatedMesh::~AnimatedMesh()
	{
	}

	void AnimatedMesh::Update( std::chrono::milliseconds const & p_tslf )
	{
		if ( m_playingAnimation )
		{
			m_playingAnimation->Update( p_tslf );
		}
	}

	void AnimatedMesh::DoAddAnimation( String const & p_name )
	{
		auto l_it = m_animations.find( p_name );

		if ( l_it == m_animations.end() )
		{
			if ( m_mesh.HasAnimation( p_name ) )
			{
				auto & l_animation = static_cast< MeshAnimation & >( m_mesh.GetAnimation( p_name ) );
				auto l_instance = std::make_unique< MeshAnimationInstance >( *this, l_animation );
				m_animations.emplace( p_name, std::move( l_instance ) );
			}
		}
	}

	void AnimatedMesh::DoStartAnimation( AnimationInstance & p_animation )
	{
		REQUIRE( m_playingAnimation == nullptr );
		m_playingAnimation = &static_cast< MeshAnimationInstance & >( p_animation );
	}

	void AnimatedMesh::DoStopAnimation( AnimationInstance & p_animation )
	{
		REQUIRE( m_playingAnimation == &p_animation );
		m_playingAnimation = nullptr;
	}

	void AnimatedMesh::DoClearAnimations()
	{
		m_playingAnimation = nullptr;
	}
}
