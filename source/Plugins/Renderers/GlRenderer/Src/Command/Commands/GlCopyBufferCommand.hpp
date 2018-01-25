/*
This file belongs to Renderer.
See LICENSE file in root folder
*/
#pragma once

#include "GlCommandBase.hpp"

#include <Miscellaneous/BufferCopy.hpp>

namespace gl_renderer
{
	/**
	*\brief
	*	Commande copie d'un vertex buffer.
	*/
	class CopyBufferCommand
		: public CommandBase
	{
	public:
		/**
		*\brief
		*	Constructeur.
		*\param[in] copyInfo
		*	Les informations de copie.
		*\param[in] src
		*	Le tampon source.
		*\param[in] dst
		*	Le tampon destination.
		*/
		CopyBufferCommand( renderer::BufferCopy const & copyInfo
			, renderer::BufferBase const & src
			, renderer::BufferBase const & dst );

		void apply()const override;
		CommandPtr clone()const override;

	private:
		Buffer const & m_src;
		Buffer const & m_dst;
		renderer::BufferCopy m_copyInfo;
	};
}
