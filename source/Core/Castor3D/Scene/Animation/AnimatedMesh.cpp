#include "Castor3D/Scene/Animation/AnimatedMesh.hpp"

#include "Castor3D/Animation/Animable.hpp"
#include "Castor3D/Model/Mesh/Mesh.hpp"
#include "Castor3D/Model/Mesh/Animation/MeshAnimation.hpp"
#include "Castor3D/Scene/Geometry.hpp"
#include "Castor3D/Scene/Animation/Mesh/MeshAnimationInstance.hpp"
#include "Castor3D/Scene/Animation/Mesh/MeshAnimationInstanceSubmesh.hpp"

namespace castor3d
{
	AnimatedMesh::AnimatedMesh( castor::String const & name
		, Mesh & mesh
		, Geometry & geometry )
		: AnimatedObject{ AnimationType::eMesh, name }
		, m_mesh{ mesh }
		, m_geometry{ geometry }
	{
	}

	void AnimatedMesh::update( castor::Milliseconds const & elapsed )
	{
		if ( m_playingAnimation )
		{
			auto real = elapsed;

			if ( m_reinit )
			{
				real = 0_ms;
				m_playingAnimation->clear();
			}

			m_playingAnimation->update( real );
			m_geometry.markDirty();

			if ( m_reinit )
			{
				m_playingAnimation = nullptr;
				m_reinit = false;
			}
		}

	}

	void AnimatedMesh::doAddAnimation( castor::String const & name )
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
		CU_Require( m_playingAnimation == nullptr );
		m_playingAnimation = &static_cast< MeshAnimationInstance & >( animation );
	}

	void AnimatedMesh::doStopAnimation( AnimationInstance & animation )
	{
		CU_Require( m_playingAnimation == &animation );
		m_reinit = true;
	}

	void AnimatedMesh::doClearAnimations()
	{
		m_reinit = true;
	}
}
