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
#ifndef ___Gl_RenderSystem___
#define ___Gl_RenderSystem___

#include "Module_GlRender.h"
#include "GlPipeline.h"
#include "GlBuffer.h"
#include "GlContext.h"

namespace Castor3D
{
	class GlRenderSystem : public RenderSystem
	{
	protected:
		static bool sm_useVertexBufferObjects;
		static bool sm_extensionsInit;
		static bool sm_gpuShader4;

	protected:
		std::set <int>				m_setAvailableIndexes;
		std::set <int>				m_setAvailableGLIndexes;
		GlPipeline					m_pipeline;
		std::set <LightRendererPtr>	m_setLightRenderers;
		int 						m_iOpenGlMajor;
		int 						m_iOpenGlMinor;

	public:
		GlRenderSystem( SceneManager * p_pSceneManager);
		virtual ~GlRenderSystem();

		virtual void Initialise();
		virtual void Delete();
		virtual void Cleanup();

		bool InitOpenGlExtensions();	//!< Initialize OpenGL Extensions (using glew) \ingroup GLSL
		bool HasGlslSupport();			//!< Returns true if OpenGL Shading Language is supported. (This function will return a GLSL version number in a future release) \ingroup GLSL
		bool HasOpenGl2Support();		//!< Returns true if OpenGL 2.0 is supported. This function is deprecated and shouldn't be used anymore. \ingroup GLSL \deprecated
		bool HasGeometryShaderSupport();//!< Returns true if Geometry Shaders are supported. \ingroup GLSL
		bool HasShaderModel4();			//!< Returns true if Shader Model 4 is supported. \ingroup GLSL

		virtual void RenderAmbientLight( const Colour & p_clColour);

		virtual int LockLight();
		virtual void UnlockLight( int p_iIndex);
		virtual void SetCurrentShaderProgram( ShaderProgramBase * p_pVal);
		virtual void DrawIndexedPrimitives( ePRIMITIVE_TYPE p_eType, size_t p_uiMinVertex, size_t p_uiVertexCount, size_t p_uiFirstIndex, size_t p_uiCount);

	public:
		static inline bool	UseVertexBufferObjects	()	{ return sm_useVertexBufferObjects; }
		static inline bool	IsExtensionInit			()	{ return sm_extensionsInit; }
		static inline bool	HasGpuShader4			()	{ return sm_gpuShader4; }
		static inline int	GetOpenGlMajor			()	{ return GetSingletonPtr()->m_iOpenGlMajor; }
		static inline int	GetOpenGlMinor			()	{ return GetSingletonPtr()->m_iOpenGlMinor; }

		virtual void BeginOverlaysRendering();

		inline static GlRenderSystem * GetSingletonPtr() { return RenderSystem::_getSingletonPtr<GlRenderSystem>(); }

		GlContext			*	GetMainContext			()const	{ return _getMainContext<GlContext>(); }
		GlContext			*	GetCurrentContext		()const	{ return _getCurrentContext<GlContext>(); }
		GlShaderProgram		*	GetCurrentShaderProgram	()const	{ return _getCurrentShaderProgram<GlShaderProgram>(); }

	private:

		virtual GlslShaderObjectPtr		_createGlslVertexShader		();
		virtual GlslShaderObjectPtr		_createGlslFragmentShader	();
		virtual GlslShaderObjectPtr		_createGlslGeometryShader	();
		virtual CgShaderObjectPtr		_createCgVertexShader		();
		virtual CgShaderObjectPtr		_createCgFragmentShader		();
		virtual CgShaderObjectPtr		_createCgGeometryShader		();

		virtual HlslShaderObjectPtr		_createHlslVertexShader		() { return HlslShaderObjectPtr(); }
		virtual HlslShaderObjectPtr		_createHlslFragmentShader	() { return HlslShaderObjectPtr(); }
		virtual HlslShaderObjectPtr		_createHlslGeometryShader	() { return HlslShaderObjectPtr(); }
		virtual HlslShaderProgramPtr	_createHlslShaderProgram	( const String & p_vertexShaderFile, const String & p_fragmentShaderFile, const String & p_geometryShaderFile) { return HlslShaderProgramPtr(); }

		virtual CgShaderProgramPtr		_createCgShaderProgram		( const String & p_vertexShaderFile, const String & p_fragmentShaderFile, const String & p_geometryShaderFile);

		virtual VertexAttribsBufferBoolPtr		_create1BoolVertexAttribsBuffer		( const String & p_strArg);
		virtual VertexAttribsBufferIntPtr		_create1IntVertexAttribsBuffer		( const String & p_strArg);
		virtual VertexAttribsBufferUIntPtr		_create1UIntVertexAttribsBuffer		( const String & p_strArg);
		virtual VertexAttribsBufferFloatPtr		_create1FloatVertexAttribsBuffer	( const String & p_strArg);
		virtual VertexAttribsBufferDoublePtr	_create1DoubleVertexAttribsBuffer	( const String & p_strArg);
		virtual VertexAttribsBufferBoolPtr		_create2BoolVertexAttribsBuffer		( const String & p_strArg);
		virtual VertexAttribsBufferIntPtr		_create2IntVertexAttribsBuffer		( const String & p_strArg);
		virtual VertexAttribsBufferUIntPtr		_create2UIntVertexAttribsBuffer		( const String & p_strArg);
		virtual VertexAttribsBufferFloatPtr		_create2FloatVertexAttribsBuffer	( const String & p_strArg);
		virtual VertexAttribsBufferDoublePtr	_create2DoubleVertexAttribsBuffer	( const String & p_strArg);
		virtual VertexAttribsBufferBoolPtr		_create3BoolVertexAttribsBuffer		( const String & p_strArg);
		virtual VertexAttribsBufferIntPtr		_create3IntVertexAttribsBuffer		( const String & p_strArg);
		virtual VertexAttribsBufferUIntPtr		_create3UIntVertexAttribsBuffer		( const String & p_strArg);
		virtual VertexAttribsBufferFloatPtr		_create3FloatVertexAttribsBuffer	( const String & p_strArg);
		virtual VertexAttribsBufferDoublePtr	_create3DoubleVertexAttribsBuffer	( const String & p_strArg);
		virtual VertexAttribsBufferBoolPtr		_create4BoolVertexAttribsBuffer		( const String & p_strArg);
		virtual VertexAttribsBufferIntPtr		_create4IntVertexAttribsBuffer		( const String & p_strArg);
		virtual VertexAttribsBufferUIntPtr		_create4UIntVertexAttribsBuffer		( const String & p_strArg);
		virtual VertexAttribsBufferFloatPtr		_create4FloatVertexAttribsBuffer	( const String & p_strArg);
		virtual VertexAttribsBufferDoublePtr	_create4DoubleVertexAttribsBuffer	( const String & p_strArg);

		template <typename T, size_t Count> shared_ptr< GlVertexAttribsBuffer<T, Count> > _createAttribsBuffer( const String & p_strArg)
		{
			shared_ptr< GlVertexAttribsBuffer<T, Count> > l_pReturn;

			if (UseVertexBufferObjects() && UseShaders())
			{
				l_pReturn = BufferManager::CreateBuffer< T, GlVertexAttribsBuffer<T, Count> >( p_strArg);
			}

			return l_pReturn;
		}
	};
}

#endif
