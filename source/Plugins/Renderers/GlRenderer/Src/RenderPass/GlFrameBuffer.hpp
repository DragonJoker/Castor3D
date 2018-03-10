/*
This file belongs to RendererLib.
See LICENSE file in root folder
*/
#pragma once

#include "GlRendererPrerequisites.hpp"

#include <RenderPass/FrameBuffer.hpp>

namespace gl_renderer
{
	/**
	*\brief
	*	Classe encapsulant le concept de Framebuffer.
	*\remarks
	*	Contient les tampon de profondeur et de couleur.
	*/
	class FrameBuffer
		: public renderer::FrameBuffer
	{
	public:
		/**
		*\brief
		*	Crée un FrameBuffer compatible avec la passe de rendu donnée.
		*\remarks
		*	Si la compatibilité entre les textures voulues et les formats de la passe de rendu
		*	n'est pas possible, une std::runtime_error est lancée.
		*\param[in] dimensions
		*	Les dimensions du tampon d'images.
		*/
		FrameBuffer( renderer::RenderPass const & renderPass
			, renderer::Extent2D const & dimensions );
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
		FrameBuffer( renderer::RenderPass const & renderPass
			, renderer::Extent2D const & dimensions
			, renderer::FrameBufferAttachmentArray && textures );
		/**
		*\brief
		*	Destructeur
		*/
		~FrameBuffer();
		/**
		*\~english
		*\brief
		*	Sets the draw buffers.
		*\param[in] attaches
		*	The attaches.
		*\~french
		*\brief
		*	Définit les tampons d'écriture.
		*\param[in] attaches
		*	Les attaches.
		*/
		void setDrawBuffers( renderer::AttachmentDescriptionArray const & attaches )const;
		/**
		*\~english
		*\brief
		*	Sets the draw buffers.
		*\param[in] attaches
		*	The attaches.
		*\~french
		*\brief
		*	Définit les tampons d'écriture.
		*\param[in] attaches
		*	Les attaches.
		*/
		void setDrawBuffers( renderer::AttachmentReferenceArray const & attaches )const;
		/**
		*\return
		*	Le Framebuffer OpenGL.
		*/
		inline GLuint getFrameBuffer()const
		{
			assert( m_frameBuffer != GL_INVALID_INDEX );
			return m_frameBuffer;
		}
		/**
		*\~english
		*\return
		*	The colour attachments array.
		*\~french
		*\return
		*	Le tableau des attaches couleur.
		*/
		inline auto const & getColourAttaches()const
		{
			return m_colourAttaches;
		}
		/**
		*\~english
		*\return
		*	The depth and/or stencil attachments array.
		*\~french
		*\return
		*	Le tableau des attaches profondeur et/ou stencil.
		*/
		inline auto const & getDepthStencilAttaches()const
		{
			return m_depthStencilAttaches;
		}

	private:
		struct Attachment
		{
			GlAttachmentPoint point;
			GLuint object;
			GlAttachmentType type;
		};
		GLuint m_frameBuffer{ GL_INVALID_INDEX };
		std::vector< Attachment > m_colourAttaches;
		std::vector< Attachment > m_depthStencilAttaches;
	};
}
