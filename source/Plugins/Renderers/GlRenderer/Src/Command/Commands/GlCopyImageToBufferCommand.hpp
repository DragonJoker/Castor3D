/*
This file belongs to RendererLib.
See LICENSE file in root folder
*/
#pragma once

#include "GlCommandBase.hpp"

#include <Image/TextureView.hpp>
#include <Miscellaneous/BufferImageCopy.hpp>

namespace gl_renderer
{
	/**
	*\brief
	*	Commande de copie du contenu d'une image dans un tampon.
	*/
	class CopyImageToBufferCommand
		: public CommandBase
	{
	public:
		/**
		*\brief
		*	Constructeur.
		*\param[in] copyInfo
		*	Les informations de copie.
		*\param[in] src
		*	L'image source.
		*\param[in] dst
		*	Le tampon destination.
		*/
		CopyImageToBufferCommand( renderer::BufferImageCopyArray const & copyInfo
			, renderer::Texture const & src
			, renderer::BufferBase const & dst );
		CopyImageToBufferCommand( CopyImageToBufferCommand const & rhs );

		void apply()const override;
		CommandPtr clone()const override;

	private:
		void applyOne( renderer::BufferImageCopy const & copyInfo
			, renderer::TextureView const & view )const;

	private:
		Texture const & m_src;
		Buffer const & m_dst;
		renderer::BufferImageCopyArray m_copyInfo;
		GlInternal m_internal;
		GlFormat m_format;
		GlType m_type;
		GlTextureType m_target;
		std::vector< renderer::TextureViewPtr > m_views;
	};
}
