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
#ifndef ___C3D_RenderSystem___
#define ___C3D_RenderSystem___

#include "../Prerequisites.h"
#include "../Log.h"
#include "Buffer.h"

namespace Castor3D
{
	template <class Ty> struct RendererFunctor;
	template <class Ty> struct BufferCreator;
	template <class Ty> struct ShaderObjectCreator;
	template <class Ty> struct ShaderProgramFunctor;
	template <class Ty, typename T, size_t Count> struct VertexAttribCreator;
	//! The render system representation
	/*!
	This is the class which is the link between Castor3D and the renderer driver (OpenGL or Direct3D)
	It is also the noly class which can create the renderers
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	*/
	class C3D_API RenderSystem : public MemoryTraced<RenderSystem>
	{
	protected:
		template <class Ty> friend struct RendererFunctor;
		template <class Ty> friend struct BufferCreator;
		template <class Ty> friend struct ShaderObjectCreator;
		template <class Ty> friend struct ShaderProgramFunctor;
		template <class Ty, typename T, size_t Count> friend struct VertexAttribCreator;

		static Castor::MultiThreading::RecursiveMutex	m_mutex;
		static RenderSystem	*	sm_singleton;
		static bool				sm_useMultiTexturing;				//!< Tells whether or not we can use multitexturing
		static bool				sm_useShaders;						//!< Tells whether or not we can use shaders
		static bool				sm_forceShaders;					//!< Tells whether or not we can use shaders
		static bool				sm_useGeometryShaders;				//!< Tells whether or not we can use geometry shaders
		static bool				sm_initialised;						//!< tells whether or not it is initialised

	protected:
		SubmeshRendererPtrArray					m_submeshesRenderers;
		TextureEnvironmentRendererPtrArray		m_texEnvRenderers;
		TextureRendererPtrArray					m_textureRenderers;
		PassRendererPtrArray					m_passRenderers;
		LightRendererPtrArray					m_lightRenderers;
		WindowRendererPtrArray					m_windowRenderers;
		CameraRendererPtrArray					m_cameraRenderers;
		OverlayRendererPtrArray					m_overlayRenderers;

		TextureRendererPtrArray					m_textureRenderersToCleanup;

		ContextPtrMap							m_contextMap;
		Context								*	m_mainContext;
		Context								*	m_currentContext;
		ShaderProgramBase					*	m_pCurrentProgram;

		SceneManager						*	m_pSceneManager;

	public:
		/**
		 * Constructor
		 */
		RenderSystem( SceneManager * p_pSceneManager);
		/**
		 * Destructor
		 */
		virtual ~RenderSystem();
		/**
		 * Initialises the render system
		 */
		virtual void Initialise()							= 0;
		virtual void Delete()								= 0;
		virtual void CleanupRenderers();
		virtual void CleanupRenderersToCleanup();
		inline SceneManager * GetSceneManager	()const { return m_pSceneManager; }
		/**
		 * Creates a renderer
		 */
		template <class Ty> static shared_ptr<Ty> CreateRenderer();
		/**
		 * Creates a vertex buffer
		 */
		template <size_t N> static VertexBufferPtr CreateVertexBuffer( const BufferElementDeclaration (& p_elements)[N])
		{
			return CreateVertexBuffer( p_elements, N);
		}
		/**
		* Creates a vertex buffer
		 */
		static VertexBufferPtr CreateVertexBuffer( const BufferElementDeclaration * p_elements, size_t p_uiNbElements);
		/**
		 * Creates an index buffer
		 */
		static IndexBufferPtr CreateIndexBuffer();
		/**
		 * Creates a texture buffer
		 */
		static TextureBufferObjectPtr CreateTextureBuffer();
		/**
		 * Creates an attributes buffer
		 */
		template <class Ty, typename T, size_t Count> static shared_ptr<Ty> CreateBuffer( const String & p_strArg);
		/**
		 * Creates a Vertex Shader, only the render system can do that
		 */
		template <class Ty> static shared_ptr<Ty> CreateVertexShader();
		/**
		 * Creates a Fragment Shader, only the render system can do that
		 */
		template <class Ty> static shared_ptr<Ty> CreateFragmentShader();
		/**
		 * Creates a Geometry Shader, only the render system can do that
		 */
		template <class Ty> static shared_ptr<Ty> CreateGeometryShader();
		/**
		 * Creates a ShaderProgram, only the render system can do that
		 */
		template <class Ty> static shared_ptr<Ty> CreateShaderProgram( const String & p_vertexShaderFile, const String & p_fragmentShaderFile, const String & p_geometryShaderFile);
		/**
		 * Applies generic transformations (translation and rotation) from a position and an orientation
		 */
		virtual void ApplyTransformations( const Point3r & p_position, real * p_matrix);
		/**
		 * Applies generic transformations (translation and rotation) from a position and an orientation
		 */
		virtual void ApplyTransformations( const Point3r & p_position, const Matrix4x4r & p_matrix);
		/**
		* Removes generic transformations (translation and rotation) from a position and an orientation
		*/
		virtual void RemoveTransformations();
		/**
		 * Begins the overlay (2d elements) rendering
		 */
		virtual void BeginOverlaysRendering();
		/**
		 * Ends the overlay (2d elements) rendering
		 */
		virtual void EndOverlaysRendering();
		/**
		 * Renders the ambient lighting
		 */
		virtual void RenderAmbientLight( const Colour & p_clColour)=0;
		/**
		 * Adds a rendering context
		 */
		void AddContext( Context * p_context, RenderWindow * p_window);

