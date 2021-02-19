/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ParticleSystemImpl_H___
#define ___C3D_ParticleSystemImpl_H___

#include "ParticleModule.hpp"
#include "Castor3D/Render/RenderModule.hpp"

namespace castor3d
{
	class ParticleSystemImpl
	{
	public:
		/**
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
			CU_ScopedEnumBounds( eCpu )
		};

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	type	The implementation type.
		 *\param[in]	parent	The parent particle system.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	type	Le type d'implémentation.
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
		 *\param		device	The current device.
		 *\return		\p true if all is OK.
		 *\~french
		 *\brief		Initialise l'implémentation.
		 *\param		device	Le device actuel.
		 *\return		\p true si tout s'est bien passé.
		 */
		C3D_API virtual bool initialise( RenderDevice const & device ) = 0;
		/**
		 *\~english
		 *\brief		Cleans the implementation.
		 *\param		device	The current device.
		 *\~french
		 *\brief		Nettoie l'implémentation.
		 *\param		device	Le device actuel.
		 */
		C3D_API virtual void cleanup( RenderDevice const & device ) = 0;
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
		 *\brief			Updates the render pass, CPU wise.
		 *\param[in, out]	updater	The update data.
		 *\~french
		 *\brief			Met à jour la passe de rendu, au niveau CPU.
		 *\param[in, out]	updater	Les données d'update.
		 */
		C3D_API virtual void update( CpuUpdater & updater ) = 0;
		/**
		 *\~english
		 *\brief			Updates the render pass, GPU wise.
		 *\param[in, out]	updater	The update data.
		 *\~french
		 *\brief			Met à jour la passe de rendu, au niveau GPU.
		 *\param[in, out]	updater	Les données d'update.
		 */
		C3D_API virtual uint32_t update( GpuUpdater & updater ) = 0;
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
