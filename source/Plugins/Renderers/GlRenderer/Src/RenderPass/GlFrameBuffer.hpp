/*
This file belongs to Renderer.
See LICENSE file in root folder
*/
#pragma once

#include "GlRendererPrerequisites.hpp"

#include <RenderPass/FrameBuffer.hpp>

namespace gl_renderer
{
	enum GlAttachmentPoint
		: GLenum
	{
		GL_ATTACHMENT_POINT_DEPTH_STENCIL = 0x821A,
		GL_ATTACHMENT_POINT_COLOR0 = 0x8CE0,
		GL_ATTACHMENT_POINT_COLOR1 = 0x8CE1,
		GL_ATTACHMENT_POINT_COLOR2 = 0x8CE2,
		GL_ATTACHMENT_POINT_COLOR3 = 0x8CE3,
		GL_ATTACHMENT_POINT_COLOR4 = 0x8CE4,
		GL_ATTACHMENT_POINT_COLOR5 = 0x8CE5,
		GL_ATTACHMENT_POINT_COLOR6 = 0x8CE6,
		GL_ATTACHMENT_POINT_COLOR7 = 0x8CE7,
		GL_ATTACHMENT_POINT_COLOR8 = 0x8CE8,
		GL_ATTACHMENT_POINT_COLOR9 = 0x8CE9,
		GL_ATTACHMENT_POINT_COLOR10 = 0x8CEA,
		GL_ATTACHMENT_POINT_COLOR11 = 0x8CEB,
		GL_ATTACHMENT_POINT_COLOR12 = 0x8CEC,
		GL_ATTACHMENT_POINT_COLOR13 = 0x8CED,
		GL_ATTACHMENT_POINT_COLOR14 = 0x8CEE,
		GL_ATTACHMENT_POINT_COLOR15 = 0x8CEF,
		GL_ATTACHMENT_POINT_DEPTH = 0x8D00,
		GL_ATTACHMENT_POINT_STENCIL = 0x8D20,
	};

	enum GlAttachmentType
		: GLenum
	{
		GL_ATTACHMENT_TYPE_COLOR = 0x1800,
		GL_ATTACHMENT_TYPE_DEPTH = 0x1801,
		GL_ATTACHMENT_TYPE_STENCIL = 0x1802,
		GL_ATTACHMENT_TYPE_DEPTH_STENCIL = 0x84F9,
	};

	enum GlFrameBufferTarget
		: GLenum
	{
		GL_FRAMEBUFFER = 0x8D40,
		GL_READ_FRAMEBUFFER = 0x8CA8,
		GL_DRAW_FRAMEBUFFER = 0x8CA9,
	};

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
			, renderer::UIVec2 const & dimensions );
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
			, renderer::UIVec2 const & dimensions
			, renderer::TextureViewCRefArray const & textures );
		/**
		*\brief
		*	Destructeur
		*/
		~FrameBuffer();
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
		void download( renderer::Queue const & queue
			, uint32_t index
			, uint32_t xoffset
			, uint32_t yoffset
			, uint32_t width
			, uint32_t height
			, renderer::PixelFormat format
			, uint8_t * data )const noexcept;
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
			GLuint index;
			GLuint object;
			GlAttachmentType type;
		};
		GLuint m_frameBuffer{ GL_INVALID_INDEX };
		std::vector< Attachment > m_colourAttaches;
		std::vector< Attachment > m_depthStencilAttaches;
	};
}
