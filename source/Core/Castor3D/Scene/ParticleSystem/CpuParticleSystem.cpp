#include "Castor3D/Scene/ParticleSystem/CpuParticleSystem.hpp"

#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Scene/BillboardList.hpp"
#include "Castor3D/Scene/SceneNode.hpp"
#include "Castor3D/Scene/ParticleSystem/ParticleEmitter.hpp"
#include "Castor3D/Scene/ParticleSystem/ParticleUpdater.hpp"
#include "Castor3D/Scene/ParticleSystem/ParticleSystem.hpp"

using namespace castor;

namespace castor3d
{
	CpuParticleSystem::CpuParticleSystem( ParticleSystem & parent )
		: ParticleSystemImpl{ ParticleSystemImpl::Type::eCpu, parent }
	{
	}

	CpuParticleSystem::~CpuParticleSystem()
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
		auto & device = updater.device;
		auto & vbo = m_parent.getBillboards()->getVertexBuffer();
		VkDeviceSize stride = m_inputs.stride();
		auto mappedSize = ashes::getAlignedSize( VkDeviceSize( m_firstUnused * stride )
			, device.properties.limits.nonCoherentAtomSize );

		if ( auto dst = vbo.getBuffer().lock( 0u, mappedSize, 0u ) )
		{
			for ( auto i = 0u; i < m_firstUnused; ++i )
			{
				std::memcpy( dst, m_particles[i].getData(), stride );
				dst += stride;
			}

			vbo.getBuffer().flush( 0u, mappedSize );
			vbo.getBuffer().unlock();
		}

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
