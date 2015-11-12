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
#ifndef ___C3D_BACK_BUFFERS_H___
#define ___C3D_BACK_BUFFERS_H___

#include "FrameBuffer.hpp"

namespace Castor3D
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.8.0
	\date		20/10/2015
	\~english
	\brief		Back buffers class.
	\remark		Render window default back buffers.
	\~french
	\brief		Classe de tampons d'image de fenêtre.
	\remark		Tampons de rendu d'une fenêtre.
	*/
	class BackBuffers
		: public FrameBuffer
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\para[in]		p_engine	The engine.
		 *\~french
		 *\brief		Constructeur.
		 *\para[in]		p_engine	Le moteur.
		 */
		C3D_API BackBuffers( Engine & p_engine );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API virtual ~BackBuffers();
		/**
		 *\~english
		 *\brief		Initialises the internal buffer, in RAM memory.
		 *\param[in]	p_size		The buffer dimensions.
		 *\param[in]	p_format	The buffer pixel format.
		 *\return		\p true if successful.
		 *\~french
		 *\brief		Initialise le tampon interne, en mémoire RAM.
		 *\param[in]	p_size		Les dimensions du tampon.
		 *\param[in]	p_format	Le format des pixels du tampon.
		 *\return		\p true si tout s'est bien passé.
		 */
		C3D_API bool Initialise( Castor::Size const & p_size, Castor::ePIXEL_FORMAT p_format );
		/**
		 *\~english
		 *\brief		Activation function, to tell the GPU it is active.
		 *\param[in]	p_buffer	The frame buffer to bind.
		 *\param[in]	p_target	The frame buffer binding target.
		 *\return		\p true if successful.
		 *\~french
		 *\brief		Fonction d'activation, pour dire au GPU qu'il est activé.
		 *\param[in]	p_buffer	Le tampon d'image à activer.
		 *\param[in]	p_target	La cible d'activation du tampon d'image.
		 *\return		\p true si tout s'est bien passé.
		 */
		C3D_API virtual bool Bind( eBUFFER p_buffer, eFRAMEBUFFER_TARGET p_target ) = 0;
		/**
		 *\~english
		 *\brief		Tells if the back buffer has a fixed size.
		 *\~french
		 *\brief		Dit si le tampon de fenêtre a une taille fixe
		 */
		C3D_API virtual bool HasFixedSize()const = 0;
		/**
		 *\~english
		 *\return		The back buffer fixed size.
		 *\~french
		 *\return		La taille fixe du tampon de fenêtre.
		 */
		C3D_API virtual Castor::Size GetSize()const = 0;
		/**
		 *\~english
		 *\brief		Checks if the FBO is complete
		 *\return		\p false if the buffer is in error if there is an attachment missing
		 *\~french
		 *\brief		Vérifie si le FBO est complet
		 *\return		\p false si le tampon est en erreur ou s'il manque une attache.
		 */
		C3D_API virtual bool IsComplete()const
		{
			return true;
		}

	private:
		using FrameBuffer::Initialise;
		using FrameBuffer::Bind;
		using FrameBuffer::BlitInto;
		using FrameBuffer::SetDrawBuffers;
		using FrameBuffer::SetDrawBuffer;
		using FrameBuffer::Attach;
		using FrameBuffer::DetachAll;
		using FrameBuffer::Create;
		using FrameBuffer::SetReadBuffer;
		using FrameBuffer::CreateColourRenderBuffer;
		using FrameBuffer::CreateDepthStencilRenderBuffer;
		using FrameBuffer::IsComplete;
		using FrameBuffer::DownloadBuffer;

	private:
		/**
		 *\copydoc		Castor3D::FrameBuffer::DoBind
		 */
		C3D_API virtual bool DoBind( Castor3D::eFRAMEBUFFER_TARGET p_eTarget );
		/**
		 *\copydoc		Castor3D::FrameBuffer::DoUnbind
		 */
		C3D_API virtual void DoUnbind();

	private:
		Castor::PxBufferBaseSPtr m_buffer;
	};
}

#endif
