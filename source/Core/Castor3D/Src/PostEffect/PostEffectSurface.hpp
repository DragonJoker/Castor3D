/*
See LICENSE file in root folder
*/
#ifndef ___C3D_PostEffectSurface_H___
#define ___C3D_PostEffectSurface_H___

#include "Castor3DPrerequisites.hpp"

#include <Design/OwnedBy.hpp>

#include <RenderPass/FrameBuffer.hpp>

namespace castor3d
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		03/07/2016
	\~english
	\brief		Post render effect surface structure.
	\remark		Holds basic informations for a possible post effect surface: framebuffer and colour texture.
	\~french
	\brief		Surface pour effet post rendu.
	\remark		Contient les informations basiques de surface d'un effet: framebuffer, texture de couleur.
	*/
	struct PostEffectSurface
		: public castor::OwnedBy< Engine >
	{
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine	The engine.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine	Le moteur.
		 */
		C3D_API explicit PostEffectSurface( Engine & engine );
		/**
		 *\~english
		 *\brief		Initialises the surface.
		 *\param[in]	renderTarget	The render target to which is attached this effect.
		 *\param[in]	size			The surface size.
		 *\param[in]	index			The surface index.
		 *\param[in]	sampler			The surface sampler.
		 *\param[in]	format			The surface pixel format.
		 *\~french
		 *\brief		Initialise la surface.
		 *\param[in]	renderTarget	La cible de rendu sur laquelle cet effet s'applique.
		 *\param[in]	size			Les dimensions de la surface.
		 *\param[in]	index			L'index de la surface.
		 *\param[in]	sampler			L'échantillonneur de la surface.
		 *\param[in]	format			Le format des pixels de la surface.
		 */
		C3D_API bool initialise( renderer::RenderPass const & renderPass
			, castor::Size const & size
			, SamplerSPtr sampler
			, renderer::Format format = renderer::Format::eR32G32B32A32_SFLOAT
			, uint32_t mipLevels = 1u );
		/**
		 *\~english
		 *\brief		Initialises the surface.
		 *\param[in]	renderTarget	The render target to which is attached this effect.
		 *\param[in]	size			The surface size.
		 *\param[in]	index			The surface index.
		 *\param[in]	sampler			The surface sampler.
		 *\param[in]	format			The surface pixel format.
		 *\~french
		 *\brief		Initialise la surface.
		 *\param[in]	renderTarget	La cible de rendu sur laquelle cet effet s'applique.
		 *\param[in]	size			Les dimensions de la surface.
		 *\param[in]	index			L'index de la surface.
		 *\param[in]	sampler			L'échantillonneur de la surface.
		 *\param[in]	format			Le format des pixels de la surface.
		 */
		C3D_API bool initialise( renderer::RenderPass const & renderPass
			, castor::Size const & size
			, SamplerSPtr sampler
			, renderer::Format colourFormat
			, renderer::Format depthFormat );
		/**
		 *\~english
		 *\brief		Initialises the surface.
		 *\param[in]	renderTarget	The render target to which is attached this effect.
		 *\param[in]	size			The surface size.
		 *\param[in]	index			The surface index.
		 *\param[in]	sampler			The surface sampler.
		 *\param[in]	format			The surface pixel format.
		 *\~french
		 *\brief		Initialise la surface.
		 *\param[in]	renderTarget	La cible de rendu sur laquelle cet effet s'applique.
		 *\param[in]	size			Les dimensions de la surface.
		 *\param[in]	index			L'index de la surface.
		 *\param[in]	sampler			L'échantillonneur de la surface.
		 *\param[in]	format			Le format des pixels de la surface.
		 */
		C3D_API bool initialise( renderer::RenderPass const & renderPass
			, castor::Size const & size
			, SamplerSPtr sampler
			, TextureLayoutSPtr colourTexture );
		/**
		 *\~english
		 *\brief		Initialises the surface.
		 *\param[in]	renderTarget	The render target to which is attached this effect.
		 *\param[in]	size			The surface size.
		 *\param[in]	index			The surface index.
		 *\param[in]	sampler			The surface sampler.
		 *\param[in]	format			The surface pixel format.
		 *\~french
		 *\brief		Initialise la surface.
		 *\param[in]	renderTarget	La cible de rendu sur laquelle cet effet s'applique.
		 *\param[in]	size			Les dimensions de la surface.
		 *\param[in]	index			L'index de la surface.
		 *\param[in]	sampler			L'échantillonneur de la surface.
		 *\param[in]	format			Le format des pixels de la surface.
		 */
		C3D_API bool initialise( renderer::RenderPass const & renderPass
			, castor::Size const & size
			, SamplerSPtr sampler
			, TextureLayoutSPtr colourTexture
			, renderer::Format depthFormat );
		/**
		 *\~english
		 *\brief		Initialises the surface.
		 *\param[in]	renderTarget	The render target to which is attached this effect.
		 *\param[in]	size			The surface size.
		 *\param[in]	index			The surface index.
		 *\param[in]	sampler			The surface sampler.
		 *\param[in]	format			The surface pixel format.
		 *\~french
		 *\brief		Initialise la surface.
		 *\param[in]	renderTarget	La cible de rendu sur laquelle cet effet s'applique.
		 *\param[in]	size			Les dimensions de la surface.
		 *\param[in]	index			L'index de la surface.
		 *\param[in]	sampler			L'échantillonneur de la surface.
		 *\param[in]	format			Le format des pixels de la surface.
		 */
		C3D_API bool initialise( renderer::RenderPass const & renderPass
			, castor::Size const & size
			, SamplerSPtr sampler
			, renderer::Format colourFormat
			, TextureLayoutSPtr depthTexture );
		/**
		 *\~english
		 *\brief		Initialises the surface.
		 *\param[in]	renderTarget	The render target to which is attached this effect.
		 *\param[in]	size			The surface size.
		 *\param[in]	index			The surface index.
		 *\param[in]	sampler			The surface sampler.
		 *\param[in]	format			The surface pixel format.
		 *\~french
		 *\brief		Initialise la surface.
		 *\param[in]	renderTarget	La cible de rendu sur laquelle cet effet s'applique.
		 *\param[in]	size			Les dimensions de la surface.
		 *\param[in]	index			L'index de la surface.
		 *\param[in]	sampler			L'échantillonneur de la surface.
		 *\param[in]	format			Le format des pixels de la surface.
		 */
		C3D_API bool initialise( renderer::RenderPass const & renderPass
			, castor::Size const & size
			, SamplerSPtr sampler
			, TextureLayoutSPtr colourTexture
			, TextureLayoutSPtr depthTexture );
		/**
		 *\~english
		 *\brief		Cleans up the surface.
		 *\~french
		 *\brief		Nettoie la surface.
		 */
		C3D_API void cleanup();

		//!\~english	The surface framebuffer.
		//!\~french		Le framebuffer de la surface.
		renderer::FrameBufferPtr frameBuffer;
		//!\~english	The sampler.
		//!\~french		L'échantillonneur.
		SamplerSPtr sampler;
		//!\~english	The surface colour texture.
		//!\~french		La texture couleur de la surface.
		TextureLayoutSPtr colourTexture;
		//!\~english	The surface depth texture.
		//!\~french		La texture profondeur de la surface.
		TextureLayoutSPtr depthTexture;
		//!\~english	The surface dimensions.
		//!\~french		Les dimensions de la surface.
		castor::Size size;
	};
}

#endif
