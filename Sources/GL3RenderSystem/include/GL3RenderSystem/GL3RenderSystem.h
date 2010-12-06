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
#ifndef ___GL3_RenderSystem___
#define ___GL3_RenderSystem___

#include "Module_GLRender.h"

namespace Castor3D
{
	class C3D_GL3_API GL3RenderSystem : public GLRenderSystem
	{
	public:
		GL3RenderSystem();
		virtual ~GL3RenderSystem();

		virtual void Initialise();

		void AddContext( GLContext * p_context, RenderWindow * p_window);
		std::set <LightRendererPtr>::iterator GetLightRenderersIterator() { return m_setLightRenderers.begin(); }
		std::set <LightRendererPtr>::const_iterator GetLightRenderersEnd() { return m_setLightRenderers.end(); }
		virtual void SetCurrentShaderProgram( ShaderProgram * p_pVal);
		virtual int LockLight();
		virtual void UnlockLight( int p_iIndex);

		virtual void DisplayArc( eDRAW_TYPE p_displayMode, const Point3rPtrList & p_vertex);

		virtual ShaderProgram *	CreateShaderProgram		( const String & p_vertexShaderFile, const String & p_fragmentShaderFile, const String & p_geometryShaderFile);

		virtual void BeginOverlaysRendering();
		virtual void RenderAmbientLight( const Colour & p_clColour);

	public:
		inline static GL3RenderSystem * GetSingletonPtr() { return RenderSystem::_getSingletonPtr<GL3RenderSystem>(); }

	private:
		virtual SubmeshRendererPtr				_createSubmeshRenderer				();
		virtual TextureEnvironmentRendererPtr	_createTextureEnvironmentRenderer	();
		virtual TextureRendererPtr				_createTextureRenderer				();
		virtual PassRendererPtr					_createPassRenderer					();
		virtual LightRendererPtr				_createLightRenderer				();
		virtual CameraRendererPtr				_createCameraRenderer				();
		virtual WindowRendererPtr				_createWindowRenderer				();
		virtual OverlayRendererPtr				_createOverlayRenderer				();
	};
}

#endif