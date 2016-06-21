#include "AnimatedMesh.hpp"

#include "Animation/Animable.hpp"
#include "Animation/Mesh/MeshAnimation.hpp"
#include "Mesh/Mesh.hpp"
#include "Scene/Animation/Mesh/MeshAnimationInstance.hpp"

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

	void AnimatedMesh::Update( real p_tslf )
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
			auto & l_animation = static_cast< MeshAnimation & >( m_mesh.GetAnimation( p_name ) );
			auto l_instance = std::make_shared< MeshAnimationInstance >( *this, l_animation );
			m_animations.insert( { p_name, l_instance } );
		}
	}

	void AnimatedMesh::DoStartAnimation( AnimationInstanceSPtr p_animation )
	{
		REQUIRE( m_playingAnimation == nullptr );
		m_playingAnimation = std::static_pointer_cast< MeshAnimationInstance >( p_animation );
	}

	void AnimatedMesh::DoStopAnimation( AnimationInstanceSPtr p_animation )
	{
		REQUIRE( m_playingAnimation == p_animation );
		m_playingAnimation.reset();
	}

	void AnimatedMesh::DoClearAnimations()
	{
		m_playingAnimation.reset();
	}
}
