#include "Castor3D/Scene/ParticleSystem/CpuParticleSystem.hpp"

#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Scene/BillboardList.hpp"
#include "Castor3D/Scene/SceneNode.hpp"
#include "Castor3D/Scene/ParticleSystem/ParticleEmitter.hpp"
#include "Castor3D/Scene/ParticleSystem/ParticleUpdater.hpp"
#include "Castor3D/Scene/ParticleSystem/ParticleSystem.hpp"

CU_ImplementCUSmartPtr( castor3d, CpuParticleSystem )

using namespace castor;

namespace castor3d
{
	CpuParticleSystem::CpuParticleSystem( ParticleSystem & parent )
		: ParticleSystemImpl{ ParticleSystemImpl::Type::eCpu, parent }
	{
	}

	bool CpuParticleSystem::initialise( RenderDevice const & device )
	{
		m_firstUnused = 1u;
		m_particles.reserve( m_parent.getMaxParticlesCount() );
		auto & defaultValues = m_parent.getDefaultValues();

		for ( auto i = 0u; i < m_parent.getMaxParticlesCount(); ++i )
		{
			m_particles.emplace_back( m_inputs, defaultValues );
		}

		return doInitialise();
	}

	void CpuParticleSystem::cleanup( RenderDevice const & device )
	{
		doCleanup();
		m_particles.clear();
		m_emitters.clear();
		m_updaters.clear();
		m_firstUnused = 1u;
	}

	void CpuParticleSystem::update( castor3d::CpuUpdater & updater )
	{
		auto firstUnused = m_firstUnused;

		for ( auto i = 0u; i < firstUnused; ++i )
		{
			m_updaters.front()->update( updater.time
				, m_particles[i] );
		}

		doPackParticles();
	}

	uint32_t CpuParticleSystem::update( castor3d::GpuUpdater & updater )
	{
		auto & vbo = m_parent.getBillboards()->getVertexBuffer();
		VkDeviceSize stride = m_inputs.stride();
		auto * dst = vbo.getData().data();

		for ( auto i = 0u; i < m_firstUnused; ++i )
		{
			std::memcpy( dst, m_particles[i].getData(), stride );
			dst += stride;
		}

		vbo.markDirty( VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT
			, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT );
		return m_firstUnused;
	}

	void CpuParticleSystem::addParticleVariable( castor::String const & name, ParticleFormat type, castor::String const & defaultValue )
	{
		m_inputs.push_back( ParticleElementDeclaration{ name, ElementUsage::eUnknown, type, m_inputs.stride() } );
	}

	void CpuParticleSystem::onEmit( Particle const & particle )
	{
		m_particles[m_firstUnused++] = particle;
		doOnEmit( particle );
	}

	ParticleEmitter * CpuParticleSystem::addEmitter( ParticleEmitterUPtr emitter )
	{
		m_emitters.emplace_back( std::move( emitter ) );
		auto result = m_emitters.back().get();

		if ( result )
		{
			m_onEmits.emplace_back( result->onEmit.connect( [this]( castor3d::Particle const & particle )
				{
					onEmit( particle );
				} ) );
		}

		return result;
	}

	ParticleUpdater * CpuParticleSystem::addUpdater( ParticleUpdaterUPtr updater )
	{
		m_updaters.emplace_back( std::move( updater ) );
		return m_updaters.back().get();
	}
}
