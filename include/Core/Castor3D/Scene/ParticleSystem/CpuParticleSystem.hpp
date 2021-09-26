/*
See LICENSE file in root folder
*/
#ifndef ___C3D_CpuParticleSystem_H___
#define ___C3D_CpuParticleSystem_H___

#include "Castor3D/Scene/ParticleSystem/Particle.hpp"
#include "Castor3D/Scene/ParticleSystem/ParticleEmitter.hpp"
#include "Castor3D/Scene/ParticleSystem/ParticleSystemImpl.hpp"

namespace castor3d
{
	class CpuParticleSystem
		: public ParticleSystemImpl
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	parent	The parent particle system.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	parent	Le système de particules parent.
		 */
		C3D_API explicit CpuParticleSystem( ParticleSystem & parent );
		/**
		 *\copydoc		castor3d::ParticleSystemImpl::initialise
		 */
		C3D_API bool initialise( RenderDevice const & device )override;
		/**
		 *\copydoc		castor3d::ParticleSystemImpl::cleanup
		 */
		C3D_API void cleanup( RenderDevice const & device )override;
		/**
		 *\copydoc		castor3d::ParticleSystemImpl::update
		 */
		C3D_API void update( castor3d::CpuUpdater & updater )override;
		/**
		 *\copydoc		castor3d::ParticleSystemImpl::update
		 */
		C3D_API uint32_t update( castor3d::GpuUpdater & updater )override;
		/**
		 *\copydoc		castor3d::ParticleSystemImpl::addParticleVariable
		 */
		C3D_API void addParticleVariable( castor::String const & name
			, ParticleFormat type
			, castor::String const & defaultValue )override;

	protected:
		/**
		 *\~english
		 *\brief		Adds the given emitter to the list.
		 *\~french
		 *\brief		Donne l'emitter donné à la liste.
		 */
		C3D_API ParticleEmitter * addEmitter( ParticleEmitterUPtr emitter );
		/**
		 *\~english
		 *\brief		Adds the given updater to the list.
		 *\~french
		 *\brief		Donne l'updater donné à la liste.
		 */
		C3D_API ParticleUpdater * addUpdater( ParticleUpdaterUPtr updater );
		/**
		 *\~english
		 *\brief		Called when a particle is emitted.
		 *\~french
		 *\brief		Appelé lorsqu'une particule est créée.
		 */
		C3D_API void onEmit( Particle const & particle );

	private:
		/**
		 *\~english
		 *\brief		Initialises the implementation.
		 *\return		\p true if all is OK.
		 *\~french
		 *\brief		Initialise l'implémentation.
		 *\return		\p true si tout s'est bien passé.
		 */
		C3D_API virtual bool doInitialise() = 0;
		/**
		 *\~english
		 *\brief		Cleans the implementation.
		 *\~french
		 *\brief		Nettoie l'implémentation.
		 */
		C3D_API virtual void doCleanup()
		{
		}
		/**
		 *\~english
		 *\brief		Called when a particle is emitted.
		 *\~french
		 *\brief		Appelé lorsqu'une particule est créée.
		 */
		C3D_API virtual void doOnEmit( Particle const & particle )
		{
		}
		/**
		 *\~english
		 *\brief		Called when a particle is emitted.
		 *\~french
		 *\brief		Appelé lorsqu'une particule est créée.
		 */
		C3D_API virtual void doPackParticles() = 0;

	protected:
		//!\~english	The particle's elements description.
		//!\~french		La description des éléments d'une particule.
		ParticleDeclaration m_inputs;
		//!\~english	The particles.
		//!\~french		Les particules.
		ParticleArray m_particles;
		//!\~english	The particles emitters.
		//!\~french		Les émetteurs de particules.
		ParticleEmitterArray m_emitters;
		//!\~english	The particles updaters.
		//!\~french		Les updaters de particules.
		ParticleUpdaterArray m_updaters;
		//!\~english	The first unused particle data.
		//!\~french		La première particule inutilisée.
		uint32_t m_firstUnused{ 1u };

	private:
		std::vector< ParticleEmitter::OnEmitConnection > m_onEmits;
	};
}

#endif
