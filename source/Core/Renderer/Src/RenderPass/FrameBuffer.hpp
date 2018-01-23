/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_FrameBuffer_HPP___
#define ___Renderer_FrameBuffer_HPP___
#pragma once

#include "RendererPrerequisites.hpp"

namespace renderer
{
	/**
	*\brief
	*	Classe encapsulant le concept de Framebuffer.
	*\remarks
	*	Contient les tampon de profondeur et de couleur.
	*/
	class FrameBuffer
	{
	protected:
		/**
		*\brief
		*	Crée un FrameBuffer.
		*/
		FrameBuffer();
		/**
		*\brief
		*	Crée un FrameBuffer compatible avec la passe de rendu donnée.
		*\remarks
		*	Si la compatibilité entre les textures voulues et les formats de la passe de rendu
		*	n'est pas possible, une std::runtime_error est lancée.
		*\param[in] dimensions
		*	Les dimensions du tampon d'images.
		*\param[in] textures
		*	Les textures voulues pour le tampon d'images à créer.
		*/
		FrameBuffer( RenderPass const & renderPass
			, UIVec2 const & dimensions
			, TextureViewCRefArray const & textures );

	public:
		/**
		*\~english
		*\brief
		*	Destructor.
		*\~french
		*\brief
		*	Destructeur.
		*/
		virtual ~FrameBuffer() = default;
		/**
		*\brief
		*	Copie des données dans la RAM.
		*\remarks
		*	Pour utiliser cette fonction, il faut que le tampon soit activé.
		*\param[in] xoffset, yoffset
		*	Le décalage à partir duquel les données seront copiées, par rapport
		*	au d�but du stockage de la texture, en VRAM.
		*\param[in] width, height
		*	Les dimensions des données à copier.
		*\param[in] format
		*	Le format voulu pour les données.
		*\param[out] data
		*	Reçoit les données copiées.
		*/
		virtual void download( Queue const & queue
			, uint32_t index
			, uint32_t xoffset
			, uint32_t yoffset
			, uint32_t width
			, uint32_t height
			, PixelFormat format
			, uint8_t * data )const noexcept = 0;
	};
}

#endif
