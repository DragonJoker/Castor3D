/*
See LICENSE file in root folder
*/
#ifndef ___C3D_COLOUR_RENDER_BUFFER_H___
#define ___C3D_COLOUR_RENDER_BUFFER_H___

#include "RenderBuffer.hpp"

namespace castor3d
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.7.0.0
	\date		12/11/2012
	\~english
	\brief		Render buffer receiving colour PixelComponents
	\~french
	\brief		Tampon de rendu recevant une composante de couleur
	*/
	class ColourRenderBuffer
		: public RenderBuffer
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_format	The buffer pixel format
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_format	Le format des pixels du tampon
		 */
		C3D_API explicit ColourRenderBuffer( castor::PixelFormat p_format );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API virtual ~ColourRenderBuffer();
	};
}

#endif
