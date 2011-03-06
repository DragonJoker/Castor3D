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
#ifndef ___Gl2_RenderSystem___
#define ___Gl2_RenderSystem___

#include "Module_Gl2Render.h"
#include "Gl2LightRenderer.h"
#include "Gl2MaterialRenderer.h"
#include "Gl2SubmeshRenderer.h"
#include "Gl2TextureRenderer.h"
#include "Gl2WindowRenderer.h"
#include "Gl2Buffer.h"
#include "Gl2Context.h"
#include "Gl2ShaderProgram.h"

namespace Castor3D
{
	class C3D_Gl2_API Gl2RenderSystem : public GlRenderSystem
	{
	public:
		Gl2RenderSystem( SceneManager * p_pSceneManager);
		virtual ~Gl2RenderSystem();

		virtual void RenderAmbientLight( const Colour & p_clColour);

	public:
		inline static Gl2RenderSystem * GetSingletonPtr() { return RenderSystem::_getSingletonPtr<Gl2RenderSystem>(); }

		Gl2Context			*	GetMainContext			()const	{ return _getMainContext<Gl2Context>(); }
		Gl2Context			*	GetCurrentContext		()const	{ return _getCurrentContext<Gl2Context>(); }
		Gl2ShaderProgram	*	GetCurrentShaderProgram	()const	{ return _getCurrentShaderProgram<Gl2ShaderProgram>(); }

	private:
		virtual GlslShaderProgramPtr		_createGlslShaderProgram	( const String & p_vertexShaderFile, const String & p_fragmentShaderFile, const String & p_geometryShaderFile);

		virtual SubmeshRendererPtr				_createSubmeshRenderer	();
		virtual TextureEnvironmentRendererPtr	_createTexEnvRenderer	();
		virtual TextureRendererPtr				_createTextureRenderer	();
		virtual PassRendererPtr					_createPassRenderer		();
		virtual CameraRendererPtr				_createCameraRenderer	();
		virtual LightRendererPtr				_createLightRenderer	();
		virtual WindowRendererPtr				_createWindowRenderer	();
		virtual OverlayRendererPtr				_createOverlayRenderer	();

		virtual IndexBufferPtr			_createIndexBuffer		();
		virtual VertexBufferPtr			_createVertexBuffer		( const BufferElementDeclaration * p_pElements, size_t p_uiCount);
		virtual TextureBufferObjectPtr	_createTextureBuffer	();
	};
}

#endif