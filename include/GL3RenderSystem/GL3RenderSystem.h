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
#ifndef ___Gl3_RenderSystem___
#define ___Gl3_RenderSystem___

#include "Module_Gl3Render.h"
#include "Gl3Context.h"
#include "Gl3ShaderProgram.h"

namespace Castor3D
{
	class C3D_Gl3_API Gl3RenderSystem : public GlRenderSystem
	{
	public:
		Gl3RenderSystem( SceneManager * p_pSceneManager);
		virtual ~Gl3RenderSystem();

		virtual void Initialise();

		void AddContext( Context * p_context, RenderWindow * p_window);
		std::set <LightRendererPtr>::iterator GetLightRenderersIterator() { return m_setLightRenderers.begin(); }
		std::set <LightRendererPtr>::const_iterator GetLightRenderersEnd() { return m_setLightRenderers.end(); }
		virtual void SetCurrentShaderProgram( ShaderProgramBase * p_pVal);
		virtual int LockLight();
		virtual void UnlockLight( int p_iIndex);

		virtual void BeginOverlaysRendering();
		virtual void RenderAmbientLight( const Colour & p_clColour);

	public:
		inline static Gl3RenderSystem * GetSingletonPtr() { return RenderSystem::_getSingletonPtr<Gl3RenderSystem>(); }

		Gl3Context			*	GetMainContext			()const	{ return _getMainContext<Gl3Context>(); }
		Gl3Context			*	GetCurrentContext		()const	{ return _getCurrentContext<Gl3Context>(); }
		Gl3ShaderProgram	*	GetCurrentShaderProgram	()const	{ return _getCurrentShaderProgram<Gl3ShaderProgram>(); }

	private:
		virtual GlslShaderProgramPtr	_createGlslShaderProgram	( const String & p_vertexShaderFile, const String & p_fragmentShaderFile, const String & p_geometryShaderFile);

		virtual SubmeshRendererPtr				_createSubmeshRenderer	();
		virtual TextureEnvironmentRendererPtr	_createTexEnvRenderer	();
		virtual TextureRendererPtr				_createTextureRenderer	();
		virtual PassRendererPtr					_createPassRenderer		();
		virtual CameraRendererPtr				_createCameraRenderer	();
		virtual LightRendererPtr				_createLightRenderer	();
		virtual WindowRendererPtr				_createWindowRenderer	();
		virtual OverlayRendererPtr				_createOverlayRenderer	();

		virtual IndexBufferPtr			_createIndexBuffer			();
		virtual VertexBufferPtr			_createVertexBuffer			( const BufferElementDeclaration * p_pElements, size_t p_uiCount);
		virtual TextureBufferObjectPtr	_createTextureBuffer		();
	};
}

#endif
