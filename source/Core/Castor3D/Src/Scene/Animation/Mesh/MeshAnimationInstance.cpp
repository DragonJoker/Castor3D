#include "MeshAnimationInstance.hpp"

#include "Engine.hpp"
#include "MeshAnimationInstanceSubmesh.hpp"

#include "Animation/Mesh/MeshAnimation.hpp"
#include "Mesh/Submesh.hpp"
#include "Scene/Animation/AnimatedMesh.hpp"

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
			// using std::make_pair to prevent GCC from using copy ctor...
			m_submeshes.insert( std::make_pair( l_submesh.GetSubmesh().GetId(), MeshAnimationInstanceSubmesh{ *this, l_submesh } ) );
		}
	}

	MeshAnimationInstance::~MeshAnimationInstance()
	{
	}

	MeshAnimationInstanceSubmesh const * MeshAnimationInstance::GetAnimationSubmesh( uint32_t p_index )const
	{
		auto l_it = m_submeshes.find( p_index );
		MeshAnimationInstanceSubmesh const * l_result = nullptr;

		if ( l_it != m_submeshes.end() )
		{
			l_result = &l_it->second;
		}

		return l_result;
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
