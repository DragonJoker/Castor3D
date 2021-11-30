/*
See LICENSE file in root folder
*/
#ifndef ___C3D_IblTextures_H___
#define ___C3D_IblTextures_H___

#include "PbrModule.hpp"

#include "Castor3D/Render/Passes/CommandsSemaphore.hpp"
#include "Castor3D/Render/PBR/BrdfPrefilter.hpp"
#include "Castor3D/Render/PBR/RadianceComputer.hpp"
#include "Castor3D/Render/PBR/EnvironmentPrefilter.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"

namespace castor3d
{
	class IblTextures
		: public castor::OwnedBy< Scene >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	scene	The scene.
		 *\param[in]	device	The GPU device.
		 *\param[in]	source	The source environment map.
		 *\param[in]	sampler	The sampler used for the environment map.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	scene	La scène.
		 *\param[in]	device	Le device GPU.
		 *\param[in]	source	La texture d'environnement source.
		 *\param[in]	sampler	Le sampler utilisé pour la texture d'environnement.
		 */
		C3D_API explicit IblTextures( Scene & scene
			, RenderDevice const & device
			, Texture const & source
			, SamplerResPtr sampler );
		C3D_API ~IblTextures();
		/**
		 *\~english
		 *\brief		Updates the environment maps.
		 *\~french
		 *\brief		Met à jour les textures d'environnement.
		 */
		C3D_API void update( QueueData const & queueData );
		/**
		 *\~english
		 *\brief		Updates the environment maps.
		 *\param[in]	toWait	The semaphore from the previous render pass.
		 *\~french
		 *\brief		Met à jour les textures d'environnement.
		 *\param[in]	toWait	Le sémaphore de la passe de rendu précédente.
		 */
		C3D_API ashes::Semaphore const & update( QueueData const & queueData
			, ashes::Semaphore const & toWait );
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		inline Texture const & getIrradianceTexture()const
		{
			return m_radianceComputer.getResult();
		}

		inline Texture const & getPrefilteredEnvironmentTexture()const
		{
			return m_environmentPrefilter.getResult();
		}

		inline Texture const & getPrefilteredBrdfTexture()const
		{
			return m_prefilteredBrdf;
		}

		inline ashes::Sampler const & getIrradianceSampler()const
		{
			return m_radianceComputer.getSampler();
		}

		inline ashes::Sampler const & getPrefilteredEnvironmentSampler()const
		{
			return m_environmentPrefilter.getSampler();
		}

		inline ashes::Sampler const & getPrefilteredBrdfSampler()const
		{
			return m_sampler.lock()->getSampler();
		}
		/**@}*/

	private:
		Texture m_prefilteredBrdf;
		SamplerResPtr m_sampler;
		RadianceComputer m_radianceComputer;
		EnvironmentPrefilter m_environmentPrefilter;
	};
}

#endif
