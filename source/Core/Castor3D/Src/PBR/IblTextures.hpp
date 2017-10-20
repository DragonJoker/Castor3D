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
		 *\param[in]	p_scene	The scene.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_scene	La scène.
		 */
		C3D_API explicit IblTextures( Scene & p_scene );
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
		 *\param[in]	p_source	The source environment map.
		 *\~french
		 *\brief		Met à jour les textures d'environnement.
		 *\param[in]	p_source	La texture d'environnement source.
		 */
		C3D_API void update( TextureLayout const & p_source );
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
		 *\return		The irradiance texture.
		 *\~french
		 *\return		La texture d'irradiance.
		 */
		inline TextureUnit const & getIrradiance()const
		{
			return m_radianceTexture;
		}
		/**
		 *\~english
		 *\return		The prefiltered environment texture.
		 *\~french
		 *\return		La texture d'environnement préfiltrée.
		 */
		inline TextureUnit const & getPrefilteredEnvironment()const
		{
			return m_prefilteredEnvironment;
		}
		/**
		 *\~english
		 *\return		The prefiltered BRDF texture.
		 *\~french
		 *\return		La texture BRDF préfiltrée.
		 */
		inline TextureUnit const & getPrefilteredBrdf()const
		{
			return m_prefilteredBrdf;
		}

	private:
		//!\~english	The radiance texture.
		//!\~french		La texture de radiance.
		TextureUnit m_radianceTexture;
		//!\~english	The prefiltered environment texture.
		//!\~french		La texture d'environnement préfiltrée.
		TextureUnit m_prefilteredEnvironment;
		//!\~english	The prefiltered BRDF texture.
		//!\~french		La texture BRDF préfiltrée.
		TextureUnit m_prefilteredBrdf;
		//!\~english	The radiance computer.
		//!\~french		Le générateur de radiance.
		RadianceComputer m_radianceComputer;
		//!\~english	The environment prefilter pipeline.
		//!\~french		Le pipeline de préfiltrage d'environnement.
		EnvironmentPrefilter m_environmentPrefilter;
	};
}

#endif
