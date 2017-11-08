/*
See LICENSE file in root folder
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
		 *\return		\p true if OK.
		 *\~french
		 *\brief		Initialise les tampons de cache de couleur de profondeur.
		 *\return		\p true si tout s'est bien passé.
		 */
		C3D_API virtual bool initialise() = 0;
		/**
		 *\~english
		 *\brief		Cleans up cache buffers.
		 *\~french
		 *\brief		Nettoie les tampons de cache.
		 */
		C3D_API virtual void cleanup() = 0;
		/**
		 *\~english
		 *\brief		Defines the colour used when Clear is called on the color buffer.
		 *\param[in]	colour	The colour.
		 *\~french
		 *\brief		Définit la couleur utilisée quand Clear est appelée sur le tampon couleur.
		 *\param[in]	colour	La couleur.
		 */
		C3D_API void setClearColour( castor::RgbaColour const & colour );
		/**
		 *\~english
		 *\brief		Defines the colour used when Clear is called on the color buffer.
		 *\remarks		This version accepts non normalised components.
		 *\param[in]	r, g, b, a	The colour components.
		 *\~french
		 *\brief		Définit la couleur utilisée quand Clear est appelée sur le tampon couleur.
		 *\remarks		Cette version accepte des composantes non normalisées.
		 *\param[in]	r, g, b, a	Les composantes de la couleur.
		 */
		C3D_API void setClearColour( float r, float g, float b, float a );
		/**
		 *\~english
		 *\brief		Clears the buffers.
		 *\param[in]	targets	The buffers to clear (combination of BufferComponent).
		 *\~french
		 *\brief		Vide les tampons.
		 *\param[in]	targets	Les tampons à vider (combinaison de BufferComponent).
		 */
		C3D_API void clear( BufferComponents targets )const;
		/**
		 *\~english
		 *\brief		Activation function, to tell the GPU it is active
		 *\param[in]	target	The frame buffer binding target
		 *\~french
		 *\brief		Fonction d'activation, pour dire au GPU qu'il est activé
		 *\param[in]	target	La cible d'activation du tampon d'image
		 */
		C3D_API void bind( FrameBufferTarget target = FrameBufferTarget::eBoth )const;
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
		 *\param[in]	buffer		The buffer receiving this one
		 *\param[in]	rect		The rectangle
		 *\param[in]	components	Bitwise OR of BufferComponent indicating which buffers are to be copied
		 *\~french
		 *\brief		Blitte ce tampon dans celui donné
		 *\param[in]	buffer		Le tampon recevant celui-ci
		 *\param[in]	rect		Le rectangle
		 *\param[in]	components	OU logique de BufferComponent indiquant les buffers à copier
		 */
		C3D_API void blitInto( FrameBuffer const & buffer
			, castor::Rectangle const & rect
			, BufferComponents const & components )const;
		/**
		 *\~english
		 *\brief		Stretches this frame buffer into the given one.
		 *\remarks		Interpolation for depth or stencil buffer must be InterpolationMode::eNearest.
		 *\param[in]	buffer		The buffer receiving this one.
		 *\param[in]	rectSrc	The source rectangle.
		 *\param[in]	rectDst	The destination rectangle.
		 *\param[in]	components	Bitwise OR of BufferComponent indicating which buffers are to be copied.
		 *\param[in]	interpolation	The interpolation to apply if the image is stretched.
		 *\~french
		 *\brief		Stretche ce tampon dans celui donné.
		 *\param[in]	buffer		Le tampon recevant celui-ci.
		 *\param[in]	rectSrc	Le rectangle source.
		 *\param[in]	rectDst	Le rectangle destination.
		 *\param[in]	components	OU logique de BufferComponent indiquant les buffers à copier.
		 *\param[in]	interpolation	L'interpolation à appliquer si l'image est redimensionnée.
		 */
		C3D_API void stretchInto( FrameBuffer const & buffer
			, castor::Rectangle const & rectSrc
			, castor::Rectangle const & rectDst
			, BufferComponents const & components
			, InterpolationMode interpolation )const;
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
		 *\param[in]	attach	The buffer.
		 *\~french
		 *\brief		Définit le tampon de dans lequel le dessin doit être effectué.
		 *\param[in]	attach	Le tampon.
		 */
		C3D_API void setDrawBuffer( FrameBufferAttachmentSPtr attach )const;
		/**
		 *\~english
		 *\brief		Attaches a texture to this frame buffer, at given attachment point
		 *\param[in]	point	The attachment point
		 *\param[in]	index	The attachment index
		 *\param[in]	texture	The texture
		 *\param[in]	target	The dimension to which the texture must be attached
		 *\param[in]	layer	The associated layer, if p_dimension equal TextureType::eThreeDimensions or TextureType::eTwoDimensionsArray
		 *\~french
		 *\brief		Attache une texture à ce tampon d'image, au point d'attache voulu
		 *\param[in]	point	Le point d'attache
		 *\param[in]	index	L'index d'attache
		 *\param[in]	texture	La texture
		 *\param[in]	target	La dimension à laquelle la texture doit être attachée
		 *\param[in]	layer	La couche associée, si p_dimension vaut TextureType::eThreeDimensions or TextureType::eTwoDimensionsArray
		 */
		C3D_API void attach( AttachmentPoint point
			, uint8_t index
			, TextureAttachmentSPtr texture
			, TextureType target
			, int layer = 0 );
		/**
		 *\~english
		 *\brief		Attaches a texture to this frame buffer, at given attachment point
		 *\param[in]	point	The attachment point
		 *\param[in]	texture	The texture
		 *\param[in]	target	The dimension to which the texture must be attached
		 *\param[in]	layer	The associated layer, if p_dimension equal TextureType::eThreeDimensions or TextureType::eTwoDimensionsArray
		 *\~french
		 *\brief		Attache une texture à ce tampon d'image, au point d'attache voulu
		 *\param[in]	point	Le point d'attache
		 *\param[in]	texture	La texture
		 *\param[in]	target	La dimension à laquelle la texture doit être attachée
		 *\param[in]	layer	La couche associée, si p_dimension vaut TextureType::eThreeDimensions or TextureType::eTwoDimensionsArray
		 */
		C3D_API void attach( AttachmentPoint point
			, TextureAttachmentSPtr texture
			, TextureType target
			, int layer = 0 );
		/**
		 *\~english
		 *\brief		Attaches a render buffer to this frame buffer, at given attachment point
		 *\param[in]	point			The attachment point
		 *\param[in]	index			The attachment index
		 *\param[in]	renderBuffer	The render buffer
		 *\~french
		 *\brief		Attache un tampon de rendu à ce tampon d'image, au point d'attache voulu
		 *\param[in]	point			Le point d'attache
		 *\param[in]	index			L'index d'attache
		 *\param[in]	renderBuffer	Le tampon de rendu
		 */
		C3D_API void attach( AttachmentPoint point
			, uint8_t index
			, RenderBufferAttachmentSPtr renderBuffer );
		/**
		 *\~english
		 *\brief		Attaches a render buffer to this frame buffer, at given attachment point
		 *\param[in]	point			The attachment point
		 *\param[in]	renderBuffer	The render buffer
		 *\~french
		 *\brief		Attache un tampon de rendu à ce tampon d'image, au point d'attache voulu
		 *\param[in]	point			Le point d'attache
		 *\param[in]	renderBuffer	Le tampon de rendu
		 */
		C3D_API void attach( AttachmentPoint point
			, RenderBufferAttachmentSPtr renderBuffer );
		/**
		 *\~english
		 *\brief		Detaches an attach from this frame buffer.
		 *\param[in]	attach	The attach.
		 *\~french
		 *\brief		Détache une atache de ce tampon d'image.
		 *\param[in]	attach	L'attache.
		 */
		C3D_API void detach( FrameBufferAttachmentSPtr attach );
		/**
		 *\~english
		 *\brief		Retrieves a buffer attached at given attachment point and index.
		 *\param[in]	point	The attachment point.
		 *\param[in]	index	The attachment index.
		 *\return		The attachment, \p nullptr if not found.
		 *\~french
		 *\brief		Récupère un tampon attaché au point et à l'index d'attache donnés.
		 *\param[in]	point	Le point d'attache.
		 *\param[in]	index	L'index d'attache.
		 *\return		L'attache, \p nullptr si non trouvé.
		 */
		C3D_API FrameBufferAttachmentSPtr getAttachment( AttachmentPoint point
			, uint8_t index );
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
		 *\param[in]	size	The new dimensions
		 *\~french
		 *\brief		Redimensionne tous les tampons attachés
		 *\param[in]	size	Les nouvelles dimensions
		 */
		C3D_API void resize( castor::Size const & size );
		/*
		 *\~english
		 *\brief		Sets the sRGB colour space usage status.
		 *\param[in]	value	The new value.
		 *\~french
		 *\brief		Définit le statut d'utilisation de l'utilisation de l'espace de couleurs sRGB.
		 *\param[in]	value	La nouvelle valeur.
		 */
		inline void setSRGB( bool value )
		{
			m_sRGB = value;
		}
		/*
		 *\~english
		 *\return		\p true if the frame buffer uses sRGB colour space.
		 *\~french
		 *\return		\p true si le tampon d'images utilise l'espace de couleurs sRGB.
		 */
		inline bool isSRGB()const
		{
			return m_sRGB;
		}
		/**
		 *\~english
		 *\brief		Uses given attachments to this framebuffer for next draw call.
		 *\param[in]	attaches	The attachments.
		 *\~french
		 *\brief		Utilise les attaches données pour ce framebuffer, lors du prochain dessin.
		 *\param[in]	attaches	Les attaches.
		 */
		C3D_API virtual void setDrawBuffers( AttachArray const & attaches )const = 0;
		/**
		 *\~english
		 *\brief		Specifies the color buffer source for pixels
		 *\param[in]	point	The color buffer
		 *\param[in]	index	The attachment index
		 *\~french
		 *\brief		Définit le tampon de couleur source pour la lecture de pixels
		 *\param[in]	point	Le tampon de couleur
		 *\param[in]	index	L'index d'attache
		 */
		C3D_API virtual void setReadBuffer( AttachmentPoint point
			, uint8_t index )const = 0;
		/**
		 *\~english
		 *\brief		Creates a colour render buffer
		 *\param[in]	format	The buffer's pixel format
		 *\~french
		 *\brief		Crée un tampon de rendu couleur
		 *\param[in]	format	Le format de pixels du tampon
		 */
		C3D_API virtual ColourRenderBufferSPtr createColourRenderBuffer( castor::PixelFormat format )const = 0;
		/**
		 *\~english
		 *\brief		Creates a depth/stencil render buffer
		 *\param[in]	format	The buffer's pixel format
		 *\~french
		 *\brief		Crée un tampon de rendu profondeur/stencil
		 *\param[in]	format	Le format de pixels du tampon
		 */
		C3D_API virtual DepthStencilRenderBufferSPtr createDepthStencilRenderBuffer( castor::PixelFormat format )const = 0;
		/**
		 *\~english
		 *\brief		Creates an attachment to a render buffer
		 *\param[in]	renderBuffer	The render buffer
		 *\return		The created attachment
		 *\~french
		 *\brief		Crée une attache à un tampon de rendu
		 *\param[in]	renderBuffer	Le tampon de rendu
		 *\return		L'attache créée
		 */
		C3D_API virtual RenderBufferAttachmentSPtr createAttachment( RenderBufferSPtr renderBuffer )const = 0;
		/**
		 *\~english
		 *\brief		Creates an attachment to a texture
		 *\param[in]	texture	The texture
		 *\return		The created attachment
		 *\~french
		 *\brief		Crée une attache à une texture
		 *\param[in]	texture	La texture
		 *\return		L'attache créée
		 */
		C3D_API virtual TextureAttachmentSPtr createAttachment( TextureLayoutSPtr texture )const = 0;
		/**
		 *\~english
		 *\brief		Creates an attachment to a cube texture's face.
		 *\param[in]	texture	The texture.
		 *\param[in]	face	The face.
		 *\return		The created attachment
		 *\~french
		 *\brief		Crée une attache à une face d'une texture cube.
		 *\param[in]	texture	La texture.
		 *\param[in]	face	La face.
		 *\return		L'attache créée
		 */
		C3D_API virtual TextureAttachmentSPtr createAttachment( TextureLayoutSPtr texture
			, CubeMapFace face )const = 0;
		/**
		 *\~english
		 *\brief		Creates an attachment to a cube texture's face's mipmap.
		 *\param[in]	texture		The texture.
		 *\param[in]	face		The face.
		 *\param[in]	p_mipLevel	The mipmap level.
		 *\return		The created attachment
		 *\~french
		 *\brief		Crée une attache à un mipmap d'une face d'une texture cube.
		 *\param[in]	texture		La texture.
		 *\param[in]	face		La face.
		 *\param[in]	p_mipLevel	Le niveau du mipmap.
		 *\return		L'attache créée
		 */
		C3D_API virtual TextureAttachmentSPtr createAttachment( TextureLayoutSPtr texture
			, CubeMapFace face
			, uint32_t p_mipLevel )const = 0;
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
		 *\param[in]	point	The attachment point.
		 *\param[in]	index	The attachment index.
		 *\param[in]	buffer	Receives the data.
		 *\~french
		 *\brief		Récupère les données du tampon de rendu.
		 *\param[in]	point	Le point d'attache.
		 *\param[in]	index	L'index d'attache.
		 *\param[in]	buffer	Reçoit les données.
		 */
		C3D_API virtual void downloadBuffer( castor3d::AttachmentPoint point
			, uint8_t index
			, castor::PxBufferBaseSPtr buffer )const = 0;

	protected:
		/**
		 *\~english
		 *\param[in]	point	The attachment point.
		 *\param[in]	index	The attachment index.
		 *\return		The pixel format for attachment.
		 *\~french
		 *\param[in]	point	Le point d'attache.
		 *\param[in]	index	L'index d'attache.
		 *\return		Le format des pixels pour l'attache.
		 */
		C3D_API castor::PixelFormat doGetPixelFormat( AttachmentPoint point
			, uint8_t index )const;
		/**
		 *\~english
		 *\brief		Clears the given buffers components.
		 *\param[in]	targets	The buffer components, combination of BufferComponent.
		 *\~french
		 *\brief		Vide les composantes de tampon données.
		 *\param[in]	targets	Les composantes, combinaison de BufferComponent.
		 */
		C3D_API virtual void doClear( BufferComponents targets )const = 0;
		/**
		 *\~english
		 *\brief		Activation function, to tell the GPU it is active
		 *\param[in]	target	The frame buffer binding target
		 *\~french
		 *\brief		Fonction d'activation, pour dire au GPU qu'il est activé
		 *\param[in]	target	La cible d'activation du tampon d'image
		 */
		C3D_API virtual void doBind( FrameBufferTarget target )const = 0;
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
		 *\param[in]	buffer		The buffer receiving this one
		 *\param[in]	rect		The destination rectangle
		 *\param[in]	components	Bitwise OR of BufferComponent indicating which buffers are to be copied
		 *\return		\p true if successful
		 *\~french
		 *\brief		Blitte ce tampon dans celui donné
		 *\param[in]	buffer		Le tampon recevant celui-ci
		 *\param[in]	rect		Le rectangle destination
		 *\param[in]	components	OU logique de BufferComponent indiquant les buffers à copier
		 */
		C3D_API virtual void doBlitInto( FrameBuffer const & buffer
			, castor::Rectangle const & rect
			, BufferComponents const & components )const = 0;
		/**
		 *\~english
		 *\brief		Blit this frame buffer into the given one.
		 *\remarks		Interpolation for depth or stencil buffer must be InterpolationMode::eNearest.
		 *\param[in]	buffer			The buffer receiving this one.
		 *\param[in]	rectSrc			The source rectangle.
		 *\param[in]	rectDst			The destination rectangle.
		 *\param[in]	components		Bitwise OR of BufferComponent indicating which buffers are to be copied.
		 *\param[in]	interpolation	The interpolation to apply if the image is stretched.
		 *\return		\p true if successful.
		 *\~french
		 *\brief		Blitte ce tampon dans celui donné.
		 *\remarks		L'interpolation pour un tampon stencil ou profondeur doit être InterpolationMode::eNearest.
		 *\param[in]	buffer			Le tampon recevant celui-ci.
		 *\param[in]	rectSrc			Le rectangle source.
		 *\param[in]	rectDst			Le rectangle destination.
		 *\param[in]	components		OU logique de BufferComponent indiquant les buffers à copier.
		 *\param[in]	interpolation	L'interpolation à appliquer si l'image est redimensionnée.
		 */
		C3D_API virtual void doStretchInto( FrameBuffer const & buffer
			, castor::Rectangle const & rectSrc
			, castor::Rectangle const & rectDst
			, BufferComponents const & components
			, InterpolationMode interpolation )const = 0;

	private:
		C3D_API void doAttach( AttachmentPoint point, uint8_t index, FrameBufferAttachmentSPtr attach );
		C3D_API void doDetach( AttachmentPoint point, uint8_t index );

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
		//!\~english	Tells that the framebuffer uses sRGB colour space.
		//!\~french		Dit que le tampon d'images utilise l'espace de couleurs sRGB.
		bool m_sRGB{ false };
	};
}

#endif
