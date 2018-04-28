/*
See LICENSE file in root folder
*/
#ifndef ___C3D_IblTextures_H___
#define ___C3D_IblTextures_H___

#include "PBR/BrdfPrefilter.hpp"
#include "PBR/RadianceComputer.hpp"
#include "PBR/EnvironmentPrefilter.hpp"
#include "Texture/TextureUnit.hpp"

namespace castor3d
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/05/2016
	\~english
	\brief		Holds environment maps used in IBL.
	\~french
	\brief		Contient les textures d'environnement utilisées dans l'IBL.
	*/
	class IblTextures
		: public castor::OwnedBy< Scene >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	scene	The scene.
		 *\param[in]	source	The source environment map.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	scene	La scène.
		 *\param[in]	source	La texture d'environnement source.
		 */
		C3D_API explicit IblTextures( Scene & scene
			, renderer::Texture const & source
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
		inline renderer::TextureView const & getIrradianceTexture()const
		{
			return m_radianceComputer.getResult();
		}

		inline renderer::TextureView const & getPrefilteredEnvironmentTexture()const
		{
			return m_environmentPrefilter.getResult();
		}

		inline renderer::TextureView const & getPrefilteredBrdfTexture()const
		{
			return *m_prefilteredBrdfView;
		}

		inline renderer::Sampler const & getIrradianceSampler()const
		{
			return m_radianceComputer.getSampler();
		}

		inline renderer::Sampler const & getPrefilteredEnvironmentSampler()const
		{
			return m_environmentPrefilter.getSampler();
		}

		inline renderer::Sampler const & getPrefilteredBrdfSampler()const
		{
			return m_sampler->getSampler();
		}
		/**@}*/

	private:
		renderer::TexturePtr m_prefilteredBrdf;
		renderer::TextureViewPtr m_prefilteredBrdfView;
		SamplerSPtr m_sampler;
		RadianceComputer m_radianceComputer;
		EnvironmentPrefilter m_environmentPrefilter;
	};
}

#endif
