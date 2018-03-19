/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ColourSkybox_H___
#define ___C3D_ColourSkybox_H___

#include "Scene/Skybox.hpp"

#include <Buffer/Buffer.hpp>
#include <Design/ChangeTracked.hpp>

namespace castor3d
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/05/2016
	\~english
	\brief		A skybox with no image but a colour.
	\~french
	\brief		Une skybox sans image mais avec une couleur.
	*/
	class ColourSkybox
		: public Skybox
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine	The engine.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine	Le moteur.
		 */
		C3D_API explicit ColourSkybox( Engine & engine );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API virtual ~ColourSkybox();
		/**
		 *\~english
		 *\brief		Initialisation function.
		 *\return		\p true if ok.
		 *\~french
		 *\brief		Fonction d'initialisation.
		 *\return		\p true if ok.
		 */
		C3D_API bool initialise( renderer::Format targetColour
			, renderer::Format targetDepth )override;
		/**
		 *\~english
		 *\brief		Updates the skybox.
		 *\~french
		 *\brief		Met la skybox à jour.
		 */
		C3D_API void update();
		/**
		 *\~english
		 *\brief		Defines the skybox's colour.
		 *\param[in]	value	The new value.
		 *\~french
		 *\brief		Définit la couleur de la skybox.
		 *\param[in]	value	La nouvelle valeur.
		 */
		C3D_API void setColour( castor::RgbColour const & value );
		/**
		 *\~english
		 *\brief		Defines the skybox's colour.
		 *\param[in]	value	The new value.
		 *\~french
		 *\brief		Définit la couleur de la skybox.
		 *\param[in]	value	La nouvelle valeur.
		 */
		inline void setColour( castor::HdrRgbColour const & value )
		{
			m_colour = value;
		}

	public:
		using Skybox::cleanup;
		using Skybox::getIbl;
		using Skybox::setScene;

	private:
		using Skybox::setEquiTexture;
		using Skybox::getEquiTexturePath;
		using Skybox::getTexture;
		using Skybox::setTexture;

	private:
		void doUpdateColour();

	private:
		castor::ChangeTracked< castor::HdrRgbColour > m_colour;
		renderer::BufferPtr< castor::Point3f > m_stagingBuffer;
		renderer::BufferImageCopyArray m_copyRegions;
		renderer::CommandBufferPtr m_cmdCopy;
	};
}

#endif
