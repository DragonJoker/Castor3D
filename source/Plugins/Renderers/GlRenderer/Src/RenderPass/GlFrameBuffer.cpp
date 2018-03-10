/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#include "RenderPass/GlFrameBuffer.hpp"

#include "Command/GlQueue.hpp"
#include "RenderPass/GlRenderPass.hpp"
#include "Image/GlTexture.hpp"
#include "Image/GlTextureView.hpp"

#include <RenderPass/AttachmentReference.hpp>

#include <iostream>

namespace gl_renderer
{
	namespace
	{
		enum GlFramebufferStatus
			: GLenum
		{
			GL_FRAMEBUFFER_STATUS_UNDEFINED = 0x8219,
			GL_FRAMEBUFFER_STATUS_COMPLETE = 0x8CD5,
			GL_FRAMEBUFFER_STATUS_INCOMPLETE_ATTACHMENT = 0x8CD6,
			GL_FRAMEBUFFER_STATUS_INCOMPLETE_MISSING_ATTACHMENT = 0x8CD7,
			GL_FRAMEBUFFER_STATUS_INCOMPLETE_DRAW_BUFFER = 0x8CDB,
			GL_FRAMEBUFFER_STATUS_INCOMPLETE_READ_BUFFER = 0x8CDC,
			GL_FRAMEBUFFER_STATUS_UNSUPPORTED = 0x8CDD,
			GL_FRAMEBUFFER_STATUS_INCOMPLETE_MULTISAMPLE = 0x8D56,
			GL_FRAMEBUFFER_STATUS_INCOMPLETE_LAYER_TARGETS = 0x8DA8,
		};

		GlAttachmentPoint getAttachmentPoint( renderer::Format format )
		{
			if ( renderer::isDepthStencilFormat( format ) )
			{
				return GL_ATTACHMENT_POINT_DEPTH_STENCIL;
			}

			if ( renderer::isStencilFormat( format ) )
			{
				return GL_ATTACHMENT_POINT_STENCIL;
			}

			if ( renderer::isDepthFormat( format ) )
			{
				return GL_ATTACHMENT_POINT_DEPTH;
			}

			return GL_ATTACHMENT_POINT_COLOR0;
		}

		GlAttachmentPoint getAttachmentPoint( TextureView const & texture )
		{
			return getAttachmentPoint( texture.getFormat() );
		}

		GlAttachmentType getAttachmentType( renderer::Format format )
		{
			if ( renderer::isDepthStencilFormat( format ) )
			{
				return GL_ATTACHMENT_TYPE_DEPTH_STENCIL;
			}

			if ( renderer::isStencilFormat( format ) )
			{
				return GL_ATTACHMENT_TYPE_STENCIL;
			}

			if ( renderer::isDepthFormat( format ) )
			{
				return GL_ATTACHMENT_TYPE_DEPTH;
			}

			return GL_ATTACHMENT_TYPE_COLOR;
		}

		GlAttachmentType getAttachmentType( TextureView const & texture )
		{
			return getAttachmentType( texture.getFormat() );
		}

		void doCheck( GLenum status )
		{
			switch ( status )
			{
			case 0:
				std::cerr << "An error has occured." << std::endl;
				assert( false );
				break;

			case GL_FRAMEBUFFER_STATUS_COMPLETE:
				break;

			case GL_FRAMEBUFFER_STATUS_UNDEFINED:
				std::cerr << "The specified framebuffer is the default read or draw framebuffer, but the default framebuffer does not exist." << std::endl;
				assert( false );
				break;

			case GL_FRAMEBUFFER_STATUS_INCOMPLETE_ATTACHMENT:
				std::cerr << "At least one of the framebuffer attachment points are framebuffer incomplete." << std::endl;
				assert( false );
				break;

			case GL_FRAMEBUFFER_STATUS_INCOMPLETE_MISSING_ATTACHMENT:
				std::cerr << "The framebuffer does not have at least one image attached to it." << std::endl;
				assert( false );
				break;

			case GL_FRAMEBUFFER_STATUS_INCOMPLETE_DRAW_BUFFER:
				std::cerr << "The value of GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE is GL_NONE for any color attachment point(s) named by GL_DRAW_BUFFERi." << std::endl;
				assert( false );
				break;

			case GL_FRAMEBUFFER_STATUS_INCOMPLETE_READ_BUFFER:
				std::cerr << "GL_READ_BUFFER is not GL_NONE and the value of GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE is GL_NONE for the color attachment point named by GL_READ_BUFFER." << std::endl;
				assert( false );
				break;

			case GL_FRAMEBUFFER_STATUS_UNSUPPORTED:
				std::cerr << "The combination of internal formats of the attached images violates an implementation-dependent set of restrictions." << std::endl;
				assert( false );
				break;

			case GL_FRAMEBUFFER_STATUS_INCOMPLETE_MULTISAMPLE:
				std::cerr << "One of the following:" << std::endl;
				std::cerr << "  - The value of GL_RENDERBUFFER_SAMPLES is not the same for all attached renderbuffers;" << std::endl;
				std::cerr << "  - The value of GL_TEXTURE_SAMPLES is the not same for all attached textures;" << std::endl;
				std::cerr << "  - The attached images are a mix of renderbuffers and textures, the value of GL_RENDERBUFFER_SAMPLES does not match the value of GL_TEXTURE_SAMPLES;" << std::endl;
				std::cerr << "  - The value of GL_TEXTURE_FIXED_SAMPLE_LOCATIONS is not the same for all attached textures;" << std::endl;
				std::cerr << "  - The attached images are a mix of renderbuffers and textures, the value of GL_TEXTURE_FIXED_SAMPLE_LOCATIONS is not GL_TRUE for all attached textures." << std::endl;
				assert( false );
				break;

			case GL_FRAMEBUFFER_STATUS_INCOMPLETE_LAYER_TARGETS:
				std::cerr << "At least one framebuffer attachment is layered, and any populated attachment is not layered, or all populated color attachments are not from textures of the same target." << std::endl;
				assert( false );
				break;
			}
		}
	}

