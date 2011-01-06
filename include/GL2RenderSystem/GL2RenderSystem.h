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
#ifndef ___GL2_RenderSystem___
#define ___GL2_RenderSystem___

#include "Module_GLRender.h"
#include "GL2LightRenderer.h"
#include "GL2MaterialRenderer.h"
#include "GL2SubmeshRenderer.h"
#include "GL2TextureRenderer.h"
#include "GL2WindowRenderer.h"
#include "GL2Buffer.h"

namespace Castor3D
{
	class C3D_GL2_API GL2RenderSystem : public GLRenderSystem
	{
	public:
		GL2RenderSystem( SceneManager * p_pSceneManager);
		virtual ~GL2RenderSystem();

		virtual ShaderProgram *		CreateShaderProgram		( const String & p_vertexShaderFile, const String & p_fragmentShaderFile, const String & p_geometryShaderFile);
		virtual void RenderAmbientLight( const Colour & p_clColour);

	public:
		inline static GL2RenderSystem * GetSingletonPtr() { return RenderSystem::_getSingletonPtr<GL2RenderSystem>(); }

	private:
		virtual SubmeshRendererPtr				_createSubmeshRenderer	();
		virtual TextureEnvironmentRendererPtr	_createTexEnvRenderer	();
		virtual TextureRendererPtr				_createTextureRenderer	();
		virtual PassRendererPtr					_createPassRenderer		();
		virtual CameraRendererPtr				_createCameraRenderer	();
		virtual LightRendererPtr				_createLightRenderer	();
		virtual WindowRendererPtr				_createWindowRenderer	();
		virtual OverlayRendererPtr				_createOverlayRenderer	();

		virtual IndicesBufferPtr		_createIndicesBuffer		();
		virtual VertexBufferPtr			_createVertexBuffer			();
		virtual NormalsBufferPtr		_createNormalsBuffer		();
		virtual TextureBufferPtr		_createTextureBuffer		();
		virtual VertexInfosBufferPtr	_createVertexInfosBuffer	();
		virtual TextureBufferObjectPtr	_createTBOBuffer			();
	};
}

#endif