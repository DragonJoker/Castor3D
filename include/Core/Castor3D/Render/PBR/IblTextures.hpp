/*
See LICENSE file in root folder
*/
#ifndef ___C3D_IblTextures_H___
#define ___C3D_IblTextures_H___

#include "PbrModule.hpp"

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
		 *\param[in]	source	The source environment map.
		 *\param[in]	sampler	The sampler used for the environment map.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	scene	La scène.
		 *\param[in]	source	La texture d'environnement source.
		 *\param[in]	sampler	Le sampler utilisé pour la texture d'environnement.
		 */
		C3D_API explicit IblTextures( Scene & scene
			, ashes::Image const & source
			, SamplerSPtr sampler );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API virtual ~IblTextures();
		/**
		 *\~english
		 *\brief		Updates the environment maps.
		 *\~french
		 *\brief		Met à jour les textures d'environnement.
		 */
		C3D_API void update();
		/**
		 *\~english
		 *\brief		Displays the maps.
		 *\param[in]	size	The render size.
		 *\~french
		 *\brief		Affiche les textures.
		 *\param[in]	size	La taille du rendu.
		 */
		C3D_API void debugDisplay( castor::Size const & size )const;
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		inline ashes::ImageView const & getIrradianceTexture()const
		{
			return m_radianceComputer.getResult();
		}

		inline ashes::ImageView const & getPrefilteredEnvironmentTexture()const
		{
			return m_environmentPrefilter.getResult();
		}

		inline ashes::ImageView const & getPrefilteredBrdfTexture()const
		{
			return m_prefilteredBrdfView;
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
			return m_sampler->getSampler();
		}
		/**@}*/

	private:
		ashes::ImagePtr m_prefilteredBrdf;
		ashes::ImageView m_prefilteredBrdfView;
		SamplerSPtr m_sampler;
		RadianceComputer m_radianceComputer;
		EnvironmentPrefilter m_environmentPrefilter;
	};
}

#endif
