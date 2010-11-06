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
#ifndef ___CSGLRenderSystem___
#define ___CSGLRenderSystem___

#include "Module_GLRender.h"
#include "GLPipeline.h"

namespace Castor3D
{
	class GLRenderSystem : public RenderSystem
	{
	protected:
		static bool sm_useVBO;
		static int sm_drawTypes[7];
		static int sm_environmentModes[5];
		static int sm_RGBCombinations[8];
		static int sm_RGBOperands[4];
		static int sm_alphaCombinations[6];
		static int sm_alphaOperands[2];
		static int sm_combinationSources[5];
		static int sm_textureDimensions[3];
		static int sm_lightIndexes[8];

		C3DSet( int) m_setAvailableIndexes;

		static bool sm_extensionsInit;
		static bool sm_geometryShader;
		static bool sm_gpuShader4;

		static char * sm_GLSLStrings[];

	protected:
		ContextPtrMap m_contextMap;
		GLContext * m_mainContext;
		GLContext * m_currentContext;
		GLShaderProgram * m_pCurrentProgram;
		std::set <LightRendererPtr> m_setLightRenderers;
		GLPipeline		m_pipeline;

	public:
		GLRenderSystem();
		virtual ~GLRenderSystem();

		void AddContext( GLContext * p_context, RenderWindow * p_window);
		std::set <LightRendererPtr>::iterator GetLightRenderersIterator() { return m_setLightRenderers.begin(); }
		std::set <LightRendererPtr>::const_iterator GetLightRenderersEnd() { return m_setLightRenderers.end(); }
		inline GLShaderProgram * GetCurrentShaderProgram() { return m_pCurrentProgram; }
		void SetCurrentShaderProgram( GLShaderProgram * p_pVal);
		int LockLight();
		void UnlockLight( int p_iIndex);

		CS3D_GL_API virtual void Initialise();
		CS3D_GL_API virtual void Delete();
		CS3D_GL_API virtual void DisplayArc( eDRAW_TYPE p_displayMode, const VertexPtrList & p_vertex);

		CS3D_GL_API bool InitOpenGLExtensions();		//!< Initialize OpenGL Extensions (using glew) \ingroup GLSL
		CS3D_GL_API bool HasGLSLSupport();				//!< Returns true if OpenGL Shading Language is supported. (This function will return a GLSL version number in a future release) \ingroup GLSL  
		CS3D_GL_API bool HasOpenGL2Support();			//!< Returns true if OpenGL 2.0 is supported. This function is deprecated and shouldn't be used anymore. \ingroup GLSL \deprecated
		CS3D_GL_API bool HasGeometryShaderSupport();	//!< Returns true if Geometry Shaders are supported. \ingroup GLSL
		CS3D_GL_API bool HasShaderModel4();				//!< Returns true if Shader Model 4 is supported. \ingroup GLSL

		CS3D_GL_API virtual ShaderObject *	CreateVertexShader		();
		CS3D_GL_API virtual ShaderObject *	CreateFragmentShader	();
		CS3D_GL_API virtual ShaderObject *	CreateGeometryShader	();
		CS3D_GL_API virtual ShaderProgram *	CreateShaderProgram		( const String & p_vertexShaderFile, const String & p_fragmentShaderFile, const String & p_geometryShaderFile);

		CS3D_GL_API virtual void ApplyTransformations( const Point3r & p_position, real * p_matrix);
		CS3D_GL_API virtual void RemoveTransformations();

		CS3D_GL_API virtual void BeginOverlaysRendering();
		CS3D_GL_API virtual void EndOverlaysRendering();

		CS3D_GL_API virtual void RenderAmbientLight( const Colour & p_clColour);

		static void GL_CheckError( const String & p_text);

	public:
		static inline bool	UseVBO					()									{ return sm_useVBO; }
		static inline int	GeteDRAW_TYPE				( eDRAW_TYPE p_index)					{ return sm_drawTypes[p_index]; }
		static inline int 	GetEnvironmentMode		( EnvironmentMode p_index)			{ return sm_environmentModes[p_index]; }
		static inline int 	GetRGBCombination		( RGBCombination p_index)			{ return sm_RGBCombinations[p_index]; }
		static inline int 	GetRGBOperand			( RGBOperand p_index)				{ return sm_RGBOperands[p_index]; }
		static inline int 	GetAlphaCombination		( AlphaCombination p_index)			{ return sm_alphaCombinations[p_index]; }
		static inline int 	GetAlphaOperand			( AlphaOperand p_index)				{ return sm_alphaOperands[p_index]; }
		static inline int 	GetCombinationSource	( CombinationSource p_index)		{ return sm_combinationSources[p_index]; }
		static inline int 	GetTextureDimension		( TextureUnit::eDIMENSION p_index)	{ return sm_textureDimensions[p_index]; }
		static inline int 	GetLightIndex			( LightIndexes p_index)				{ return sm_lightIndexes[p_index]; }
		static inline bool	IsExtensionInit			()									{ return sm_extensionsInit; }
		static inline bool	HasGeometryShader		()									{ return sm_geometryShader; }
		static inline bool	HasGPUShader4			()									{ return sm_gpuShader4; }
		static inline char*	GetGLSLString			( int p_index)						{ return sm_GLSLStrings[p_index]; }

		inline GLContext		*	GetCurrentContext		()const		{ return m_currentContext; }
		inline GLContext		*	GetMainContext			()const		{ return m_mainContext; }

		inline void	SetMainContext	( GLContext * p_context)	{ m_mainContext = p_context; }

		inline static GLRenderSystem * GetSingletonPtr() { return RenderSystem::GetSingletonPtr<GLRenderSystem>(); }

	private:
		CS3D_GL_API virtual SubmeshRendererPtr				_createSubmeshRenderer				();
		CS3D_GL_API virtual TextureEnvironmentRendererPtr	_createTextureEnvironmentRenderer	();
		CS3D_GL_API virtual TextureRendererPtr				_createTextureRenderer				();
		CS3D_GL_API virtual PassRendererPtr					_createPassRenderer					();
		CS3D_GL_API virtual LightRendererPtr				_createLightRenderer				();
		CS3D_GL_API virtual CameraRendererPtr				_createCameraRenderer				();
		CS3D_GL_API virtual ViewportRendererPtr				_createViewportRenderer				();
		CS3D_GL_API virtual WindowRendererPtr				_createWindowRenderer				();
		CS3D_GL_API virtual SceneNodeRendererPtr			_createSceneNodeRenderer			();
		CS3D_GL_API virtual OverlayRendererPtr				_createOverlayRenderer				();
	};

#ifdef _DEBUG
#	define CheckGLError( x) GLRenderSystem::GL_CheckError( x)
#else
#	define CheckGLError( x)
#endif
}

#endif