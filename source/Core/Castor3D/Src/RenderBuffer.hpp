/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___C3D_RENDER_BUFFER_H___
#define ___C3D_RENDER_BUFFER_H___

#include "Castor3DPrerequisites.hpp"

#include <Size.hpp>

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
	class C3D_API RenderBuffer
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_eType			The buffer component
		 *\param[in]	p_ePixelFormat	The buffer pixel format
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_eType			La composante du tampon
		 *\param[in]	p_ePixelFormat	Le format des pixels du tampon
		 */
		RenderBuffer( eBUFFER_COMPONENT p_eType, Castor::ePIXEL_FORMAT p_ePixelFormat );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~RenderBuffer();
		/**
		 *\~english
		 *\brief		Creation function
		 *\return		\p true if OK
		 *\~french
		 *\brief		Fonction de création
		 *\return		\p true si tout s'est bien passé
		 */
		virtual bool Create() = 0;
		/**
		 *\~english
		 *\brief		Destruction function
		 *\~french
		 *\brief		Fonction de destruction
		 */
		virtual void Destroy() = 0;
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
		virtual bool Initialise( Castor::Size const & p_size ) = 0;
		/**
		 *\~english
		 *\brief		Cleanup function
		 *\~french
		 *\brief		Fonction de nettoyage
		 */
		virtual void Cleanup() = 0;
		/**
		 *\~english
		 *\brief		Activation function, to tell the GPU it is active
		 *\return		\p true if successful
		 *\~french
		 *\brief		Fonction d'activation, pour dire au GPU qu'il est activé
		 *\return		\p true si tout s'est bien passé
		 */
		virtual bool Bind() = 0;
		/**
		 *\~english
		 *\brief		Deactivation function, to tell the GPU it is inactive
		 *\~french
		 *\brief		Fonction de désactivation, pour dire au GPU qu'il est désactivé
		 */
		virtual void Unbind() = 0;
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
		virtual bool Resize( Castor::Size const & p_size ) = 0;
		/**
		 *\~english
		 *\brief		Retrieves the buffer pixel format
		 *\return		The pixel format
		 *\~french
		 *\brief		Récupère le format des pixels du tampon
		 *\return		Le format des pixels
		 */
		inline Castor::ePIXEL_FORMAT GetPixelFormat()const
		{
			return m_ePixelFormat;
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
		inline eBUFFER_COMPONENT GetComponent()const
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
			m_iSamplesCount = p_iSamples;
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
			return m_iSamplesCount;
		}

	private:
		//!\~english The component to which this buffer is bound	\~french La composante à laquelle ce tampon est associé
		eBUFFER_COMPONENT m_eComponent;
		//!\~english The buffer dimensions	\~french Les dimensions du buffer
		Castor::Size m_size;
		//!\~english The buffer pixel format	\~french Le format des pixels du tampon
		Castor::ePIXEL_FORMAT m_ePixelFormat;
		//!\~english The samples count, if multisampling is enabled	\~french Le nombre d'échantillons, en cas de multisampling
		int m_iSamplesCount;
	};
}

#endif
