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
#ifndef ___C3D_FRAME_BUFFER_H___
#define ___C3D_FRAME_BUFFER_H___

#include "Castor3DPrerequisites.hpp"

#include <Graphics/Colour.hpp>
#include <Design/OwnedBy.hpp>
#include <Graphics/Rectangle.hpp>

#include <memory>

namespace castor3d
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
		: public castor::OwnedBy< Engine >
		, public std::enable_shared_from_this< FrameBuffer >
	{
	public:
		DECLARE_VECTOR( FrameBufferAttachmentSPtr, Attach );
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	engine	The core engine
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	engine	Le moteur
		 */
		C3D_API explicit FrameBuffer( Engine & engine );
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
		C3D_API bool initialise( castor::Size const & p_size );
		/**
		 *\~english
		 *\brief		Cleans up cache buffers.
		 *\~french
		 *\brief		Nettoie les tampons de cache.
		 */
		C3D_API void cleanup();
		/**
		 *\~english
		 *\brief		Defines the colour used when Clear is called on the color buffer.
		 *\param[in]	p_colour	The colour.
		 *\~french
		 *\brief		Définit la couleur utilisée quand Clear est appelée sur le tampon couleur.
		 *\param[in]	p_colour	La couleur.
		 */
		C3D_API void setClearColour( castor::Colour const & p_colour );
		/**
		 *\~english
		 *\brief		Defines the colour used when Clear is called on the color buffer.
		 *\remarks		This version accepts non normalised components.
		 *\param[in]	p_r, p_g, p_b, p_a	The colour components.
		 *\~french
		 *\brief		Définit la couleur utilisée quand Clear est appelée sur le tampon couleur.
		 *\remarks		Cette version accepte des composantes non normalisées.
		 *\param[in]	p_r, p_g, p_b, p_a	Les composantes de la couleur.
		 */
		C3D_API void setClearColour( float p_r, float p_g, float p_b, float p_a );
		/**
		 *\~english
		 *\brief		Clears the buffers.
		 *\param[in]	p_targets	The buffers to clear (combination of BufferComponent).
		 *\~french
		 *\brief		Vide les tampons.
		 *\param[in]	p_targets	Les tampons à vider (combinaison de BufferComponent).
		 */
		C3D_API void clear( BufferComponents p_targets );
		/**
		 *\~english
		 *\brief		Activation function, to tell the GPU it is active
		 *\param[in]	p_target	The frame buffer binding target
		 *\~french
		 *\brief		Fonction d'activation, pour dire au GPU qu'il est activé
		 *\param[in]	p_target	La cible d'activation du tampon d'image
		 */
		C3D_API void bind( FrameBufferTarget p_target = FrameBufferTarget::eBoth )const;
		/**
		 *\~english
		 *\brief		Deactivation function, to tell the GPU it is inactive
		 *\~french
		 *\brief		Fonction de désactivation, pour dire au GPU qu'il est désactivé
		 */
		C3D_API void unbind()const;
		/**
		 *\~english
		 *\brief		Blit this frame buffer into the given one
		 *\param[in]	p_buffer		The buffer receiving this one
		 *\param[in]	p_rect			The rectangle
		 *\param[in]	p_components	Bitwise OR of BufferComponent indicating which buffers are to be copied
		 *\~french
		 *\brief		Blitte ce tampon dans celui donné
		 *\param[in]	p_buffer		Le tampon recevant celui-ci
		 *\param[in]	p_rect			Le rectangle
		 *\param[in]	p_components	OU logique de BufferComponent indiquant les buffers à copier
		 */
		C3D_API void blitInto( FrameBuffer const & p_buffer, castor::Rectangle const & p_rect, castor::FlagCombination< BufferComponent > const & p_components )const;
		/**
		 *\~english
		 *\brief		Stretches this frame buffer into the given one.
		 *\remarks		Interpolation for depth or stencil buffer must be InterpolationMode::eNearest.
		 *\param[in]	p_buffer		The buffer receiving this one.
		 *\param[in]	p_rectSrc		The source rectangle.
		 *\param[in]	p_rectDst		The destination rectangle.
		 *\param[in]	p_components	Bitwise OR of BufferComponent indicating which buffers are to be copied.
		 *\param[in]	p_interpolation	The interpolation to apply if the image is stretched.
		 *\~french
		 *\brief		Stretche ce tampon dans celui donné.
		 *\param[in]	p_buffer		Le tampon recevant celui-ci.
		 *\param[in]	p_rectSrc		Le rectangle source.
		 *\param[in]	p_rectDst		Le rectangle destination.
		 *\param[in]	p_components	OU logique de BufferComponent indiquant les buffers à copier.
		 *\param[in]	p_interpolation	L'interpolation à appliquer si l'image est redimensionnée.
		 */
		C3D_API void stretchInto( FrameBuffer const & p_buffer, castor::Rectangle const & p_rectSrc, castor::Rectangle const & p_rectDst, castor::FlagCombination< BufferComponent > const & p_components, InterpolationMode p_interpolation )const;
		/**
		 *\~english
		 *\brief		Specifies the buffers to be drawn into
		 *\remarks		All buffers attached are selected
		 *\~french
		 *\brief		Définit les buffers dans lesquels le dessin doit être effectué
		 *\remarks		Tous les buffers attachés sont sélectionnés
		 */
		C3D_API void setDrawBuffers()const;
		/**
		 *\~english
		 *\brief		Specifies the buffer to be drawn into.
		 *\param[in]	p_attach	The buffer.
		 *\~french
		 *\brief		Définit le tampon de dans lequel le dessin doit être effectué.
		 *\param[in]	p_attach	Le tampon.
		 */
		C3D_API void setDrawBuffer( FrameBufferAttachmentSPtr p_attach )const;
		/**
		 *\~english
		 *\brief		Attaches a texture to this frame buffer, at given attachment point
		 *\param[in]	p_point		The attachment point
		 *\param[in]	p_index		The attachment index
		 *\param[in]	p_texture	The texture
		 *\param[in]	p_target	The dimension to which the texture must be attached
		 *\param[in]	p_layer		The associated layer, if p_dimension equal TextureType::eThreeDimensions or TextureType::eTwoDimensionsArray
		 *\~french
		 *\brief		Attache une texture à ce tampon d'image, au point d'attache voulu
		 *\param[in]	p_point		Le point d'attache
		 *\param[in]	p_index		L'index d'attache
		 *\param[in]	p_texture	La texture
		 *\param[in]	p_target	La dimension à laquelle la texture doit être attachée
		 *\param[in]	p_layer		La couche associée, si p_dimension vaut TextureType::eThreeDimensions or TextureType::eTwoDimensionsArray
		 */
		C3D_API void attach( AttachmentPoint p_point, uint8_t p_index, TextureAttachmentSPtr p_texture, TextureType p_target, int p_layer = 0 );
		/**
		 *\~english
		 *\brief		Attaches a texture to this frame buffer, at given attachment point
		 *\param[in]	p_point		The attachment point
		 *\param[in]	p_texture	The texture
		 *\param[in]	p_target	The dimension to which the texture must be attached
		 *\param[in]	p_layer		The associated layer, if p_dimension equal TextureType::eThreeDimensions or TextureType::eTwoDimensionsArray
		 *\~french
		 *\brief		Attache une texture à ce tampon d'image, au point d'attache voulu
		 *\param[in]	p_point		Le point d'attache
		 *\param[in]	p_texture	La texture
		 *\param[in]	p_target	La dimension à laquelle la texture doit être attachée
		 *\param[in]	p_layer		La couche associée, si p_dimension vaut TextureType::eThreeDimensions or TextureType::eTwoDimensionsArray
		 */
		C3D_API void attach( AttachmentPoint p_point, TextureAttachmentSPtr p_texture, TextureType p_target, int p_layer = 0 );
		/**
		 *\~english
		 *\brief		Attaches a render buffer to this frame buffer, at given attachment point
		 *\param[in]	p_point			The attachment point
		 *\param[in]	p_index			The attachment index
		 *\param[in]	p_renderBuffer	The render buffer
		 *\~french
		 *\brief		Attache un tampon de rendu à ce tampon d'image, au point d'attache voulu
		 *\param[in]	p_point			Le point d'attache
		 *\param[in]	p_index			L'index d'attache
		 *\param[in]	p_renderBuffer	Le tampon de rendu
		 */
		C3D_API void attach( AttachmentPoint p_point, uint8_t p_index, RenderBufferAttachmentSPtr p_renderBuffer );
		/**
		 *\~english
		 *\brief		Attaches a render buffer to this frame buffer, at given attachment point
		 *\param[in]	p_point			The attachment point
		 *\param[in]	p_renderBuffer	The render buffer
		 *\~french
		 *\brief		Attache un tampon de rendu à ce tampon d'image, au point d'attache voulu
		 *\param[in]	p_point			Le point d'attache
		 *\param[in]	p_renderBuffer	Le tampon de rendu
		 */
		C3D_API void attach( AttachmentPoint p_point, RenderBufferAttachmentSPtr p_renderBuffer );
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
		C3D_API FrameBufferAttachmentSPtr getAttachment( AttachmentPoint p_point, uint8_t p_index );
		/**
		 *\~english
		 *\brief		Detaches all attached objects
		 *\~french
		 *\brief		Détache tous les objets attachés
		 */
		C3D_API void detachAll();
		/**
		 *\~english
		 *\brief		Resizes each attached buffer
		 *\param[in]	p_size	The new dimensions
		 *\~french
		 *\brief		Redimensionne tous les tampons attachés
		 *\param[in]	p_size	Les nouvelles dimensions
		 */
		C3D_API void resize( castor::Size const & p_size );
		/**
		 *\~english
		 *\brief		Creation function
		 *\return		\p true if OK
		 *\~french
		 *\brief		Fonction de création
		 *\return		\p true si tout s'est bien passé
		 */
		C3D_API virtual bool create() = 0;
		/**
		 *\~english
		 *\brief		Destruction function
		 *\~french
		 *\brief		Fonction de destruction
		 */
		C3D_API virtual void destroy() = 0;
		/**
		 *\~english
		 *\brief		Uses given attachments to this framebuffer for next draw call.
		 *\param[in]	p_attaches	The attachments.
		 *\~french
		 *\brief		Utilise les attaches données pour ce framebuffer, lors du prochain dessin.
		 *\param[in]	p_attaches	Les attaches.
		 */
		C3D_API virtual void setDrawBuffers( AttachArray const & p_attaches )const = 0;
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
		C3D_API virtual void setReadBuffer( AttachmentPoint p_point, uint8_t p_index )const = 0;
		/**
		 *\~english
		 *\brief		Creates a colour render buffer
		 *\param[in]	p_format	The buffer's pixel format
		 *\~french
		 *\brief		Crée un tampon de rendu couleur
		 *\param[in]	p_format	Le fromat de pixels du tampon
		 */
		C3D_API virtual ColourRenderBufferSPtr createColourRenderBuffer( castor::PixelFormat p_format ) = 0;
		/**
		 *\~english
		 *\brief		Creates a depth/stencil render buffer
		 *\param[in]	p_format	The buffer's pixel format
		 *\~french
		 *\brief		Crée un tampon de rendu profondeur/stencil
		 *\param[in]	p_format	Le fromat de pixels du tampon
		 */
		C3D_API virtual DepthStencilRenderBufferSPtr createDepthStencilRenderBuffer( castor::PixelFormat p_format ) = 0;
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
		C3D_API virtual RenderBufferAttachmentSPtr createAttachment( RenderBufferSPtr p_renderBuffer ) = 0;
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
		C3D_API virtual TextureAttachmentSPtr createAttachment( TextureLayoutSPtr p_texture ) = 0;
		/**
		 *\~english
		 *\brief		Creates an attachment to a cube texture's face.
		 *\param[in]	p_texture	The texture.
		 *\param[in]	p_face		The face.
		 *\return		The created attachment
		 *\~french
		 *\brief		Crée une attache à une face d'une texture cube.
		 *\param[in]	p_texture	La texture.
		 *\param[in]	p_face	La face.
		 *\return		L'attache créée
		 */
		C3D_API virtual TextureAttachmentSPtr createAttachment( TextureLayoutSPtr p_texture, CubeMapFace p_face ) = 0;
		/**
		 *\~english
		 *\brief		Creates an attachment to a cube texture's face's mipmap.
		 *\param[in]	p_texture	The texture.
		 *\param[in]	p_face		The face.
		 *\param[in]	p_mipLevel	The mipmap level.
		 *\return		The created attachment
		 *\~french
		 *\brief		Crée une attache à un mipmap d'une face d'une texture cube.
		 *\param[in]	p_texture	La texture.
		 *\param[in]	p_face		La face.
		 *\param[in]	p_mipLevel	Le niveau du mipmap.
		 *\return		L'attache créée
		 */
		C3D_API virtual TextureAttachmentSPtr createAttachment( TextureLayoutSPtr p_texture, CubeMapFace p_face, uint32_t p_mipLevel ) = 0;
		/**
		 *\~english
		 *\brief		Checks if the FBO is complete
		 *\return		\p false if the buffer is in error if there is an attachment missing
		 *\~french
		 *\brief		Vérifies i le FBO est complet
		 *\return		\p false si le tampon est en erreur ou s'il manque une attache.
		 */
		C3D_API virtual bool isComplete()const = 0;
		/**
		 *\~english
		 *\brief		downloads the render buffer data.
		 *\param[in]	p_point		The attachment point.
		 *\param[in]	p_index		The attachment index.
		 *\param[in]	p_buffer	Receives the data.
		 *\~french
		 *\brief		Récupère les données du tampon de rendu.
		 *\param[in]	p_point		Le point d'attache.
		 *\param[in]	p_index		L'index d'attache.
		 *\param[in]	p_buffer	Reçoit les données.
		 */
		C3D_API virtual void downloadBuffer( castor3d::AttachmentPoint p_point, uint8_t p_index, castor::PxBufferBaseSPtr p_buffer ) = 0;

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
		C3D_API castor::PixelFormat doGetPixelFormat( AttachmentPoint p_point, uint8_t p_index );
		/**
		 *\~english
		 *\brief		Clears the given buffers components.
		 *\param[in]	p_targets	The buffer components, combination of BufferComponent.
		 *\~french
		 *\brief		Vide les composantes de tampon données.
		 *\param[in]	p_targets	Les composantes, combinaison de BufferComponent.
		 */
		C3D_API virtual void doClear( BufferComponents p_targets ) = 0;
		/**
		 *\~english
		 *\brief		Activation function, to tell the GPU it is active
		 *\param[in]	p_target	The frame buffer binding target
		 *\~french
		 *\brief		Fonction d'activation, pour dire au GPU qu'il est activé
		 *\param[in]	p_target	La cible d'activation du tampon d'image
		 */
		C3D_API virtual void doBind( FrameBufferTarget p_target )const = 0;
		/**
		 *\~english
		 *\brief		Deactivation function, to tell the GPU it is inactive
		 *\~french
		 *\brief		Fonction de désactivation, pour dire au GPU qu'il est désactivé
		 */
		C3D_API virtual void doUnbind()const = 0;
		/**
		 *\~english
		 *\brief		Blit this frame buffer into the given one
		 *\param[in]	p_buffer		The buffer receiving this one
		 *\param[in]	p_rect			The destination rectangle
		 *\param[in]	p_components	Bitwise OR of BufferComponent indicating which buffers are to be copied
		 *\return		\p true if successful
		 *\~french
		 *\brief		Blitte ce tampon dans celui donné
		 *\param[in]	p_buffer		Le tampon recevant celui-ci
		 *\param[in]	p_rect			Le rectangle destination
		 *\param[in]	p_components	OU logique de BufferComponent indiquant les buffers à copier
		 */
		C3D_API virtual void doBlitInto( FrameBuffer const & p_buffer, castor::Rectangle const & p_rect, castor::FlagCombination< BufferComponent > const & p_components )const = 0;
		/**
		 *\~english
		 *\brief		Blit this frame buffer into the given one.
		 *\remarks		Interpolation for depth or stencil buffer must be InterpolationMode::eNearest.
		 *\param[in]	p_buffer		The buffer receiving this one.
		 *\param[in]	p_rectSrc		The source rectangle.
		 *\param[in]	p_rectDst		The destination rectangle.
		 *\param[in]	p_components	Bitwise OR of BufferComponent indicating which buffers are to be copied.
		 *\param[in]	p_interpolation	The interpolation to apply if the image is stretched.
		 *\return		\p true if successful.
		 *\~french
		 *\brief		Blitte ce tampon dans celui donné.
		 *\remarks		L'interpolation pour un tampon stencil ou profondeur doit être InterpolationMode::eNearest.
		 *\param[in]	p_buffer		Le tampon recevant celui-ci.
		 *\param[in]	p_rectSrc		Le rectangle source.
		 *\param[in]	p_rectDst		Le rectangle destination.
		 *\param[in]	p_components	OU logique de BufferComponent indiquant les buffers à copier.
		 *\param[in]	p_interpolation	L'interpolation à appliquer si l'image est redimensionnée.
		 */
		C3D_API virtual void doStretchInto( FrameBuffer const & p_buffer, castor::Rectangle const & p_rectSrc, castor::Rectangle const & p_rectDst, castor::FlagCombination< BufferComponent > const & p_components, InterpolationMode p_interpolation )const = 0;

	private:
		C3D_API void doAttach( AttachmentPoint p_point, uint8_t p_index, FrameBufferAttachmentSPtr p_attach );
		C3D_API void doDetach( AttachmentPoint p_point, uint8_t p_index );

	protected:
		//!\~english	All attachments.
		//!\~french		Toutes les attaches.
		AttachArray m_attaches;
		//!\~english	The clear colour red PixelComponents.
		//!\~french		La composante rouge de la couleur de vidage.
		float m_redClear{ 0.0f };
		//!\~english	The clear colour green PixelComponents.
		//!\~french		La composante verte de la couleur de vidage.
		float m_greenClear{ 0.0f };
		//!\~english	The clear colour blue PixelComponents.
		//!\~french		La composante bleue de la couleur de vidage.
		float m_blueClear{ 0.0f };
		//!\~english	The clear colour alpha PixelComponents.
		//!\~french		La composante alpha de la couleur de vidage.
		float m_alphaClear{ 1.0f };
	};
}

#endif
