/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)

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
#ifndef ___C3D_FRAME_BUFFER_H___
#define ___C3D_FRAME_BUFFER_H___

#include "Castor3DPrerequisites.hpp"

#include <Colour.hpp>
#include <OwnedBy.hpp>
#include <Rectangle.hpp>

#include <memory>

namespace Castor3D
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.7.0.0
	\date		12/11/2012
	\~english
	\brief		Frame buffer class
	\remark		A frame buffer is where a render target makes its render
	\~french
	\brief		Classe de tampon d'image
	\remark		Un tampon d'image est là où une cible de rendu fait son rendu
	*/
	class FrameBuffer
		: public Castor::OwnedBy< Engine >
		, public std::enable_shared_from_this< FrameBuffer >
	{
	public:
		DECLARE_VECTOR( FrameBufferAttachmentSPtr, Attach );
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_engine	The core engine
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_engine	Le moteur
		 */
		C3D_API FrameBuffer( Engine & p_engine );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API virtual ~FrameBuffer();
		/**
		 *\~english
		 *\brief		Initialises color and depth cache buffers.
		 *\param[in]	p_size	The frame buffer size.
		 *\return		\p true if OK.
		 *\~french
		 *\brief		Initialise les tampons de cache de couleur de profondeur.
		 *\param[in]	p_size	La taille du tampon d'image.
		 *\return		\p true si tout s'est bien passé.
		 */
		C3D_API bool Initialise( Castor::Size const & p_size );
		/**
		 *\~english
		 *\brief		Cleans up cache buffers.
		 *\~french
		 *\brief		Nettoie les tampons de cache.
		 */
		C3D_API void Cleanup();
		/**
		 *\~english
		 *\brief		Defines the colour used when Clear is called on the color buffer.
		 *\param[in]	p_colour	The colour.
		 *\~french
		 *\brief		Définit la couleur utilisée quand Clear est appelée sur le tampon couleur.
		 *\param[in]	p_colour	La couleur.
		 */
		C3D_API void SetClearColour( Castor::Colour const & p_colour );
		/**
		 *\~english
		 *\brief		Clears the buffers.
		 *\~french
		 *\brief		Vide les tampons.
		 */
		C3D_API void Clear();
		/**
		 *\~english
		 *\brief		Activation function, to tell the GPU it is active
		 *\param[in]	p_target	The frame buffer binding target
		 *\param[in]	p_mode		The frame buffer binding mode
		 *\return		\p true if successful
		 *\~french
		 *\brief		Fonction d'activation, pour dire au GPU qu'il est activé
		 *\param[in]	p_target	La cible d'activation du tampon d'image
		 *\param[in]	p_mode		Le mode d'activation du tampon d'image
		 *\return		\p true si tout s'est bien passé
		 */
		C3D_API bool Bind( eFRAMEBUFFER_MODE p_mode = eFRAMEBUFFER_MODE_AUTOMATIC, eFRAMEBUFFER_TARGET p_target = eFRAMEBUFFER_TARGET_BOTH );
		/**
		 *\~english
		 *\brief		Deactivation function, to tell the GPU it is inactive
		 *\~french
		 *\brief		Fonction de désactivation, pour dire au GPU qu'il est désactivé
		 */
		C3D_API void Unbind();
		/**
		 *\~english
		 *\brief		Blit this frame buffer into the given one
		 *\param[in]	p_buffer		The buffer receiving this one
		 *\param[in]	p_rect			The rectangle
		 *\param[in]	p_components	Bitwise OR of eBUFFER_COMPONENT indicating which buffers are to be copied
		 *\return		\p true if successful
		 *\~french
		 *\brief		Blitte ce tampon dans celui donné
		 *\param[in]	p_buffer		Le tampon recevant celui-ci
		 *\param[in]	p_rect			Le rectangle
		 *\param[in]	p_components	OU logique de eBUFFER_COMPONENT indiquant les buffers à copier
		 *\return		\p true si tout s'est bien passé
		 */
		C3D_API bool BlitInto( FrameBufferSPtr p_buffer, Castor::Rectangle const & p_rect, uint32_t p_components );
		/**
		 *\~english
		 *\brief		Stretches this frame buffer into the given one.
		 *\remarks		Interpolation for depth or stencil buffer must be eINTERPOLATION_MODE_NEAREST.
		 *\param[in]	p_buffer		The buffer receiving this one.
		 *\param[in]	p_rectSrc		The source rectangle.
		 *\param[in]	p_rectDst		The destination rectangle.
		 *\param[in]	p_components	Bitwise OR of eBUFFER_COMPONENT indicating which buffers are to be copied.
		 *\param[in]	p_interpolation	The interpolation to apply if the image is stretched.
		 *\return		\p true if successful.
		 *\~french
		 *\brief		Stretche ce tampon dans celui donné.
		 *\param[in]	p_buffer		Le tampon recevant celui-ci.
		 *\param[in]	p_rectSrc		Le rectangle source.
		 *\param[in]	p_rectDst		Le rectangle destination.
		 *\param[in]	p_components	OU logique de eBUFFER_COMPONENT indiquant les buffers à copier.
		 *\param[in]	p_interpolation	L'interpolation à appliquer si l'image est redimensionnée.
		 *\return		\p true si tout s'est bien passé.
		 */
		C3D_API bool StretchInto( FrameBufferSPtr p_buffer, Castor::Rectangle const & p_rectSrc, Castor::Rectangle const & p_rectDst, uint32_t p_components, eINTERPOLATION_MODE p_interpolation );
		/**
		 *\~english
		 *\brief		Specifies the buffers to be drawn into
		 *\remarks		All buffers attached are selected
		 *\~french
		 *\brief		Définit les buffers dans lesquels le dessin doit être effectué
		 *\remarks		Tous les buffers attachés sont sélectionnés
		 */
		C3D_API void SetDrawBuffers();
		/**
		 *\~english
		 *\brief		Specifies the color buffer to be drawn into
		 *\param[in]	p_attach	The color buffer
		 *\~french
		 *\brief		Définit le tampon de couleur dans lequel le dessin doit être effectué
		 *\param[in]	p_attach	Le tampon de couleur
		 */
		C3D_API void SetDrawBuffer( TextureAttachmentSPtr p_attach );
		/**
		 *\~english
		 *\brief		Specifies the color buffer to be drawn into
		 *\param[in]	p_attach	The color buffer
		 *\~french
		 *\brief		Définit le tampon de couleur dans lequel le dessin doit être effectué
		 *\param[in]	p_attach	Le tampon de couleur
		 */
		C3D_API void SetDrawBuffer( RenderBufferAttachmentSPtr p_attach );
		/**
		 *\~english
		 *\brief		Attaches a texture to this frame buffer, at given attachment point
		 *\param[in]	p_point		The attachment point
		 *\param[in]	p_index		The attachment index
		 *\param[in]	p_texture	The texture
		 *\param[in]	p_target	The dimension to which the texture must be attached
		 *\param[in]	p_layer		The associated layer, if p_dimension equal eTEXTURE_TYPE_3D or eTEXTURE_TYPE_2DARRAY
		 *\return		\p true if OK
		 *\~french
		 *\brief		Attache une texture à ce tampon d'image, au point d'attache voulu
		 *\param[in]	p_point		Le point d'attache
		 *\param[in]	p_index		L'index d'attache
		 *\param[in]	p_texture	La texture
		 *\param[in]	p_target	La dimension à laquelle la texture doit être attachée
		 *\param[in]	p_layer		La couche associée, si p_dimension vaut eTEXTURE_TYPE_3D or eTEXTURE_TYPE_2DARRAY
		 *\return		\p true si tout s'est bien passé
		 */
		C3D_API bool Attach( eATTACHMENT_POINT p_point, uint8_t p_index, TextureAttachmentSPtr p_texture, eTEXTURE_TYPE p_target, int p_layer = 0 );
		/**
		 *\~english
		 *\brief		Attaches a texture to this frame buffer, at given attachment point
		 *\param[in]	p_point		The attachment point
		 *\param[in]	p_texture	The texture
		 *\param[in]	p_target	The dimension to which the texture must be attached
		 *\param[in]	p_layer		The associated layer, if p_dimension equal eTEXTURE_TYPE_3D or eTEXTURE_TYPE_2DARRAY
		 *\return		\p true if OK
		 *\~french
		 *\brief		Attache une texture à ce tampon d'image, au point d'attache voulu
		 *\param[in]	p_point		Le point d'attache
		 *\param[in]	p_texture	La texture
		 *\param[in]	p_target	La dimension à laquelle la texture doit être attachée
		 *\param[in]	p_layer		La couche associée, si p_dimension vaut eTEXTURE_TYPE_3D or eTEXTURE_TYPE_2DARRAY
		 *\return		\p true si tout s'est bien passé
		 */
		C3D_API bool Attach( eATTACHMENT_POINT p_point, TextureAttachmentSPtr p_texture, eTEXTURE_TYPE p_target, int p_layer = 0 );
		/**
		 *\~english
		 *\brief		Attaches a render buffer to this frame buffer, at given attachment point
		 *\param[in]	p_point			The attachment point
		 *\param[in]	p_index			The attachment index
		 *\param[in]	p_renderBuffer	The render buffer
		 *\return		\p true if OK
		 *\~french
		 *\brief		Attache un tampon de rendu à ce tampon d'image, au point d'attache voulu
		 *\param[in]	p_point			Le point d'attache
		 *\param[in]	p_index			L'index d'attache
		 *\param[in]	p_renderBuffer	Le tampon de rendu
		 *\return		\p true si tout s'est bien passé
		 */
		C3D_API bool Attach( eATTACHMENT_POINT p_point, uint8_t p_index, RenderBufferAttachmentSPtr p_renderBuffer );
		/**
		 *\~english
		 *\brief		Attaches a render buffer to this frame buffer, at given attachment point
		 *\param[in]	p_point			The attachment point
		 *\param[in]	p_renderBuffer	The render buffer
		 *\return		\p true if OK
		 *\~french
		 *\brief		Attache un tampon de rendu à ce tampon d'image, au point d'attache voulu
		 *\param[in]	p_point			Le point d'attache
		 *\param[in]	p_renderBuffer	Le tampon de rendu
		 *\return		\p true si tout s'est bien passé
		 */
		C3D_API bool Attach( eATTACHMENT_POINT p_point, RenderBufferAttachmentSPtr p_renderBuffer );
		/**
		 *\~english
		 *\brief		Retrieves a buffer attached at given attachment point and index.
		 *\param[in]	p_point	The attachment point.
		 *\param[in]	p_index	The attachment index.
		 *\return		The attachment, \p nullptr if not found.
		 *\~french
		 *\brief		Récupère un tampon attaché au point et à l'index d'attache donnés.
		 *\param[in]	p_point	Le point d'attache.
		 *\param[in]	p_index	L'index d'attache.
		 *\return		L'attache, \p nullptr si non trouvé.
		 */
		C3D_API FrameBufferAttachmentSPtr GetAttachment( eATTACHMENT_POINT p_point, uint8_t p_index );
		/**
		 *\~english
		 *\brief		Detaches all attached objects
		 *\~french
		 *\brief		Détache tous les objets attachés
		 */
		C3D_API void DetachAll();
		/**
		 *\~english
		 *\brief		Resizes each attached buffer
		 *\param[in]	p_size	The new dimensions
		 *\~french
		 *\brief		Redimensionne tous les tampons attachés
		 *\param[in]	p_size	Les nouvelles dimensions
		 */
		C3D_API void Resize( Castor::Size const & p_size );
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
		 *\brief		Uses given attachments to this framebuffer for next draw call.
		 *\param[in]	p_attaches	The attachments.
		 *\~french
		 *\brief		Utilise les attaches données pour ce framebuffer, lors du prochain dessin.
		 *\param[in]	p_attaches	Les attaches.
		 */
		C3D_API virtual void SetDrawBuffers( AttachArray const & p_attaches ) = 0;
		/**
		 *\~english
		 *\brief		Specifies the color buffer source for pixels
		 *\param[in]	p_point	The color buffer
		 *\param[in]	p_index	The attachment index
		 *\~french
		 *\brief		Définit le tampon de couleur source pour la lecture de pixels
		 *\param[in]	p_point	Le tampon de couleur
		 *\param[in]	p_index	L'index d'attache
		 */
		C3D_API virtual void SetReadBuffer( eATTACHMENT_POINT p_point, uint8_t p_index ) = 0;
		/**
		 *\~english
		 *\brief		Creates a colour render buffer
		 *\param[in]	p_format	The buffer's pixel format
		 *\~french
		 *\brief		Crée un tampon de rendu couleur
		 *\param[in]	p_format	Le fromat de pixels du tampon
		 */
		C3D_API virtual ColourRenderBufferSPtr CreateColourRenderBuffer( Castor::ePIXEL_FORMAT p_format ) = 0;
		/**
		 *\~english
		 *\brief		Creates a depth/stencil render buffer
		 *\param[in]	p_format	The buffer's pixel format
		 *\~french
		 *\brief		Crée un tampon de rendu profondeur/stencil
		 *\param[in]	p_format	Le fromat de pixels du tampon
		 */
		C3D_API virtual DepthStencilRenderBufferSPtr CreateDepthStencilRenderBuffer( Castor::ePIXEL_FORMAT p_format ) = 0;
		/**
		 *\~english
		 *\brief		Creates an attachment to a render buffer
		 *\param[in]	p_renderBuffer	The render buffer
		 *\return		The created attachment
		 *\~french
		 *\brief		Crée une attache à un tampon de rendu
		 *\param[in]	p_renderBuffer	Le tampon de rendu
		 *\return		L'attache créée
		 */
		C3D_API virtual RenderBufferAttachmentSPtr CreateAttachment( RenderBufferSPtr p_renderBuffer ) = 0;
		/**
		 *\~english
		 *\brief		Creates an attachment to a texture
		 *\param[in]	p_texture	The texture
		 *\return		The created attachment
		 *\~french
		 *\brief		Crée une attache à une texture
		 *\param[in]	p_texture	La texture
		 *\return		L'attache créée
		 */
		C3D_API virtual TextureAttachmentSPtr CreateAttachment( TextureLayoutSPtr p_texture ) = 0;
		/**
		 *\~english
		 *\brief		Checks if the FBO is complete
		 *\return		\p false if the buffer is in error if there is an attachment missing
		 *\~french
		 *\brief		Vérifies i le FBO est complet
		 *\return		\p false si le tampon est en erreur ou s'il manque une attache.
		 */
		C3D_API virtual bool IsComplete()const = 0;
		/**
		 *\~english
		 *\brief		Downloads the render buffer data.
		 *\param[in]	p_point		The attachment point.
		 *\param[in]	p_index		The attachment index.
		 *\param[in]	p_buffer	Receives the data.
		 *\~french
		 *\brief		Récupère les données du tampon de rendu.
		 *\param[in]	p_point		Le point d'attache.
		 *\param[in]	p_index		L'index d'attache.
		 *\param[in]	p_buffer	Reçoit les données.
		 */
		C3D_API virtual bool DownloadBuffer( Castor3D::eATTACHMENT_POINT p_point, uint8_t p_index, Castor::PxBufferBaseSPtr p_buffer ) = 0;
		/**
		 *\~english
		 *\brief		Retrieves the background colour
		 *\~french
		 *\brief		Récupère la couleur de fond
		 */
		inline Castor::Colour GetClearColour()const
		{
			return m_clearColour;
		}

	protected:
		/**
		 *\~english
		 *\param[in]	p_point	The attachment point.
		 *\param[in]	p_index			The attachment index.
		 *\return		The pixel format for attachment.
		 *\~french
		 *\param[in]	p_point	Le point d'attache.
		 *\param[in]	p_index			L'index d'attache.
		 *\return		Le format des pixels pour l'attache.
		 */
		C3D_API Castor::ePIXEL_FORMAT DoGetPixelFormat( eATTACHMENT_POINT p_point, uint8_t p_index );
		/**
		 *\~english
		 *\brief		Clears the given buffers components.
		 *\param[in]	p_targets	The buffer components, combination of eBUFFER_COMPONENT.
		 *\~french
		 *\brief		Vide les composantes de tampon données.
		 *\param[in]	p_targets	Les composantes, combinaison de eBUFFER_COMPONENT.
		 */
		C3D_API virtual void DoClear( uint32_t p_targets ) = 0;
		/**
		 *\~english
		 *\brief		Activation function, to tell the GPU it is active
		 *\param[in]	p_target	The frame buffer binding target
		 *\return		\p true if successful
		 *\~french
		 *\brief		Fonction d'activation, pour dire au GPU qu'il est activé
		 *\param[in]	p_target	La cible d'activation du tampon d'image
		 *\return		\p true si tout s'est bien passé
		 */
		C3D_API virtual bool DoBind( eFRAMEBUFFER_TARGET p_target ) = 0;
		/**
		 *\~english
		 *\brief		Deactivation function, to tell the GPU it is inactive
		 *\~french
		 *\brief		Fonction de désactivation, pour dire au GPU qu'il est désactivé
		 */
		C3D_API virtual void DoUnbind() = 0;
		/**
		 *\~english
		 *\brief		Blit this frame buffer into the given one
		 *\param[in]	p_buffer		The buffer receiving this one
		 *\param[in]	p_rect			The destination rectangle
		 *\param[in]	p_components	Bitwise OR of eBUFFER_COMPONENT indicating which buffers are to be copied
		 *\return		\p true if successful
		 *\~french
		 *\brief		Blitte ce tampon dans celui donné
		 *\param[in]	p_buffer		Le tampon recevant celui-ci
		 *\param[in]	p_rect			Le rectangle destination
		 *\param[in]	p_components	OU logique de eBUFFER_COMPONENT indiquant les buffers à copier
		 *\return		\p true si tout s'est bien passé
		 */
		C3D_API virtual bool DoBlitInto( FrameBufferSPtr p_buffer, Castor::Rectangle const & p_rect, uint32_t p_components ) = 0;
		/**
		 *\~english
		 *\brief		Blit this frame buffer into the given one.
		 *\remarks		Interpolation for depth or stencil buffer must be eINTERPOLATION_MODE_NEAREST.
		 *\param[in]	p_buffer		The buffer receiving this one.
		 *\param[in]	p_rectSrc		The source rectangle.
		 *\param[in]	p_rectDst		The destination rectangle.
		 *\param[in]	p_components	Bitwise OR of eBUFFER_COMPONENT indicating which buffers are to be copied.
		 *\param[in]	p_interpolation	The interpolation to apply if the image is stretched.
		 *\return		\p true if successful.
		 *\~french
		 *\brief		Blitte ce tampon dans celui donné.
		 *\remarks		L'interpolation pour un tampon stencil ou profondeur doit être eINTERPOLATION_MODE_NEAREST.
		 *\param[in]	p_buffer		Le tampon recevant celui-ci.
		 *\param[in]	p_rectSrc		Le rectangle source.
		 *\param[in]	p_rectDst		Le rectangle destination.
		 *\param[in]	p_components	OU logique de eBUFFER_COMPONENT indiquant les buffers à copier.
		 *\param[in]	p_interpolation	L'interpolation à appliquer si l'image est redimensionnée.
		 *\return		\p true si tout s'est bien passé.
		 */
		C3D_API virtual bool DoStretchInto( FrameBufferSPtr p_buffer, Castor::Rectangle const & p_rectSrc, Castor::Rectangle const & p_rectDst, uint32_t p_components, eINTERPOLATION_MODE p_interpolation ) = 0;

	private:
		C3D_API bool DoAttach( eATTACHMENT_POINT p_point, uint8_t p_index, FrameBufferAttachmentSPtr p_attach );
		C3D_API void DoDetach( eATTACHMENT_POINT p_point, uint8_t p_index );

	protected:
		//!\~english All attachments.	\~french Toutes les attaches.
		AttachArray m_attaches;
		//!\~english The background colour	\~french La couleur de fond
		Castor::Colour m_clearColour;
	};
}

#endif
