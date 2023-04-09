#include "Castor3D/Scene/Animation/Mesh/MeshAnimationInstance.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Model/Mesh/Animation/MeshAnimation.hpp"
#include "Castor3D/Model/Mesh/Animation/MeshMorphTarget.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Scene/Geometry.hpp"
#include "Castor3D/Scene/Animation/AnimatedMesh.hpp"
#include "Castor3D/Scene/Animation/Mesh/MeshAnimationInstanceSubmesh.hpp"

CU_ImplementSmartPtr( castor3d, MeshAnimationInstance )

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

	void MeshAnimationInstance::clear()
	{
		play();
		m_stopping = true;
		m_currentTime = 0_ms;
	}

	void MeshAnimationInstance::doUpdate()
	{
		if ( !m_meshAnimation.isEmpty() )
		{
			if ( m_stopping )
			{
				for ( auto & submesh : m_submeshes )
				{
					submesh.second.clear();
				}
			}
			else
			{
				m_meshAnimation.findKeyFrame( m_currentTime
					, m_prev
					, m_curr );
				auto ratio = float( ( m_currentTime - ( *m_prev )->getTimeIndex() ).count() ) / float( ( ( *m_curr )->getTimeIndex() - ( *m_prev )->getTimeIndex() ).count() );
				auto & prvKF = static_cast< MeshMorphTarget const & >( *( *m_prev ) );
				auto & curKF = static_cast< MeshMorphTarget const & >( *( *m_curr ) );

				for ( auto & submesh : m_submeshes )
				{
					auto prvIt = prvKF.find( submesh.second.getSubmesh() );
					auto curIt = curKF.find( submesh.second.getSubmesh() );
					CU_Require( prvIt != prvKF.end() && curIt != curKF.end() );
					submesh.second.update( ratio
						, prvIt->second
						, curIt->second
						, prvKF.getBoundingBox()
						, curKF.getBoundingBox() );
				}
			}

			static_cast< Mesh & >( *m_meshAnimation.getAnimable() ).updateContainers();
		}

		if ( m_stopping )
		{
			stop();
			m_stopping = false;
		}
	}

	//*************************************************************************************************
}
