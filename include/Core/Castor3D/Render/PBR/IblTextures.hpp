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
		 *\param[in]	brdf	The prefiltered BRDF map.
		 *\param[in]	sampler	The sampler used for the environment map.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	scene	La scène.
		 *\param[in]	device	Le device GPU.
		 *\param[in]	source	La texture d'environnement source.
		 *\param[in]	brdf	La texture de BRDF.
		 *\param[in]	sampler	Le sampler utilisé pour la texture d'environnement.
		 */
		C3D_API explicit IblTextures( Scene & scene
			, RenderDevice const & device
			, Texture const & source
			, Texture const & brdf
			, SamplerObs sampler );
		/**
		 *\~english
		 *\brief		Updates the environment maps.
		 *\param[in]	queueData	The queue receiving the GPU commands.
		 *\~french
		 *\brief		Met à jour les textures d'environnement.
		 *\param[in]	queueData	La queue recevant les commandes GPU.
		 */
		C3D_API void update( QueueData const & queueData );
		/**
		 *\~english
		 *\brief		Updates the environment maps.
		 *\param[in]	signalsToWait	The semaphore from the previous render pass.
		 *\param[in]	queue			The queue receiving the GPU commands.
		 *\return		The semaphores signaled by this pass.
		 *\~french
		 *\brief		Met à jour les textures d'environnement.
		 *\param[in]	signalsToWait	Le sémaphore de la passe de rendu précédente.
		 *\param[in]	queue			La queue recevant les commandes GPU.
		 *\return		Les sémaphores signalés par cette passe.
		 */
		C3D_API crg::SemaphoreWaitArray update( crg::SemaphoreWaitArray signalsToWait
			, ashes::Queue const & queue )const;
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		Texture const & getIrradianceTexture()const noexcept
		{
			return m_radianceComputer.getResult();
		}

		Texture const & getPrefilteredEnvironmentTexture()const noexcept
		{
			return m_environmentPrefilter.getResult();
		}

		Texture const & getPrefilteredEnvironmentSheenTexture()const noexcept
		{
			return m_environmentSheenPrefilter.getResult();
		}

		Texture const & getPrefilteredBrdfTexture()const noexcept
		{
			return m_brdf;
		}

		ashes::Sampler const & getIrradianceSampler()const noexcept
		{
			return m_radianceComputer.getSampler();
		}

		ashes::Sampler const & getPrefilteredEnvironmentSampler()const noexcept
		{
			return m_environmentPrefilter.getSampler();
		}

		ashes::Sampler const & getPrefilteredEnvironmentSheenSampler()const noexcept
		{
			return m_environmentSheenPrefilter.getSampler();
		}

		ashes::Sampler const & getPrefilteredBrdfSampler()const noexcept
		{
			CU_Require( m_sampler != nullptr );
			return m_sampler->getSampler();
		}
		/**@}*/

	private:
		Texture const & m_brdf;
		SamplerObs m_sampler;
		RadianceComputer m_radianceComputer;
		EnvironmentPrefilter m_environmentPrefilter;
		EnvironmentPrefilter m_environmentSheenPrefilter;
	};
}

#endif
