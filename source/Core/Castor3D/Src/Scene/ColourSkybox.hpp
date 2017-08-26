/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef ___C3D_ColourSkybox_H___
#define ___C3D_ColourSkybox_H___

#include "Scene/Skybox.hpp"

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
		C3D_API bool initialise()override;
		/**
		 *\~english
		 *\brief		Renders the skybox.
		 *\param[in]	camera	The scene's camera.
		 *\~french
		 *\brief		Dessine la skybox.
		 *\param[in]	camera	La caméra de la scène.
		 */
		C3D_API void render( Camera const & camera )override;
		/**
		 *\~english
		 *\brief		Defines the skybox's colour.
		 *\param[in]	value	The new value.
		 *\~french
		 *\brief		Définit la couleur de la skybox.
		 *\param[in]	value	La nouvelle valeur.
		 */
		C3D_API void setColour( castor::Colour const & value );
		/**
		 *\~english
		 *\brief		Defines the skybox's colour.
		 *\param[in]	value	The new value.
		 *\~french
		 *\brief		Définit la couleur de la skybox.
		 *\param[in]	value	La nouvelle valeur.
		 */
		inline void setColour( castor::HdrColour const & value )
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
		void doUpdateTextureImage( uint32_t index
			, castor::Pixel< castor::PixelFormat::eRGB32F > const & pixel );

	private:
		//!\~english	The skybox colour.
		//!\~french		La couleur de la skybox.
		castor::ChangeTracked< castor::HdrColour > m_colour;
	};
}

#endif
