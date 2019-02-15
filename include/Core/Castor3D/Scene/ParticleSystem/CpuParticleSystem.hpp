/*
See LICENSE file in root folder
*/
#ifndef ___C3D_CpuParticleSystem_H___
#define ___C3D_CpuParticleSystem_H___

#include "Castor3D/Scene/ParticleSystem/Particle.hpp"
#include "Castor3D/Scene/ParticleSystem/ParticleDeclaration.hpp"
#include "Castor3D/Scene/ParticleSystem/ParticleSystemImpl.hpp"

namespace castor3d
{
	using ParticleArray = std::vector< Particle >;
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/11/2016
	\~english
	\brief		CPU only Particle system implementation.
	\~french
	\brief		Implémentation d'un système de particules CPU.
	*/
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
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API virtual ~CpuParticleSystem();
		/**
		 *\copydoc		castor3d::ParticleSystemImpl::initialise
		 */
		C3D_API bool initialise()override;
		/**
		 *\copydoc		castor3d::ParticleSystemImpl::cleanup
		 */
		C3D_API void cleanup()override;
		/**
		 *\copydoc		castor3d::ParticleSystemImpl::addParticleVariable
		 */
		C3D_API void addParticleVariable( castor::String const & name
			, ParticleFormat type
			, castor::String const & defaultValue )override;

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
		C3D_API virtual void doCleanup() = 0;

	protected:
		//!\~english	The particle's elements description.
		//!\~french		La description des éléments d'une particule.
		ParticleDeclaration m_inputs;
		//!\~english	The particles.
		//!\~french		Les particules.
		ParticleArray m_particles;
	};
}

#endif
