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
#ifndef ___Gl2_ModuleRender___
#define ___Gl2_ModuleRender___

#include <OpenGlCommon/Module_GlRender.h>

#ifdef _WIN32
#	ifdef Gl2RenderSystem_EXPORTS
#		define C3D_Gl2_API __declspec(dllexport)
#	else
#		define C3D_Gl2_API __declspec(dllimport)
#	endif
#else
#	define C3D_Gl2_API
#endif

namespace Castor3D
{
	class GlVbIndicesBuffer;
	class GlVbVertexBuffer;
	class GlVbNormalsBuffer;
	class GlVbTextureBuffer;
	class GlVboTextureBuffer;

	class Gl2RendererDriver;
	class Gl2RenderSystem;

	class Gl2LightRenderer;
	class Gl2PassRenderer;
	class Gl2SubmeshRenderer;
	class Gl2TextureRenderer;
	class Gl2WindowRenderer;
	class Gl2Context;

	class Gl2ShaderProgram;

	typedef Templates::shared_ptr<Gl2ShaderProgram>		Gl2ShaderProgramPtr;
}

#endif
