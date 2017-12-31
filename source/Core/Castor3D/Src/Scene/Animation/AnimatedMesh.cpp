#include "AnimatedMesh.hpp"

#include "Animation/Animable.hpp"
#include "Animation/Mesh/MeshAnimation.hpp"
#include "Mesh/Mesh.hpp"
#include "Scene/Animation/Mesh/MeshAnimationInstance.hpp"
#include "Scene/Animation/Mesh/MeshAnimationInstanceSubmesh.hpp"

using namespace castor;

namespace castor3d
{
	AnimatedMesh::AnimatedMesh( String const & name
		, Mesh & mesh
		, Geometry & geometry )
		: AnimatedObject{ name }
		, m_mesh{ mesh }
		, m_geometry{ geometry }
	{
	}

	AnimatedMesh::~AnimatedMesh()
	{
	}

	void AnimatedMesh::update( Milliseconds const & elpased )
	{
		if ( m_playingAnimation )
		{
			m_playingAnimation->update( elpased );
		}
	}

	void AnimatedMesh::doAddAnimation( String const & name )
	{
		auto it = m_animations.find( name );

		if ( it == m_animations.end() )
		{
			if ( m_mesh.hasAnimation( name ) )
			{
				auto & animation = static_cast< MeshAnimation & >( m_mesh.getAnimation( name ) );
				auto instance = std::make_unique< MeshAnimationInstance >( *this, animation );
				m_animations.emplace( name, std::move( instance ) );
			}
		}
	}

	void AnimatedMesh::doStartAnimation( AnimationInstance & animation )
	{
		REQUIRE( m_playingAnimation == nullptr );
		m_playingAnimation = &static_cast< MeshAnimationInstance & >( animation );
	}

	void AnimatedMesh::doStopAnimation( AnimationInstance & animation )
	{
		REQUIRE( m_playingAnimation == &animation );
		m_playingAnimation = nullptr;
	}

	void AnimatedMesh::doClearAnimations()
	{
		m_playingAnimation = nullptr;
	}
}
