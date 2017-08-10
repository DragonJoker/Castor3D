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
#ifndef ___C3D_FRAME_BUFFER_ATTACHMENT_H___
#define ___C3D_FRAME_BUFFER_ATTACHMENT_H___

#include "Castor3DPrerequisites.hpp"
#include <Graphics/Rectangle.hpp>

namespace castor3d
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.7.0.0
	\date		12/11/2012
	\~english
	\brief		Description of an attachment between a frame buffer and a render buffer
	\~french
	\brief		Description d'une liaison entre un tampon d'image et un tamon de rendu
	*/
	class FrameBufferAttachment
	{
	protected:
		friend class FrameBuffer;
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_type	The attachment type.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_type	Le type d'attache
		 */
		C3D_API explicit FrameBufferAttachment( AttachmentType p_type );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API virtual ~FrameBufferAttachment();

	public:
		/**
		 *\~english
 		 *\brief			downloads the attach content in the given buffer.
		 *\param[in]		p_offset	The offset position.
		 *\param[in,out]	p_buffer	Receives the data, and describes the area dimensions and its pixel format.
		 *\~french
		 *\brief			Copie les données de l'attache dans le tampon donné.
		 *\param[in]		p_offset	La position de décalage.
		 *\param[in,out]	p_buffer	Reçoit les données, et décrit les dimensions à copier, ainsi que le format voulu pour les pixels.
		 */
		C3D_API void download( castor::Position const & p_offset
			, castor::PxBufferBase & p_buffer )const;
		/**
		 *\~english
		 *\brief		Attaches the render buffer to the currently bound frame buffer, at given attachment point.
		 *\param[in]	p_attachment	The attachment point.
		 *\param[in]	p_index			The attachment index.
		 *\~french
		 *\brief		Attache le tampon de rendu au tampon d'image actuellement actif, au point d'attache voulu.
		 *\param[in]	p_attachment	Le point d'attache.
		 *\param[in]	p_index			L'index d'attache.
		 */
		C3D_API void attach( AttachmentPoint p_attachment
			, uint8_t p_index );
		/**
		 *\~english
		 *\brief		Attaches the render buffer to the currently bound frame buffer, at given attachment point.
		 *\param[in]	p_attachment	The attachment point.
		 *\~french
		 *\brief		Attache le tampon de rendu au tampon d'image actuellement actif, au point d'attache voulu.
		 *\param[in]	p_attachment	Le point d'attache.
		 */
		C3D_API void attach( AttachmentPoint p_attachment );
		/**
		 *\~english
		 *\brief		Detaches the render buffer from the frame buffer
		 *\~french
		 *\brief		Détache le tampon de rendu du tampon d'image
		 */
		C3D_API void detach();
		/**
		 *\~english
		 *\brief		Clears the attachment buffer.
		 *\param[in]	p_colour	The clear colour.
		 *\~french
		 *\brief		Vide le tampon de l'attache.
		 *\param[in]	p_colour	La couleur de vidage.
		 */
		C3D_API void clear( castor::Colour const & p_colour )const;
		/**
		 *\~english
		 *\brief		Clears the attachment buffer.
		 *\param[in]	p_depth		The depth clear value.
		 *\~french
		 *\brief		Vide le tampon de l'attache.
		 *\param[in]	p_depth		La valeur de nettoyage pour la profondeur.
		 */
		C3D_API void clear( float p_depth )const;
		/**
		 *\~english
		 *\brief		Clears the attachment buffer.
		 *\param[in]	p_stencil	The stencil clear value.
		 *\~french
		 *\brief		Vide le tampon de l'attache.
		 *\param[in]	p_stencil	La valeur de nettoyage pour le stencil.
		 */
		C3D_API void clear( int p_stencil )const;
		/**
		 *\~english
		 *\brief		Clears the attachment buffer.
		 *\param[in]	p_depth		The depth clear value.
		 *\param[in]	p_stencil	The stencil clear value.
		 *\~french
		 *\brief		Vide le tampon de l'attache.
		 *\param[in]	p_depth		La valeur de nettoyage pour la profondeur.
		 *\param[in]	p_stencil	La valeur de nettoyage pour le stencil.
		 */
		C3D_API void clear( float p_depth, int p_stencil )const;
		/**
		*\~english
		*\return		The data buffer.
		*\~french
		*\return		Le tampon de pixels.
		*/
		C3D_API virtual castor::PxBufferBaseSPtr getBuffer()const = 0;
		/**
		 *\~english
		 *\return		The attachment point
		 *\~french
		 *\return		Le point d'attache
		 */
		inline AttachmentPoint getAttachmentPoint()const
		{
			return m_point;
		}
		/**
		 *\~english
		 *\return		The attachment type.
		 *\~french
		 *\return		Le type d'attache.
		 */
		inline AttachmentType getAttachmentType()const
		{
			return m_type;
		}
		/**
		 *\~english
		 *\return		The attachment index.
		 *\~french
		 *\return		L'index d'attache.
		 */
		inline uint8_t getAttachmentIndex()const
		{
			return m_index;
		}

	protected:
		/**
		 *\~english
		 *\brief			downloads the attach content in the given buffer.
		 *\param[in]		p_offset	The offset position.
		 *\param[in,out]	p_buffer	Receives the data, and describes the area dimensions and its pixel format.
		 *\~french
		 *\brief			Copie les données de l'attache dans le tampon donné.
		 *\param[in]		p_offset	La position de décalage.
		 *\param[in,out]	p_buffer	Reçoit les données, et décrit les dimensions à copier, ainsi que le format voulu pour les pixels.
		 */
		C3D_API virtual void doDownload( castor::Position const & p_offset
			, castor::PxBufferBase & p_buffer )const = 0;
		/**
		 *\~english
		 *\brief		Attaches the render buffer to the frame buffer, at given attachment point
		 *\~french
		 *\brief		Attache le tampon de rendu au tampon d'image, au point d'attache voulu
		 */
		C3D_API virtual void doAttach() = 0;
		/**
		 *\~english
		 *\brief		Detaches the render buffer from the frame buffer
		 *\~french
		 *\brief		Détache le tampon de rendu du tampon d'image
		 */
		C3D_API virtual void doDetach() = 0;
		/**
		 *\~english
		 *\brief		Clears the attachment buffer.
		 *\param[in]	p_colour	The clear colour.
		 *\~french
		 *\brief		Vide le tampon de l'attache.
		 *\param[in]	p_colour	La couleur de vidage.
		 */
		C3D_API virtual void doClear( castor::Colour const & p_colour )const = 0;
		/**
		 *\~english
		 *\brief		Clears the attachment buffer.
		 *\param[in]	p_depth		The depth clear value.
		 *\~french
		 *\brief		Vide le tampon de l'attache.
		 *\param[in]	p_depth		La valeur de nettoyage pour la profondeur.
		 */
		C3D_API virtual void doClear( float p_depth )const = 0;
		/**
		 *\~english
		 *\brief		Clears the attachment buffer.
		 *\param[in]	p_stencil	The stencil clear value.
		 *\~french
		 *\brief		Vide le tampon de l'attache.
		 *\param[in]	p_stencil	La valeur de nettoyage pour le stencil.
		 */
		C3D_API virtual void doClear( int p_stencil )const = 0;
		/**
		 *\~english
		 *\brief		Clears the attachment buffer.
		 *\param[in]	p_depth		The depth clear value.
		 *\param[in]	p_stencil	The stencil clear value.
		 *\~french
		 *\brief		Vide le tampon de l'attache.
		 *\param[in]	p_depth		La valeur de nettoyage pour la profondeur.
		 *\param[in]	p_stencil	La valeur de nettoyage pour le stencil.
		 */
		C3D_API virtual void doClear( float p_depth, int p_stencil )const = 0;

	private:
		//!\~english	The attachment type.
		//!\~french		Le type d'attache.
		AttachmentType m_type;
		//!\~english	The attachment point.
		//!\~french		Le point d'attache.
		AttachmentPoint m_point;
		//!\~english	The attachment index.
		//!\~french		L'index d'attache.
		uint8_t m_index;
	};
}

#endif
