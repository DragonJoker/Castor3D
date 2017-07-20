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
		for ( auto & submesh : p_animation.m_submeshes )
		{
			// using std::make_pair to prevent GCC from using copy ctor...
			m_submeshes.insert( std::make_pair( submesh.GetSubmesh().GetId(), MeshAnimationInstanceSubmesh{ *this, submesh } ) );
		}
	}

	MeshAnimationInstance::~MeshAnimationInstance()
	{
	}

	MeshAnimationInstanceSubmesh const * MeshAnimationInstance::GetAnimationSubmesh( uint32_t p_index )const
	{
		auto it = m_submeshes.find( p_index );
		MeshAnimationInstanceSubmesh const * result = nullptr;

		if ( it != m_submeshes.end() )
		{
			result = &it->second;
		}

		return result;
	}

	void MeshAnimationInstance::DoUpdate()
	{
		for ( auto & submesh : m_submeshes )
		{
			submesh.second.Update( m_currentTime );
		}
	}

	//*************************************************************************************************
}
