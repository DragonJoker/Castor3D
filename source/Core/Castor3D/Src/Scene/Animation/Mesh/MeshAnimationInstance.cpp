#include "MeshAnimationInstance.hpp"

#include "Engine.hpp"
#include "MeshAnimationInstanceSubmesh.hpp"

#include "Animation/Mesh/MeshAnimation.hpp"
#include "Event/Frame/InitialiseEvent.hpp"
#include "Mesh/Submesh.hpp"
#include "Scene/Animation/AnimatedMesh.hpp"

#include "Scene/Geometry.hpp"

using namespace Castor;

namespace Castor3D
{
	//*************************************************************************************************

	MeshAnimationInstance::MeshAnimationInstance( AnimatedMesh & p_object, MeshAnimation & p_animation )
		: AnimationInstance{ p_object, p_animation }
		, m_animatedMesh{ p_object }
		, m_meshAnimation{ p_animation }
	{
		for ( auto & l_submesh : p_animation.m_submeshes )
		{
			m_submeshes.insert( { l_submesh.GetSubmesh().GetId(), MeshAnimationInstanceSubmesh{ *this, l_submesh } } );
		}
	}

	MeshAnimationInstance::~MeshAnimationInstance()
	{
	}

	inline MeshAnimationInstanceSubmesh const * MeshAnimationInstance::GetAnimationSubmesh( uint32_t p_index )const
	{
		auto l_it = m_submeshes.find( p_index );
		MeshAnimationInstanceSubmesh const * l_return = nullptr;

		if ( l_it != m_submeshes.end() )
		{
			l_return = &l_it->second;
		}

		return l_return;
	}

	void MeshAnimationInstance::DoUpdate()
	{
		for ( auto & l_submesh : m_submeshes )
		{
			l_submesh.second.Update( m_currentTime );
		}
	}

	//*************************************************************************************************
}
