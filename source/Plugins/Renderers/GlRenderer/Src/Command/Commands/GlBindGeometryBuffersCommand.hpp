/*
This file belongs to RendererLib.
See LICENSE file in root folder
*/
#pragma once

#include "GlCommandBase.hpp"

namespace gl_renderer
{
	/**
	*\brief
	*	Classe de base d'une commande.
	*/
	class BindGeometryBuffersCommand
		: public CommandBase
	{
	public:
		/**
		*\brief
		*	Constructeur.
		*/
		BindGeometryBuffersCommand( GeometryBuffers const & vao );

		void apply()const override;
		CommandPtr clone()const override;

	private:
		GeometryBuffers const & m_vao;
	};
}
