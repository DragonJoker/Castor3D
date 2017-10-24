/*
See LICENSE file in root folder
*/
#ifndef ___C3D_CpuParticleSystem_H___
#define ___C3D_CpuParticleSystem_H___

#include "ParticleSystemImpl.hpp"
#include "Particle.hpp"

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
		 *\param[in]	p_parent	The parent particle system.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_parent	Le système de particules parent.
		 */
		C3D_API explicit CpuParticleSystem( ParticleSystem & p_parent );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API virtual ~CpuParticleSystem();
		/**
		 *\copydoc		castor3d::ParticleSystemImpl::Initialise
		 */
		C3D_API bool initialise()override;
		/**
		 *\copydoc		castor3d::ParticleSystemImpl::Cleanup
		 */
		C3D_API void cleanup()override;
		/**
		 *\copydoc		castor3d::ParticleSystemImpl::addParticleVariable
		 */
		C3D_API void addParticleVariable( castor::String const & p_name, ElementType p_type, castor::String const & p_defaultValue )override;

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
		BufferDeclaration m_inputs;
		//!\~english	The particles.
		//!\~french		Les particules.
		ParticleArray m_particles;
	};
}

#endif
