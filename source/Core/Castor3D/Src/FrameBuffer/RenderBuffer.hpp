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
#ifndef ___C3D_RENDER_BUFFER_H___
#define ___C3D_RENDER_BUFFER_H___

#include "Castor3DPrerequisites.hpp"

#include <Graphics/Size.hpp>

namespace Castor3D
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.7.0.0
	\date		12/11/2012
	\~english
	\brief		Render buffer base class
	\remark		A render buffer is a buffer that receives specific component from a frame buffer
	\~french
	\brief		Classe de base d'un tampon de rendu
	\remark		Un tampon de rendu est un tampon qui reçoit une composante spécifique d'un tampon d'image
	*/
	class RenderBuffer
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_type			The buffer component
		 *\param[in]	p_format	The buffer pixel format
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_type			La composante du tampon
		 *\param[in]	p_format	Le format des pixels du tampon
		 */
		C3D_API RenderBuffer( BufferComponent p_type, Castor::PixelFormat p_format );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API virtual ~RenderBuffer();
		/**
		 *\~english
		 *\brief		Creation function
		 *\return		\p true if OK
		 *\~french
		 *\brief		Fonction de création
		 *\return		\p true si tout s'est bien passé
		 */
		C3D_API virtual bool Create() = 0;
		/**
		 *\~english
		 *\brief		Destruction function
		 *\~french
		 *\brief		Fonction de destruction
		 */
		C3D_API virtual void Destroy() = 0;
		/**
		 *\~english
		 *\brief		Initialisation function
		 *\param[in]	p_size	The buffer dimensions
		 *\return		\p true if OK
		 *\~french
		 *\brief		Fonction d'initialisation
		 *\param[in]	p_size	Les dimensions du tampon
		 *\return		\p true si tout s'est bien passé
		 */
		C3D_API virtual bool Initialise( Castor::Size const & p_size ) = 0;
		/**
		 *\~english
		 *\brief		Cleanup function
		 *\~french
		 *\brief		Fonction de nettoyage
		 */
		C3D_API virtual void Cleanup() = 0;
		/**
		 *\~english
		 *\brief		Activation function, to tell the GPU it is active
		 *\return		\p true if successful
		 *\~french
		 *\brief		Fonction d'activation, pour dire au GPU qu'il est activé
		 *\return		\p true si tout s'est bien passé
		 */
		C3D_API virtual bool Bind() = 0;
		/**
		 *\~english
		 *\brief		Deactivation function, to tell the GPU it is inactive
		 *\~french
		 *\brief		Fonction de désactivation, pour dire au GPU qu'il est désactivé
		 */
		C3D_API virtual void Unbind() = 0;
		/**
		 *\~english
		 *\brief		Resizing function
		 *\param[in]	p_size	The buffer dimensions
		 *\return		\p true if OK
		 *\~french
		 *\brief		Fonction de redimensionnement
		 *\param[in]	p_size	Les dimensions du tampon
		 *\return		\p true si tout s'est bien passé
		 */
		C3D_API virtual bool Resize( Castor::Size const & p_size ) = 0;
		/**
		 *\~english
		 *\brief		Retrieves the buffer pixel format
		 *\return		The pixel format
		 *\~french
		 *\brief		Récupère le format des pixels du tampon
		 *\return		Le format des pixels
		 */
		inline Castor::PixelFormat GetPixelFormat()const
		{
			return m_pixelFormat;
		}
		/**
		 *\~english
		 *\brief		Retrieves the buffer dimensions
		 *\return		The dimensions
		 *\~french
		 *\brief		Récupère les dimensions du tampon
		 *\return		Les dimensions
		 */
		inline Castor::Size const & GetDimensions()const
		{
			return m_size;
		}
		/**
		 *\~english
		 *\brief		Retrieves the buffer width
		 *\return		The width
		 *\~french
		 *\brief		Récupère la largeur du tampon
		 *\return		La largeur
		 */
		inline uint32_t GetWidth()const
		{
			return m_size.width();
		}
		/**
		 *\~english
		 *\brief		Retrieves the buffer height
		 *\return		The height
		 *\~french
		 *\brief		Récupère la hauteur du tampon
		 *\return		La hauteur
		 */
		inline uint32_t GetHeight()const
		{
			return m_size.height();
		}
		/**
		 *\~english
		 *\brief		Retrieves the buffer component
		 *\return		The component
		 *\~french
		 *\brief		Récupère la composante du tampon
		 *\return		La composante
		 */
		inline BufferComponent GetComponent()const
		{
			return m_eComponent;
		}
		/**
		 *\~english
		 *\brief		Defines the buffer samples count
		 *\param[in]	p_iSamples	The samples count
		 *\~french
		 *\brief		Définit le nombre de samples du tampon
		 *\param[in]	p_iSamples	Le nombre de samples
		 */
		inline void SetSamplesCount( int p_iSamples )
		{
			m_samplesCount = p_iSamples;
		}
		/**
		 *\~english
		 *\brief		Retrieves the buffer samples count
		 *\return		The samples count
		 *\~french
		 *\brief		Récupère le nombre de samples du tampon
		 *\return		Le nombre de samples
		 */
		inline int GetSamplesCount()
		{
			return m_samplesCount;
		}

	protected:
		//!\~english The buffer dimensions	\~french Les dimensions du buffer
		Castor::Size m_size;

	private:
		//!\~english The component to which this buffer is bound	\~french La composante à laquelle ce tampon est associé
		BufferComponent m_eComponent;
		//!\~english The buffer pixel format	\~french Le format des pixels du tampon
		Castor::PixelFormat m_pixelFormat;
		//!\~english The samples count, if multisampling is enabled	\~french Le nombre d'échantillons, en cas de multisampling
		int m_samplesCount;
	};
}

#endif
