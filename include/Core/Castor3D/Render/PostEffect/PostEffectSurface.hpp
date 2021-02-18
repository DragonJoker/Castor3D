/*
See LICENSE file in root folder
*/
#ifndef ___C3D_PostEffectSurface_H___
#define ___C3D_PostEffectSurface_H___

#include "PostEffectModule.hpp"

#include <CastorUtils/Graphics/Size.hpp>

#include <ashespp/Image/ImageView.hpp>
#include <ashespp/RenderPass/FrameBuffer.hpp>

namespace castor3d
{
	struct PostEffectSurface
		: public castor::OwnedBy< Engine >
	{
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine		The engine.
		 *\param[in]	debugName	The surface debug name.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine		Le moteur.
		 *\param[in]	debugName	Le nom de debug de la surface.
		 */
		C3D_API explicit PostEffectSurface( Engine & engine
			, castor::String debugName );
		/**
		 *\~english
		 *\brief		Initialises the surface.
		 *\param[in]	device			The GPU device.
		 *\param[in]	renderPass		The render pass from which the frame buffer is created.
		 *\param[in]	size			The surface's size.
		 *\param[in]	format			The surface's colour pixel format.
		 *\param[in]	mipLevels		The surface's colour texture mipmap level.
		 *\~french
		 *\brief		Initialise la surface.
		 *\param[in]	device			Le device GPU.
		 *\param[in]	renderPass		La passe de rendu depuis laquelle le frame buffer est créé.
		 *\param[in]	size			Les dimensions de la surface.
		 *\param[in]	format			Le format des pixels couleur de la surface.
		 *\param[in]	mipLevels		Le nombre de niveausx de mipmap de la texture couleur de la surface.
		 */
		C3D_API bool initialise( RenderDevice const & device
			, ashes::RenderPass const & renderPass
			, castor::Size const & size
			, VkFormat format = VK_FORMAT_R32G32B32A32_SFLOAT
			, uint32_t mipLevels = 1u );
		/**
		 *\~english
		 *\brief		Initialises the surface.
		 *\param[in]	device			The GPU device.
		 *\param[in]	renderPass		The render pass from which the frame buffer is created.
		 *\param[in]	size			The surface's size.
		 *\param[in]	colourFormat	The surface's colour pixel format.
		 *\param[in]	depthFormat		The surface's depth pixel format.
		 *\~french
		 *\brief		Initialise la surface.
		 *\param[in]	device			Le device GPU.
		 *\param[in]	renderPass		La passe de rendu depuis laquelle le frame buffer est créé.
		 *\param[in]	size			Les dimensions de la surface.
		 *\param[in]	colourFormat	Le format des pixels couleur de la surface.
		 *\param[in]	depthFormat		Le format des pixels profondeur de la surface.
		 */
		C3D_API bool initialise( RenderDevice const & device
			, ashes::RenderPass const & renderPass
			, castor::Size const & size
			, VkFormat colourFormat
			, VkFormat depthFormat );
		/**
		 *\~english
		 *\brief		Initialises the surface.
		 *\param[in]	device			The GPU device.
		 *\param[in]	renderPass		The render pass from which the frame buffer is created.
		 *\param[in]	size			The surface's size.
		 *\param[in]	colourTexture	The surface's colour texture.
		 *\~french
		 *\brief		Initialise la surface.
		 *\param[in]	device			Le device GPU.
		 *\param[in]	renderPass		La passe de rendu depuis laquelle le frame buffer est créé.
		 *\param[in]	size			Les dimensions de la surface.
		 *\param[in]	colourTexture	La texture couleur de la surface.
		 */
		C3D_API bool initialise( RenderDevice const & device
			, ashes::RenderPass const & renderPass
			, castor::Size const & size
			, TextureLayoutSPtr colourTexture );
		/**
		 *\~english
		 *\brief		Initialises the surface.
		 *\param[in]	device			The GPU device.
		 *\param[in]	renderPass		The render pass from which the frame buffer is created.
		 *\param[in]	size			The surface's size.
		 *\param[in]	colourTexture	The surface's colour texture.
		 *\param[in]	depthFormat		The surface's depth pixel format.
		 *\~french
		 *\brief		Initialise la surface.
		 *\param[in]	device			Le device GPU.
		 *\param[in]	renderPass		La passe de rendu depuis laquelle le frame buffer est créé.
		 *\param[in]	size			Les dimensions de la surface.
		 *\param[in]	colourTexture	La texture couleur de la surface.
		 *\param[in]	depthFormat		Le format des pixels profondeur de la surface.
		 */
		C3D_API bool initialise( RenderDevice const & device
			, ashes::RenderPass const & renderPass
			, castor::Size const & size
			, TextureLayoutSPtr colourTexture
			, VkFormat depthFormat );
		/**
		 *\~english
		 *\brief		Initialises the surface.
		 *\param[in]	device			The GPU device.
		 *\param[in]	renderPass		The render pass from which the frame buffer is created.
		 *\param[in]	size			The surface's size.
		 *\param[in]	colourFormat	The surface's colour pixel format.
		 *\param[in]	depthTexture	The surface's depth texture.
		 *\~french
		 *\brief		Initialise la surface.
		 *\param[in]	device			Le device GPU.
		 *\param[in]	renderPass		La passe de rendu depuis laquelle le frame buffer est créé.
		 *\param[in]	size			Les dimensions de la surface.
		 *\param[in]	colourFormat	Le format des pixels couleur de la surface.
		 *\param[in]	depthTexture	La texture profondeur de la surface.
		 */
		C3D_API bool initialise( RenderDevice const & device
			, ashes::RenderPass const & renderPass
			, castor::Size const & size
			, VkFormat colourFormat
			, TextureLayoutSPtr depthTexture );
		/**
		 *\~english
		 *\brief		Initialises the surface.
		 *\param[in]	device			The GPU device.
		 *\param[in]	renderPass		The render pass from which the frame buffer is created.
		 *\param[in]	size			The surface's size.
		 *\param[in]	colourTexture	The surface's colour texture.
		 *\param[in]	depthTexture	The surface's depth texture.
		 *\~french
		 *\brief		Initialise la surface.
		 *\param[in]	device			Le device GPU.
		 *\param[in]	renderPass		La passe de rendu depuis laquelle le frame buffer est créé.
		 *\param[in]	size			Les dimensions de la surface.
		 *\param[in]	colourTexture	La texture couleur de la surface.
		 *\param[in]	depthTexture	La texture profondeur de la surface.
		 */
		C3D_API bool initialise( RenderDevice const & device
			, ashes::RenderPass const & renderPass
			, castor::Size const & size
			, TextureLayoutSPtr colourTexture
			, TextureLayoutSPtr depthTexture );
		/**
		 *\~english
		 *\brief		Cleans up the surface.
		 *\param[in]	device	The GPU device.
		 *\~french
		 *\brief		Nettoie la surface.
		 *\param[in]	device	Le device GPU.
		 */
		C3D_API void cleanup( RenderDevice const & device );

		//!\~english	The surface framebuffer.
		//!\~french		Le framebuffer de la surface.
		ashes::FrameBufferPtr frameBuffer;
		//!\~english	The surface colour texture.
		//!\~french		La texture couleur de la surface.
		TextureLayoutSPtr colourTexture;
		//!\~english	The surface depth texture.
		//!\~french		La texture profondeur de la surface.
		TextureLayoutSPtr depthTexture;
		//!\~english	The surface depth view.
		//!\~french		La vue profondeur de la surface.
		ashes::ImageView depthView;
		//!\~english	The surface stencil view.
		//!\~french		La vue stencil de la surface.
		ashes::ImageView stencilView;
		//!\~english	The surface dimensions.
		//!\~french		Les dimensions de la surface.
		castor::Size size;

	private:
		castor::String m_debugName;
	};
}

#endif
