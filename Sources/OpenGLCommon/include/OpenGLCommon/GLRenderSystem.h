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
#ifndef ___GL_RenderSystem___
#define ___GL_RenderSystem___

#include "Module_GLRender.h"
#include "GLPipeline.h"

namespace Castor3D
{
	class GLRenderSystem : public RenderSystem
	{
	protected:
		static bool sm_useVBO;
		static bool sm_extensionsInit;
		static bool sm_gpuShader4;

	protected:
		std::set <int>				m_setAvailableIndexes;
		std::set <int>				m_setAvailableGLIndexes;
		GLPipeline					m_pipeline;
		std::set <LightRendererPtr>	m_setLightRenderers;
		int 						m_iOpenGLMajor;
		int 						m_iOpenGLMinor;

	public:
		GLRenderSystem();
		virtual ~GLRenderSystem();

		virtual void Initialise();
		virtual void Delete();
		virtual void Cleanup();
		virtual void DisplayArc( eDRAW_TYPE p_displayMode, const Point3rPtrList & p_vertex);

		bool InitOpenGLExtensions();	//!< Initialize OpenGL Extensions (using glew) \ingroup GLSL
		bool HasGLSLSupport();			//!< Returns true if OpenGL Shading Language is supported. (This function will return a GLSL version number in a future release) \ingroup GLSL  
		bool HasOpenGL2Support();		//!< Returns true if OpenGL 2.0 is supported. This function is deprecated and shouldn't be used anymore. \ingroup GLSL \deprecated
		bool HasGeometryShaderSupport();//!< Returns true if Geometry Shaders are supported. \ingroup GLSL
		bool HasShaderModel4();			//!< Returns true if Shader Model 4 is supported. \ingroup GLSL

		virtual ShaderObject *	CreateVertexShader		();
		virtual ShaderObject *	CreateFragmentShader	();
		virtual ShaderObject *	CreateGeometryShader	();

		virtual void RenderAmbientLight( const Colour & p_clColour);

		virtual int LockLight();
		virtual void UnlockLight( int p_iIndex);
		virtual void SetCurrentShaderProgram( ShaderProgram * p_pVal);

	public:
		static inline bool	UseVBO			()	{ return sm_useVBO; }
		static inline bool	IsExtensionInit	()	{ return sm_extensionsInit; }
		static inline bool	HasGPUShader4	()	{ return sm_gpuShader4; }
		static inline int	GetOpenGLMajor	()	{ return GetSingletonPtr()->m_iOpenGLMajor; }
		static inline int	GetOpenGLMinor	()	{ return GetSingletonPtr()->m_iOpenGLMinor; }

		virtual void BeginOverlaysRendering();

		inline static GLRenderSystem * GetSingletonPtr() { return RenderSystem::_getSingletonPtr<GLRenderSystem>(); }
	};
}

#endif