		virtual int LockLight()=0;
		virtual void UnlockLight( int p_iIndex)=0;
		virtual void DrawIndexedPrimitives( ePRIMITIVE_TYPE p_eType, size_t p_uiMinVertex, size_t p_uiVertexCount, size_t p_uiFirstIndex, size_t p_uiCount)=0;

		static RenderSystem * GetSingletonPtr() { return _getSingletonPtr<RenderSystem>(); }

	public:
		/**@name Accessors */
		//@{
		static inline bool		UseMultiTexturing		()		{ return sm_useMultiTexturing; }
		static inline bool		UseShaders				()		{ return sm_useShaders; }
		static inline bool		ForceShaders			()		{ return sm_forceShaders; }
		static inline bool		HasGeometryShader		()		{ return sm_useGeometryShaders; }
		static inline bool		IsInitialised			()		{ return sm_initialised; }

		virtual void	SetCurrentShaderProgram	( ShaderProgramBase * p_pVal)	{ m_pCurrentProgram = p_pVal; }
		inline void		SetMainContext			( Context * p_context)			{ m_mainContext = p_context; }
		//@}

	protected:
		template <class Ty>	Ty	*	_getMainContext				()const	{ return static_cast <Ty *>( m_mainContext); }
		template <class Ty>	Ty	*	_getCurrentContext			()const	{ return static_cast <Ty *>( m_currentContext); }
		template <class Ty>	Ty	*	_getCurrentShaderProgram	()const	{ return static_cast <Ty *>( m_pCurrentProgram); }

		template <class Ty> static Ty * _getSingletonPtr()
		{
			return static_cast <Ty *>( sm_singleton);
		}
		void	_addGlslShaderProgram	( GlslShaderProgramPtr p_pToAdd);
		void	_addHlslShaderProgram	( HlslShaderProgramPtr p_pToAdd);
		void	_addCgShaderProgram		( CgShaderProgramPtr p_pToAdd);

		virtual GlslShaderObjectPtr		_createGlslVertexShader		() = 0;
		virtual GlslShaderObjectPtr		_createGlslFragmentShader	() = 0;
		virtual GlslShaderObjectPtr		_createGlslGeometryShader	() = 0;
		virtual HlslShaderObjectPtr		_createHlslVertexShader		() = 0;
		virtual HlslShaderObjectPtr		_createHlslFragmentShader	() = 0;
		virtual HlslShaderObjectPtr		_createHlslGeometryShader	() = 0;
		virtual CgShaderObjectPtr		_createCgVertexShader		() = 0;
		virtual CgShaderObjectPtr		_createCgFragmentShader		() = 0;
		virtual CgShaderObjectPtr		_createCgGeometryShader		() = 0;

		virtual GlslShaderProgramPtr	_createGlslShaderProgram	( const String & p_vertexShaderFile, const String & p_fragmentShaderFile, const String & p_geometryShaderFile) = 0;
		virtual HlslShaderProgramPtr	_createHlslShaderProgram	( const String & p_vertexShaderFile, const String & p_fragmentShaderFile, const String & p_geometryShaderFile) = 0;
		virtual CgShaderProgramPtr		_createCgShaderProgram		( const String & p_vertexShaderFile, const String & p_fragmentShaderFile, const String & p_geometryShaderFile) = 0;

		virtual SubmeshRendererPtr				_createSubmeshRenderer	()=0;
		virtual TextureEnvironmentRendererPtr	_createTexEnvRenderer	()=0;
		virtual TextureRendererPtr				_createTextureRenderer	()=0;
		virtual PassRendererPtr					_createPassRenderer		()=0;
		virtual CameraRendererPtr				_createCameraRenderer	()=0;
		virtual LightRendererPtr				_createLightRenderer	()=0;
		virtual WindowRendererPtr				_createWindowRenderer	()=0;
		virtual OverlayRendererPtr				_createOverlayRenderer	()=0;

		virtual IndexBufferPtr			_createIndexBuffer			()=0;
		virtual VertexBufferPtr			_createVertexBuffer			( const BufferElementDeclaration * p_elements, size_t p_uiNbElements)=0;
		virtual TextureBufferObjectPtr	_createTextureBuffer		()=0;

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

	private:
		template <typename T> shared_ptr< VertexAttribsBuffer<T> > _createAttribsBuffer( const String & p_strArg)
		{
			return BufferManager::CreateBuffer< T, VertexAttribsBuffer<T> >( p_strArg);
		}
	};

#include "RenderSystem.inl"
}

#endif
