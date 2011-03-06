/*
This source file is part of Castor3D (http://dragonjoker.co.cc

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___Gl_TexEnvironmentRenderer___
#define ___Gl_TexEnvironmentRenderer___

#include "Module_GlRender.h"

namespace Castor3D
{
	class GlTextureEnvironmentRenderer : public TextureEnvironmentRenderer
	{
	protected:
		friend class GlRenderSystem;
		friend class GlTextureRenderer;

		typedef void (GlTextureEnvironmentRenderer:: * RenderFunc)();
		typedef RenderFunc PRenderFunc;

		PRenderFunc m_pfnRender;

	public:
		GlTextureEnvironmentRenderer( SceneManager * p_pSceneManager);
		virtual ~GlTextureEnvironmentRenderer();

		virtual void Render();

	protected:
		void _render();
		void _applyRGBCombinationStep( unsigned int p_index);
		void _applyAlphaCombinationStep( unsigned int p_index);
	};
}

#endif