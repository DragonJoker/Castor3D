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
#ifndef ___C3D_FRAME_BUFFER_H___
#define ___C3D_FRAME_BUFFER_H___

#include "Castor3DPrerequisites.hpp"
#include <Rectangle.hpp>

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
	class C3D_API FrameBuffer
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\para[in]		p_pEngine	The core engine
		 *\~french
		 *\brief		Constructeur
		 *\para[in]		p_pEngine	Le moteur
		 */
		FrameBuffer( Engine * p_pEngine );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~FrameBuffer();
		/**
		 *\~english
		 *\brief		Creation function
		 *\param[in]	p_iSamplesCount	Samples count, if the frame buffer must support multisampling
		 *\return		\p true if OK
		 *\~french
		 *\brief		Fonction de création
		 *\param[in]	p_iSamplesCount	Nombre de samples, si le tampon d'image doit supporter le multisampling
		 *\return		\p true si tout s'est bien passé
		 */
		virtual bool Create( int p_iSamplesCount ) = 0;
		/**
		 *\~english
		 *\brief		Destruction function
		 *\~french
		 *\brief		Fonction de destruction
		 */
		virtual void Destroy() = 0;
		/**
		 *\~english
		 *\brief		Activation function, to tell the GPU it is active
		 *\param[in]	p_eTarget	The frame buffer binding target
		 *\param[in]	p_eMode		The frame buffer binding mode
		 *\return		\p true if successful
		 *\~french
		 *\brief		Fonction d'activation, pour dire au GPU qu'il est activé
		 *\param[in]	p_eTarget	La cible d'activation du tampon d'image
		 *\param[in]	p_eMode		Le mode d'activation du tampon d'image
		 *\return		\p true si tout s'est bien passé
		 */
		bool Bind( eFRAMEBUFFER_MODE p_eMode = eFRAMEBUFFER_MODE_AUTOMATIC, eFRAMEBUFFER_TARGET p_eTarget = eFRAMEBUFFER_TARGET_BOTH );
		/**
		 *\~english
		 *\brief		Deactivation function, to tell the GPU it is inactive
		 *\~french
		 *\brief		Fonction de désactivation, pour dire au GPU qu'il est désactivé
		 */
		void Unbind();
		/**
		 *\~english
		 *\brief		Blit this frame buffer into the given one
		 *\param[in]	p_pBuffer			The buffer receiving this one
		 *\param[in]	p_rectSrcDst		The rectangle
		 *\param[in]	p_uiComponents		Bitwise OR of eBUFFER_COMPONENT indicating which buffers are to be copied
		 *\return		\p true if successful
		 *\~french
		 *\brief		Blitte ce tampon dans celui donné
		 *\param[in]	p_pBuffer			Le tampon recevant celui-ci
		 *\param[in]	p_rectSrcDst		Le rectangle
		 *\param[in]	p_uiComponents		OU logique de eBUFFER_COMPONENT indiquant les buffers à copier
		 *\return		\p true si tout s'est bien passé
		 */
		bool BlitInto( FrameBufferSPtr p_pBuffer, Castor::Rectangle const & p_rectSrcDst, uint32_t p_uiComponents );
		/**
		 *\~english
		 *\brief		Blit this frame buffer into the given one
		 *\remark		Interpolation for depth or stencil buffer must be eINTERPOLATION_MODE_NEAREST
		 *\param[in]	p_pBuffer			The buffer receiving this one
		 *\param[in]	p_rectSrc			The source rectangle
		 *\param[in]	p_rectDst			The destination rectangle
		 *\param[in]	p_uiComponents		Bitwise OR of eBUFFER_COMPONENT indicating which buffers are to be copied
		 *\param[in]	p_eInterpolation	The interpolation to apply if the image is stretched
		 *\return		\p true if successful
		 *\~french
		 *\brief		Blitte ce tampon dans celui donné
		 *\remark		L'interpolation pour un tampon stencil ou profondeur doit être eINTERPOLATION_MODE_NEAREST
		 *\param[in]	p_pBuffer			Le tampon recevant celui-ci
		 *\param[in]	p_rectSrc			Le rectangle source
		 *\param[in]	p_rectDst			Le rectangle destination
		 *\param[in]	p_uiComponents		OU logique de eBUFFER_COMPONENT indiquant les buffers à copier
		 *\param[in]	p_eInterpolation	L'interpolation à appliquer si l'image est redimensionnée
		 *\return		\p true si tout s'est bien passé
		 */
		bool StretchInto( FrameBufferSPtr p_pBuffer, Castor::Rectangle const & p_rectSrc, Castor::Rectangle const & p_rectDst, uint32_t p_uiComponents, eINTERPOLATION_MODE p_eInterpolation );
		/**
		 *\~english
		 *\brief		Renders this buffer into another buffer's selected components
		 *\param[in]	p_pBuffer		The buffer receiving the render
		 *\param[in]	p_sizeDst		The destination dimensions
		 *\param[in]	p_uiComponents	Bitwise OR of eBUFFER_COMPONENT indicating the selected destination buffer's components
		 *\~french
		 *\brief		Rend ce tampon dans les composantes choisies d'un autre tampon
		 *\param[in]	p_pBuffer		Le tampon recevant le rendu
		 *\param[in]	p_sizeDst		Les dimensions de la destination
		 *\param[in]	p_uiComponents	OU logique de eBUFFER_COMPONENT indiquant les composantes du tampon destination
		 */
		virtual void RenderToBuffer( FrameBufferSPtr p_pBuffer, Castor::Size const & p_sizeDst, uint32_t p_uiComponents, DepthStencilStateSPtr p_pDepthStencilState, RasteriserStateSPtr p_pRasteriserState );
		/**
		 *\~english
		 *\brief		Specifies the color buffer to be drawn into
		 *\param[in]	p_eAttach	The color buffer
		 *\return		\p true if successful
		 *\~french
		 *\brief		Définit le tampon de couleur dans lequel le dessin doit être effectué
		 *\param[in]	p_eAttach	Le tampon de couleur
		 *\return		\p true si tout s'est bien passé
		 */
		bool SetDrawBuffer( eATTACHMENT_POINT p_eAttach )
		{
			return SetDrawBuffers( 1, &p_eAttach );
		}
		/**
		 *\~english
		 *\brief		Specifies the color buffers to be drawn into
		 *\param[in]	p_uiSize	The buffers count
		 *\param[in]	p_eAttach	The color buffers
		 *\return		\p true if successful
		 *\~french
		 *\brief		Définit les buffers de couleur dans lesquel le dessin doit être effectué
		 *\param[in]	p_uiSize	Le nombre de buffers
		 *\param[in]	p_eAttach	Les buffers de couleur
		 *\return		\p true si tout s'est bien passé
		 */
		template< uint32_t N > bool SetDrawBuffers( eATTACHMENT_POINT p_eAttach[N] )
		{
			return SetDrawBuffers( N, p_eAttach );
		}
		/**
		 *\~english
		 *\brief		Specifies the color buffers to be drawn into
		 *\param[in]	p_arrayAttaches	The color buffers
		 *\return		\p true if successful
		 *\~french
		 *\brief		Définit les buffers de couleur dans lesquel le dessin doit être effectué
		 *\param[in]	p_arrayAttaches	Les buffers de couleur
		 *\return		\p true si tout s'est bien passé
		 */
		inline bool SetDrawBuffers( std::vector< eATTACHMENT_POINT > const & p_arrayAttaches )
		{
			return SetDrawBuffers( uint32_t( p_arrayAttaches.size() ), &p_arrayAttaches[0] );
		}
		/**
		 *\~english
		 *\brief		Specifies the color buffers to be drawn into
		 *\param[in]	p_uiSize	The buffers count
		 *\param[in]	p_eAttach	The color buffers
		 *\return		\p true if successful
		 *\~french
		 *\brief		Définit les buffers de couleur dans lesquel le dessin doit être effectué
		 *\param[in]	p_uiSize	Le nombre de buffers
		 *\param[in]	p_eAttach	Les buffers de couleur
		 *\return		\p true si tout s'est bien passé
		 */
		virtual bool SetDrawBuffers( uint32_t p_uiSize, eATTACHMENT_POINT const * p_eAttach ) = 0;
		/**
		 *\~english
		 *\brief		Specifies the buffers to be drawn into
		 *\remark		All buffers attached are selected
		 *\return		\p true if successful
		 *\~french
		 *\brief		Définit les buffers dans lesquels le dessin doit être effectué
		 *\remark		Tous les buffers attachés sont sélectionnés
		 *\return		\p true si tout s'est bien passé
		 */
		virtual bool SetDrawBuffers() = 0;
		/**
		 *\~english
		 *\brief		Specifies the color buffer source for pixels
		 *\param[in]	p_eAttach	The color buffer
		 *\return		\p true if successful
		 *\~french
		 *\brief		Définit le tampon de couleur source pour la lecture de pixels
		 *\param[in]	p_eAttach	Le tampon de couleur
		 *\return		\p true si tout s'est bien passé
		 */
		virtual bool SetReadBuffer( eATTACHMENT_POINT p_eAttach ) = 0;
		/**
		 *\~english
		 *\brief		Creates a colour render buffer
		 *\param[in]	p_ePixelFormat	The buffer's pixel format
		 *\~french
		 *\brief		Crée un tampon de rendu couleur
		 *\param[in]	p_ePixelFormat	Le fromat de pixels du tampon
		 */
		virtual ColourRenderBufferSPtr CreateColourRenderBuffer( Castor::ePIXEL_FORMAT p_ePixelFormat ) = 0;
		/**
		 *\~english
		 *\brief		Creates a depth/stencil render buffer
		 *\param[in]	p_ePixelFormat	The buffer's pixel format
		 *\~french
		 *\brief		Crée un tampon de rendu profondeur/stencil
		 *\param[in]	p_ePixelFormat	Le fromat de pixels du tampon
		 */
		virtual DepthStencilRenderBufferSPtr CreateDepthStencilRenderBuffer( Castor::ePIXEL_FORMAT p_ePixelFormat ) = 0;
		/**
		 *\~english
		 *\brief		Attaches a render buffer to this frame buffer
		 *\param[in]	p_pAttach	The attachment
		 *\~french
		 *\brief		Attache un tampon de rendu à ce tampon d'image
		 *\param[in]	p_pAttach	L'attache
		 */
		void Attach( RenderBufferAttachmentRPtr p_pAttach );
		/**
		 *\~english
		 *\brief		Detaches a render buffer from this frame buffer
		 *\param[in]	p_pAttach	The attachment
		 *\~french
		 *\brief		Détache un tampon de rendu de ce tampon d'image
		 *\param[in]	p_pAttach	L'attache
		 */
		void Detach( RenderBufferAttachmentRPtr p_pAttach );
		/**
		 *\~english
		 *\brief		Attaches a texture to this frame buffer
		 *\param[in]	p_pAttach	The attachment
		 *\~french
		 *\brief		Attache une texture à ce tampon d'image
		 *\param[in]	p_pAttach	L'attache
		 */
		void Attach( TextureAttachmentRPtr p_pAttach );
		/**
		 *\~english
		 *\brief		Detaches a texture from this frame buffer
		 *\param[in]	p_pAttach	The attachment
		 *\~french
		 *\brief		Détache une texture de ce tampon d'image
		 *\param[in]	p_pAttach	L'attache
		 */
		void Detach( TextureAttachmentRPtr p_pAttach );
		/**
		 *\~english
		 *\brief		Attaches a texture to this frame buffer, at given attachment point
		 *\param[in]	p_eAttachment	The attachment point
		 *\param[in]	p_pTexture		The texture
		 *\param[in]	p_eTarget		The dimension to which the texture must be attached
		 *\param[in]	p_iLayer		The associated layer, if p_eDimension equal eTEXTURE_TARGET_3D or eTEXTURE_TARGET_LAYER
		 *\return		\p true if OK
		 *\~french
		 *\brief		Attache une texture à ce tampon d'image, au point d'attache voulu
		 *\param[in]	p_eAttachment	Le point d'attache
		 *\param[in]	p_pTexture		La texture
		 *\param[in]	p_eTarget		La dimension à laquelle la texture doit être attachée
		 *\param[in]	p_iLayer		La couche associée, si p_eDimension vaut eTEXTURE_TARGET_3D or eTEXTURE_TARGET_LAYER
		 *\return		\p true si tout s'est bien passé
		 */
		bool Attach( eATTACHMENT_POINT p_eAttachment, DynamicTextureSPtr p_pTexture, eTEXTURE_TARGET p_eTarget, int p_iLayer = 0 );
		/**
		 *\~english
		 *\brief		Attaches a render buffer to this frame buffer, at given attachment point
		 *\param[in]	p_eAttachment	The attachment point
		 *\param[in]	p_pRenderBuffer	The render buffer
		 *\return		\p true if OK
		 *\~french
		 *\brief		Attache un tampon de rendu à ce tampon d'image, au point d'attache voulu
		 *\param[in]	p_eAttachment	Le point d'attache
		 *\param[in]	p_pRenderBuffer	Le tampon de rendu
		 *\return		\p true si tout s'est bien passé
		 */
		bool Attach( eATTACHMENT_POINT p_eAttachment, RenderBufferSPtr p_pRenderBuffer );
		/**
		 *\~english
		 *\brief		Detaches all attached objects
		 *\~french
		 *\brief		Détache tous les objets attachés
		 */
		void DetachAll();
		/**
		 *\~english
		 *\brief		Resizes each attached buffer
		 *\param[in]	p_size	The new dimensions
		 *\~french
		 *\brief		Redimensionne tous les tampons attachés
		 *\param[in]	p_size	Les nouvelles dimensions
		 */
		virtual void Resize( Castor::Size const & p_size );
		/**
		 *\~english
		 *\brief		Checks if the FBO is complete
		 *\return		\p false if the buffer is in error if there is an attachment missing
		 *\~french
		 *\brief		Vérifies i le FBO est complet
		 *\return		\p false si le tampon est en erreur ou s'il manque une attache.
		 */
		virtual bool IsComplete() = 0;

	protected:
		/**
		 *\~english
		 *\brief		Activation function, to tell the GPU it is active
		 *\param[in]	p_eTarget	The frame buffer binding target
		 *\return		\p true if successful
		 *\~french
		 *\brief		Fonction d'activation, pour dire au GPU qu'il est activé
		 *\param[in]	p_eTarget	La cible d'activation du tampon d'image
		 *\return		\p true si tout s'est bien passé
		 */
		virtual bool DoBind( eFRAMEBUFFER_TARGET p_eTarget ) = 0;
		/**
		 *\~english
		 *\brief		Deactivation function, to tell the GPU it is inactive
		 *\~french
		 *\brief		Fonction de désactivation, pour dire au GPU qu'il est désactivé
		 */
		virtual void DoUnbind() = 0;
		/**
		 *\~english
		 *\brief		Attaches a render buffer to this frame buffer
		 *\param[in]	p_pAttach	The attachment
		 *\~french
		 *\brief		Attache un tampon de rendu à ce tampon d'image
		 *\param[in]	p_pAttach	L'attache
		 */
		virtual void DoAttach( RenderBufferAttachmentRPtr p_pAttach ) = 0;
		/**
		 *\~english
		 *\brief		Detaches a render buffer from this frame buffer
		 *\param[in]	p_pAttach	The attachment
		 *\~french
		 *\brief		Détache un tampon de rendu de ce tampon d'image
		 *\param[in]	p_pAttach	L'attache
		 */
		virtual void DoDetach( RenderBufferAttachmentRPtr p_pAttach ) = 0;
		/**
		 *\~english
		 *\brief		Attaches a texture to this frame buffer
		 *\param[in]	p_pAttach	The attachment
		 *\~french
		 *\brief		Attache un texture à ce tampon d'image
		 *\param[in]	p_pAttach	L'attache
		 */
		virtual void DoAttach( TextureAttachmentRPtr p_pAttach ) = 0;
		/**
		 *\~english
		 *\brief		Detaches a texture from this frame buffer
		 *\param[in]	p_pAttach	The attachment
		 *\~french
		 *\brief		Détache une texture de ce tampon d'image
		 *\param[in]	p_pAttach	L'attache
		 */
		virtual void DoDetach( TextureAttachmentRPtr p_pAttach ) = 0;
		/**
		 *\~english
		 *\brief		Attaches a texture to this frame buffer, at given attachment point
		 *\param[in]	p_eAttachment	The attachment point
		 *\param[in]	p_pTexture		The texture
		 *\param[in]	p_eTarget		The dimension to which the texture must be attached
		 *\param[in]	p_iLayer		The associated layer, if p_eDimension equal eTEXTURE_TARGET_3D or eTEXTURE_TARGET_LAYER
		 *\return		\p true if OK
		 *\~french
		 *\brief		Attache une texture à ce tampon d'image, au point d'attache voulu
		 *\param[in]	p_eAttachment	Le point d'attache
		 *\param[in]	p_pTexture		La texture
		 *\param[in]	p_eTarget		La dimension à laquelle la texture doit être attachée
		 *\param[in]	p_iLayer		La couche associée, si p_eDimension vaut eTEXTURE_TARGET_3D or eTEXTURE_TARGET_LAYER
		 *\return		\p true si tout s'est bien passé
		 */
		virtual bool DoAttach( eATTACHMENT_POINT p_eAttachment, DynamicTextureSPtr p_pTexture, eTEXTURE_TARGET p_eTarget, int p_iLayer = 0 ) = 0;
		/**
		 *\~english
		 *\brief		Attaches a render buffer to this frame buffer, at given attachment point
		 *\param[in]	p_eAttachment	The attachment point
		 *\param[in]	p_pRenderBuffer	The tampon de rendu
		 *\return		\p true if OK
		 *\~french
		 *\brief		Attache un tampon de rendu à ce tampon d'image, au point d'attache voulu
		 *\param[in]	p_eAttachment	Le point d'attache
		 *\param[in]	p_pRenderBuffer	Le tampon de rendu
		 *\return		\p true si tout s'est bien passé
		 */
		virtual bool DoAttach( eATTACHMENT_POINT p_eAttachment, RenderBufferSPtr p_pRenderBuffer ) = 0;
		/**
		 *\~english
		 *\brief		Detaches all attached objects
		 *\~french
		 *\brief		Détache tous les objets attachés
		 */
		virtual void DoDetachAll() = 0;
		/**
		 *\~english
		 *\brief		Blit this frame buffer into the given one
		 *\remark		Interpolation for depth or stencil buffer must be eINTERPOLATION_MODE_NEAREST
		 *\param[in]	p_pBuffer			The buffer receiving this one
		 *\param[in]	p_rectSrc			The source rectangle
		 *\param[in]	p_rectDst			The destination rectangle
		 *\param[in]	p_uiComponents		Bitwise OR of eBUFFER_COMPONENT indicating which buffers are to be copied
		 *\param[in]	p_eInterpolation	The interpolation to apply if the image is stretched
		 *\return		\p true if successful
		 *\~french
		 *\brief		Blitte ce tampon dans celui donné
		 *\remark		L'interpolation pour un tampon stencil ou profondeur doit être eINTERPOLATION_MODE_NEAREST
		 *\param[in]	p_pBuffer			Le tampon recevant celui-ci
		 *\param[in]	p_rectSrc			Le rectangle source
		 *\param[in]	p_rectDst			Le rectangle destination
		 *\param[in]	p_uiComponents		OU logique de eBUFFER_COMPONENT indiquant les buffers à copier
		 *\param[in]	p_eInterpolation	L'interpolation à appliquer si l'image est redimensionnée
		 *\return		\p true si tout s'est bien passé
		 */
		virtual bool DoStretchInto( FrameBufferSPtr p_pBuffer, Castor::Rectangle const & p_rectSrc, Castor::Rectangle const & p_rectDst, uint32_t p_uiComponents, eINTERPOLATION_MODE p_eInterpolation ) = 0;

	private:
		void DoTexAttach( eATTACHMENT_POINT p_eAttach, DynamicTextureSPtr p_pTexture );
		void DoRboAttach( eATTACHMENT_POINT p_eAttach, RenderBufferSPtr p_pRenderBuffer );
		void DoDetach( eATTACHMENT_POINT p_eAttach );

	protected:
		DECLARE_MAP( eATTACHMENT_POINT,	RenderBufferSPtr, RboAttach );
		DECLARE_MAP( eATTACHMENT_POINT,	DynamicTextureSPtr, TexAttach );
		DECLARE_VECTOR( eATTACHMENT_POINT, Attach );
		//!\~english Attached render buffers map	\~french Map des tampons de rendu attachés
		RboAttachMap m_mapRbo;
		//!\~english Attached textures map	\~french Map des textures attachées
		TexAttachMap m_mapTex;
		//!\~english The engine	\~french Le moteur
		Engine * m_pEngine;
		//!\~english All attchments	\~french Toutes les attaches
		AttachArray m_arrayAttaches;
	};
}

#endif
