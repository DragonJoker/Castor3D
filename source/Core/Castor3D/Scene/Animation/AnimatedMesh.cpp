#include "Castor3D/Scene/Animation/AnimatedMesh.hpp"

#include "Castor3D/Animation/Animable.hpp"
#include "Castor3D/Model/Mesh/Mesh.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Model/Mesh/Animation/MeshAnimation.hpp"
#include "Castor3D/Scene/Geometry.hpp"
#include "Castor3D/Scene/Animation/Mesh/MeshAnimationInstance.hpp"
#include "Castor3D/Scene/Animation/Mesh/MeshAnimationInstanceSubmesh.hpp"

CU_ImplementSmartPtr( castor3d, AnimatedMesh )

namespace castor3d
{
	AnimatedMesh::AnimatedMesh( castor::String const & name
		, Mesh & mesh
		, Geometry & geometry )
		: AnimatedObject{ AnimationType::eMesh, name }
		, m_mesh{ mesh }
		, m_geometry{ geometry }
	{
		m_ids.resize( m_mesh.getSubmeshCount() );
	}

	uint32_t AnimatedMesh::fillBuffer( Submesh const & submesh
		, MorphingWeightsConfiguration * buffer )const
	{
		uint32_t result{};

		if ( isPlayingAnimation() )
		{
			if ( auto animSubmesh = getPlayingAnimation().getAnimationSubmesh( submesh.getId() ) )
			{
				uint32_t index{};

				for ( auto const & weight : animSubmesh->getWeights() )
				{
					if ( weight != 0.0f )
					{
						buffer->morphTargetsIndices[result] = index;
						buffer->morphTargetsWeights[result] = weight;
						++result;
					}

					++index;
				}
			}
		}

		buffer->morphTargetsData->x = result;
		return result;
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

	uint32_t AnimatedMesh::getId( Submesh const & submesh )const
	{
		return m_ids[submesh.getId()];
	}

	void AnimatedMesh::setId( Submesh const & submesh
		, uint32_t id )
	{
		m_ids[submesh.getId()] = id;
	}

	void AnimatedMesh::doAddAnimation( castor::String const & name )
	{
		if ( auto it = m_animations.find( name );
			it == m_animations.end() && m_mesh.hasAnimation( name ) )
		{
			auto & animation = static_cast< MeshAnimation & >( m_mesh.getAnimation( name ) );
			auto instance = castor::makeUniqueDerived< AnimationInstance, MeshAnimationInstance >( *this, animation );
			m_animations.try_emplace( name, castor::move( instance ) );
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