	FrameBuffer::FrameBuffer( renderer::RenderPass const & renderPass
		, renderer::Extent2D const & dimensions )
		: renderer::FrameBuffer{ renderPass, dimensions, renderer::FrameBufferAttachmentArray{} }
		, m_frameBuffer{ 0u }
	{
	}

	FrameBuffer::FrameBuffer( renderer::RenderPass const & renderPass
		, renderer::Extent2D const & dimensions
		, renderer::FrameBufferAttachmentArray && views )
		: renderer::FrameBuffer{ renderPass, dimensions, std::move( views ) }
	{
		glLogCall( gl::GenFramebuffers, 1, &m_frameBuffer );
		glLogCall( gl::BindFramebuffer, GL_FRAMEBUFFER, m_frameBuffer );

		for ( auto & attach : m_attachments )
		{
			// If the image doesn't exist, it means it is a backbuffer image, hence ignore the attachment.
			if ( static_cast< Texture const & >( attach.getView().getTexture() ).hasImage() )
			{
				auto index = attach.getAttachment().index;
				Attachment attachment
				{
					getAttachmentPoint( static_cast< TextureView const & >( attach.getView() ) ),
					( ( ( attach.getView().getTexture().getType() == renderer::TextureType::e2D && attach.getView().getTexture().getLayerCount() <= 1u )
						&& attach.getView().getType() == renderer::TextureViewType::e2D )
							? static_cast< Texture const & >( attach.getView().getTexture() ).getImage()
							: static_cast< TextureView const & >( attach.getView() ).getImage() ),
					getAttachmentType( static_cast< TextureView const & >( attach.getView() ) ),
				};

				if ( attachment.point == GL_ATTACHMENT_POINT_DEPTH_STENCIL
					|| attachment.point == GL_ATTACHMENT_POINT_DEPTH
					|| attachment.point == GL_ATTACHMENT_POINT_STENCIL )
				{
					index = 0u;
					m_depthStencilAttaches.push_back( attachment );
				}
				else
				{
					m_colourAttaches.push_back( attachment );
				}

				auto target = GL_TEXTURE_2D;

				if ( static_cast< Texture const & >( attach.getTexture() ).getSamplesCount() > renderer::SampleCountFlag::e1 )
				{
					target = GL_TEXTURE_2D_MULTISAMPLE;
				}

				glLogCall( gl::FramebufferTexture2D
					, GL_FRAMEBUFFER
					, GlAttachmentPoint( attachment.point + index )
					, target
					, attachment.object
					, attach.getView().getSubResourceRange().baseMipLevel );
				doCheck( gl::CheckFramebufferStatus( GL_FRAMEBUFFER ) );
			}
			else
			{
				Attachment attachment
				{
					GL_ATTACHMENT_POINT_BACK,
					GL_INVALID_INDEX,
					getAttachmentType( attach.getView().getFormat() )
				};

				if ( renderer::isDepthOrStencilFormat( attach.getFormat() ) )
				{
					m_depthStencilAttaches.push_back( attachment );
				}
				else
				{
					m_colourAttaches.push_back( attachment );
				}
			}
		}

		doCheck( gl::CheckFramebufferStatus( GL_FRAMEBUFFER ) );
		glLogCall( gl::BindFramebuffer, GL_FRAMEBUFFER, 0 );
	}

	FrameBuffer::~FrameBuffer()
	{
		if ( m_frameBuffer > 0u )
		{
			glLogCall( gl::DeleteFramebuffers, 1, &m_frameBuffer );
		}
	}

	void FrameBuffer::setDrawBuffers( renderer::AttachmentDescriptionArray const & attaches )const
	{
		renderer::UInt32Array colours;

		for ( auto & attach : attaches )
		{
			auto & fboAttach = m_attachments[attach.index];

			if ( !renderer::isDepthOrStencilFormat( attach.format ) )
			{
				if ( static_cast< Texture const & >( fboAttach.getTexture() ).hasImage() )
				{
					colours.push_back( getAttachmentPoint( attach.format ) + attach.index );
				}
				else if ( attaches.size() == 1 )
				{
					colours.push_back( GL_ATTACHMENT_POINT_BACK );
				}
			}
		}

		glLogCall( gl::DrawBuffers, GLsizei( colours.size() ), colours.data() );
	}

	void FrameBuffer::setDrawBuffers( renderer::AttachmentReferenceArray const & attaches )const
	{
		if ( getFrameBuffer() != GL_INVALID_INDEX )
		{
			renderer::UInt32Array colours;

			for ( auto & attach : attaches )
			{
				auto & fboAttach = m_attachments[attach.attachment];
				auto format = fboAttach.getFormat();

				if ( !renderer::isDepthOrStencilFormat( format ) )
				{
					if ( static_cast< Texture const & >( fboAttach.getTexture() ).hasImage() )
					{
						colours.push_back( getAttachmentPoint( format ) + attach.attachment );
					}
					else if ( attaches.size() == 1 )
					{
						colours.push_back( GL_ATTACHMENT_POINT_BACK );
					}
				}
			}

			glLogCall( gl::DrawBuffers, GLsizei( colours.size() ), colours.data() );
		}
	}
}
