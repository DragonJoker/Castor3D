/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ParticleSystemImpl_H___
#define ___C3D_ParticleSystemImpl_H___

#include "Castor3DPrerequisites.hpp"

namespace castor3d
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/11/2016
	\~english
	\brief		Particle system implementation base class.
	\~french
	\brief		Classe de base de l'implémentation d'un système de particules.
	*/
	class ParticleSystemImpl
	{
	public:
		/*!
		\author 	Sylvain DOREMUS
		\version	0.9.0
		\date		16/11/2016
		\~english
		\brief		Particle system implementation types enumeration.
		\~french
		\brief		Enumération des types d'implémentation de système de particules.
		*/
		enum class Type
		{
			//!\~english	Particles are updated on CPU.
			//!\~french		Les particules sont mises à jour sur le CPU.
			eCpu,
			//!\~english	Particles are updated using Geometry shader and Transform feedback.
			//!\~french		Les particules sont mises à jour en utilisant les geometry shaders et le Transform feedback.
			eTransformFeedback,
			//!\~english	Particles are updated using a compute shader.
			//!\~french		Les particules sont mises à jour en utilisant un compute shader.
			eComputeShader,
			CASTOR_SCOPED_ENUM_BOUNDS( eCpu )
		};

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	type		The implementation type.
		 *\param[in]	parent	The parent particle system.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	type		Le type d'implémentation.
		 *\param[in]	parent	Le système de particules parent.
		 */
		C3D_API ParticleSystemImpl( Type type, ParticleSystem & parent );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API virtual ~ParticleSystemImpl();
		/**
		 *\~english
		 *\brief		Initialises the implementation.
		 *\return		\p true if all is OK.
		 *\~french
		 *\brief		Initialise l'implémentation.
		 *\return		\p true si tout s'est bien passé.
		 */
		C3D_API virtual bool initialise() = 0;
		/**
		 *\~english
		 *\brief		Cleans the implementation.
		 *\~french
		 *\brief		Nettoie l'implémentation.
		 */
		C3D_API virtual void cleanup() = 0;
		/**
		 *\~english
		 *\brief		adds a particle variable.
		 *\param[in]	name			The variable name.
		 *\param[in]	type			The variable type.
		 *\param[in]	defaultValue	The variable default value.
		 *\~french
		 *\brief		Ajoute une variable de particule.
		 *\param[in]	name			Le nom de la variable.
		 *\param[in]	type			Le type de la variable.
		 *\param[in]	defaultValue	La valeur par défaut de la variable.
		 */
		C3D_API virtual void addParticleVariable( castor::String const & name
			, ParticleFormat type
			, castor::String const & defaultValue ) = 0;
		/**
		 *\~english
		 *\brief		Updates the particles.
		 *\param[in]	time	The time elapsed since last update.
		 *\param[in]	total	The total elapsed time.
		 *\return		The particles count.
		 *\~french
		 *\brief		Met à jour les particules.
		 *\param[in]	time	Le temps écoulé depuis la dernière mise à jour.
		 *\param[in]	total	Le temps total écoulé.
		 *\return		Le nombre de particules.
		 */
		C3D_API virtual uint32_t update( RenderPassTimer & timer
			, castor::Milliseconds const & time
			, castor::Milliseconds const & total
			, uint32_t index ) = 0;
		/**
		 *\~english
		 *\return		The implementation type.
		 *\~french
		 *\return		Le type d'implémentation.
		 */
		inline Type getType()
		{
			return m_type;
		}
		/**
		 *\~english
		 *\return		The parent particle system.
		 *\~french
		 *\return		Le système de particules parent.
		 */
		inline ParticleSystem const & getParent()const
		{
			return m_parent;
		}

	protected:
		//!\~english	The parent particle system.
		//!\~french		Le système de particules parent.
		ParticleSystem & m_parent;
		//!\~english	The implementation type.
		//!\~french		Le type d'implémentation.
		Type m_type;
	};
}

#endif
