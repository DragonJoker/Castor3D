#include "MeshAnimationInstance.hpp"

#include "Engine.hpp"
#include "MeshAnimationInstanceSubmesh.hpp"

#include "Animation/Mesh/MeshAnimation.hpp"
#include "Event/Frame/InitialiseEvent.hpp"
#include "Scene/Animation/AnimatedMesh.hpp"

#include "Scene/Geometry.hpp"

using namespace Castor;

namespace Castor3D
{
	//*************************************************************************************************

	MeshAnimationInstance::MeshAnimationInstance( AnimatedMesh & p_object, MeshAnimation const & p_animation )
		: AnimationInstance{ p_object, p_animation }
		, m_animatedMesh{ p_object }
		, m_meshAnimation{ p_animation }
	{
		m_submeshes.reserve(p_animation.m_submeshes.size() );

		for ( auto & l_submesh : p_animation.m_submeshes )
		{
			m_submeshes.emplace_back( *this, *l_submesh );
		}
	}

	MeshAnimationInstance::~MeshAnimationInstance()
	{
	}

	void MeshAnimationInstance::DoUpdate()
	{
		for ( auto & l_submesh : m_submeshes )
		{
			l_submesh.Update( m_currentTime );
		}
	}

	//*************************************************************************************************
}
