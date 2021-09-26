#include "Castor3D/Scene/Animation/Mesh/MeshAnimationInstance.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Model/Mesh/Animation/MeshAnimation.hpp"
#include "Castor3D/Model/Mesh/Animation/MeshAnimationKeyFrame.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Scene/Animation/AnimatedMesh.hpp"
#include "Castor3D/Scene/Animation/Mesh/MeshAnimationInstanceSubmesh.hpp"

using namespace castor;

namespace castor3d
{
	//*************************************************************************************************

	MeshAnimationInstance::MeshAnimationInstance( AnimatedMesh & object, MeshAnimation & animation )
		: AnimationInstance{ object, animation }
		, m_animatedMesh{ object }
		, m_meshAnimation{ animation }
		, m_prev{ animation.isEmpty() ? animation.end() : animation.begin() }
		, m_curr{ animation.isEmpty() ? animation.end() : animation.begin() + 1 }
	{
		for ( auto & submesh : animation.m_submeshes )
		{
			// using std::make_pair to prevent GCC from using copy ctor...
			m_submeshes.insert( std::make_pair( submesh.getSubmesh().getId(),
				MeshAnimationInstanceSubmesh{ *this, submesh } ) );
		}
	}

	MeshAnimationInstanceSubmesh const * MeshAnimationInstance::getAnimationSubmesh( uint32_t index )const
	{
		auto it = m_submeshes.find( index );
		MeshAnimationInstanceSubmesh const * result = nullptr;

		if ( it != m_submeshes.end() )
		{
			result = &it->second;
		}

		return result;
	}

	void MeshAnimationInstance::doUpdate()
	{
		if ( !m_meshAnimation.isEmpty() )
		{
			m_meshAnimation.findKeyFrame( m_currentTime
				, m_prev
				, m_curr );
			m_ratio = float( ( m_currentTime - ( *m_prev )->getTimeIndex() ).count() ) / float( ( ( *m_curr )->getTimeIndex() - ( *m_prev )->getTimeIndex() ).count() );

			for ( auto & submesh : m_submeshes )
			{
				submesh.second.update( m_ratio
					, static_cast< MeshAnimationKeyFrame const & >( *( *m_prev ) ).find( submesh.second.getSubmesh() )->second
					, static_cast< MeshAnimationKeyFrame const & >( *( *m_curr ) ).find( submesh.second.getSubmesh() )->second );
			}

			static_cast< Mesh & >( *m_meshAnimation.getOwner() ).updateContainers();
		}
	}

	//*************************************************************************************************
}
