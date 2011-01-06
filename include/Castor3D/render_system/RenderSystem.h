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
		ShaderProgram						*	m_pCurrentProgram;

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
		template <class Ty> static typename SmartPtr<Ty>::Shared CreateRenderer()
		{
			CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
			SmartPtr<Ty>::Shared l_pReturn;

			if (sm_singleton != NULL)
			{
				l_pReturn = sm_singleton->_createRenderer<Ty>();

				if (l_pReturn != NULL)
				{
					sm_singleton->_addRenderer<Ty>( l_pReturn);
				}
			}

			return l_pReturn;
		}
		/**
		 * Creates a buffer
		 */
		template <class Ty> static typename SmartPtr<Ty>::Shared CreateBuffer()
		{
			CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
			SmartPtr<Ty>::Shared l_pReturn;

			if (sm_singleton != NULL)
			{
				l_pReturn = sm_singleton->_createBuffer<Ty>();
			}

			return l_pReturn;
		}
		/**
		 * Creates a buffer
		 */
		template <class Ty, typename T, size_t Count> static typename SmartPtr<Ty>::Shared CreateBuffer( const String & p_strArg)
		{
			CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
			SmartPtr<Ty>::Shared l_pReturn;

			if (sm_singleton != NULL)
			{
				l_pReturn = sm_singleton->_createBuffer<Ty, T, Count>( p_strArg);
			}

			return l_pReturn;
		}
		/**
		 * Creates a Vertex Shader, only the render system can do that
		 */
		virtual ShaderObject *		CreateVertexShader			() = 0;
		/**
		 * Creates a Fragment Shader, only the render system can do that
		 */
		virtual ShaderObject *		CreateFragmentShader		() = 0;
		/**
		 * Creates a Geometry Shader, only the render system can do that
		 */
		virtual ShaderObject *		CreateGeometryShader		() = 0;
		/**
		 * Creates a ShaderProgram, only the render system can do that
		 */
		virtual ShaderProgram *		CreateShaderProgram			( const String & p_vertexShaderFile, const String & p_fragmentShaderFile, const String & p_geometryShaderFile) = 0;
		/**
		 * Creates a Vertex Shader, only the render system can do that
		 */
		virtual CgShaderObject *	CreateCgVertexShader		() = 0;
		/**
		 * Creates a Fragment Shader, only the render system can do that
		 */
		virtual CgShaderObject *	CreateCgFragmentShader		() = 0;
		/**
		 * Creates a Geometry Shader, only the render system can do that
		 */
		virtual CgShaderObject *	CreateCgGeometryShader		() = 0;
		/**
		 * Creates a CgShaderProgram, only the render system can do that
		 */
		virtual CgShaderProgram *	CreateCgShaderProgram		( const String & p_vertexShaderFile, const String & p_fragmentShaderFile, const String & p_geometryShaderFile) = 0;
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

		static RenderSystem * GetSingletonPtr() { return _getSingletonPtr<RenderSystem>(); }

	public:
		/**@name Accessors */
		//@{
		static inline bool		UseMultiTexturing		()		{ return sm_useMultiTexturing; }
		static inline bool		UseShaders				()		{ return sm_useShaders; }
		static inline bool		ForceShaders			()		{ return sm_forceShaders; }
		static inline bool		HasGeometryShader		()		{ return sm_useGeometryShaders; }
		static inline bool		IsInitialised			()		{ return sm_initialised; }
		Context				*	GetCurrentContext		()const	{ return m_currentContext; }
		Context				*	GetMainContext			()const	{ return m_mainContext; }
		ShaderProgram		*	GetCurrentShaderProgram	()const	{ return m_pCurrentProgram; }

		virtual void	SetCurrentShaderProgram	( ShaderProgram * p_pVal)	{ m_pCurrentProgram = p_pVal; }
		inline void		SetMainContext			( Context * p_context)		{ m_mainContext = p_context; }
		//@}

	protected:
		template <class Ty> static Ty * _getSingletonPtr()
		{
			return static_cast <Ty *>( sm_singleton);
		}

		virtual SubmeshRendererPtr				_createSubmeshRenderer	()=0;
		virtual TextureEnvironmentRendererPtr	_createTexEnvRenderer	()=0;
		virtual TextureRendererPtr				_createTextureRenderer	()=0;
		virtual PassRendererPtr					_createPassRenderer		()=0;
		virtual CameraRendererPtr				_createCameraRenderer	()=0;
		virtual LightRendererPtr				_createLightRenderer	()=0;
		virtual WindowRendererPtr				_createWindowRenderer	()=0;
		virtual OverlayRendererPtr				_createOverlayRenderer	()=0;

		virtual IndicesBufferPtr		_createIndicesBuffer		()=0;
		virtual VertexBufferPtr			_createVertexBuffer			()=0;
		virtual NormalsBufferPtr		_createNormalsBuffer		()=0;
		virtual TextureBufferPtr		_createTextureBuffer		()=0;
		virtual VertexInfosBufferPtr	_createVertexInfosBuffer	()=0;
		virtual TextureBufferObjectPtr	_createTBOBuffer			()=0;

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
		template <class Ty> typename SmartPtr<Ty>::Shared _createRenderer()
		{
			CASTOR_EXCEPTION( CU_T( "Can't create a renderer of unknown type"));
		}
		template <class Ty> void _addRenderer( typename SmartPtr<Ty>::Shared p_pRenderer)
		{
			CASTOR_EXCEPTION( CU_T( "Can't add a renderer of unknown type"));
		}
		template <class Ty> typename SmartPtr<Ty>::Shared _createBuffer()
		{
			CASTOR_EXCEPTION( CU_T( "Can't create a buffer of unknown type"));
		}
		template <class Ty, typename T, size_t Count> typename SmartPtr<Ty>::Shared _createBuffer( const String & p_strArg)
		{
			CASTOR_EXCEPTION( CU_T( "Can't create a buffer of unknown type"));
		}


		template <> SubmeshRendererPtr _createRenderer<SubmeshRenderer>()
		{
			return _createSubmeshRenderer();
		}
		template <> TextureEnvironmentRendererPtr _createRenderer<TextureEnvironmentRenderer>()
		{
			return _createTexEnvRenderer();
		}
		template <> TextureRendererPtr _createRenderer<TextureRenderer>()
		{
			return _createTextureRenderer();
		}
		template <> PassRendererPtr _createRenderer<PassRenderer>()
		{
			return _createPassRenderer();
		}
		template <> CameraRendererPtr _createRenderer<CameraRenderer>()
		{
			return _createCameraRenderer();
		}
		template <> LightRendererPtr _createRenderer<LightRenderer>()
		{
			return _createLightRenderer();
		}
		template <> WindowRendererPtr _createRenderer<WindowRenderer>()
		{
			return _createWindowRenderer();
		}
		template <> OverlayRendererPtr _createRenderer<OverlayRenderer>()
		{
			return _createOverlayRenderer();
		}


		template <> void _addRenderer<SubmeshRenderer>( SubmeshRendererPtr p_pRenderer)
		{
			m_submeshesRenderers.push_back( p_pRenderer);
		}
		template <> void _addRenderer<TextureEnvironmentRenderer>( TextureEnvironmentRendererPtr p_pRenderer)
		{
			m_texEnvRenderers.push_back( p_pRenderer);
		}
		template <> void _addRenderer<TextureRenderer>( TextureRendererPtr p_pRenderer)
		{
			m_textureRenderers.push_back( p_pRenderer);
		}
		template <> void _addRenderer<PassRenderer>( PassRendererPtr p_pRenderer)
		{
			m_passRenderers.push_back( p_pRenderer);
		}
		template <> void _addRenderer<LightRenderer>( LightRendererPtr p_pRenderer)
		{
			m_lightRenderers.push_back( p_pRenderer);
		}
		template <> void _addRenderer<CameraRenderer>( CameraRendererPtr p_pRenderer)
		{
			m_cameraRenderers.push_back( p_pRenderer);
		}
		template <> void _addRenderer<WindowRenderer>( WindowRendererPtr p_pRenderer)
		{
			m_windowRenderers.push_back( p_pRenderer);
		}
		template <> void _addRenderer<OverlayRenderer>( OverlayRendererPtr p_pRenderer)
		{
			m_overlayRenderers.push_back( p_pRenderer);
		}


		template <> IndicesBufferPtr _createBuffer<IndicesBuffer>()
		{
			return _createIndicesBuffer();
		}
		template <> VertexBufferPtr _createBuffer<VertexBuffer>()
		{
			return _createVertexBuffer();
		}
		template <> NormalsBufferPtr _createBuffer<NormalsBuffer>()
		{
			return _createNormalsBuffer();
		}
		template <> TextureBufferPtr _createBuffer<TextureBuffer>()
		{
			return _createTextureBuffer();
		}
		template <> VertexInfosBufferPtr _createBuffer<VertexInfosBuffer>()
		{
			return _createVertexInfosBuffer();
		}
		template <> TextureBufferObjectPtr _createBuffer<TextureBufferObject>()
		{
			return _createTBOBuffer();
		}


		template <> VertexAttribsBufferBoolPtr _createBuffer<VertexAttribsBufferBool, bool, 1>( const String & p_strArg)
		{
			return _create1BoolVertexAttribsBuffer( p_strArg);
		}
		template <> VertexAttribsBufferIntPtr _createBuffer<VertexAttribsBufferInt, int, 1>( const String & p_strArg)
		{
			return _create1IntVertexAttribsBuffer( p_strArg);
		}
		template <> VertexAttribsBufferUIntPtr _createBuffer<VertexAttribsBufferUInt, unsigned int, 1>( const String & p_strArg)
		{
			return _create1UIntVertexAttribsBuffer( p_strArg);
		}
		template <> VertexAttribsBufferFloatPtr _createBuffer<VertexAttribsBufferFloat, float, 1>( const String & p_strArg)
		{
			return _create1FloatVertexAttribsBuffer( p_strArg);
		}
		template <> VertexAttribsBufferDoublePtr _createBuffer<VertexAttribsBufferDouble, double, 1>( const String & p_strArg)
		{
			return _create1DoubleVertexAttribsBuffer( p_strArg);
		}
		template <> VertexAttribsBufferBoolPtr _createBuffer<VertexAttribsBufferBool, bool, 2>( const String & p_strArg)
		{
			return _create2BoolVertexAttribsBuffer( p_strArg);
		}
		template <> VertexAttribsBufferIntPtr _createBuffer<VertexAttribsBufferInt, int, 2>( const String & p_strArg)
		{
			return _create2IntVertexAttribsBuffer( p_strArg);
		}
		template <> VertexAttribsBufferUIntPtr _createBuffer<VertexAttribsBufferUInt, unsigned int, 2>( const String & p_strArg)
		{
			return _create2UIntVertexAttribsBuffer( p_strArg);
		}
		template <> VertexAttribsBufferFloatPtr _createBuffer<VertexAttribsBufferFloat, float, 2>( const String & p_strArg)
		{
			return _create2FloatVertexAttribsBuffer( p_strArg);
		}
		template <> VertexAttribsBufferDoublePtr _createBuffer<VertexAttribsBufferDouble, double, 2>( const String & p_strArg)
		{
			return _create2DoubleVertexAttribsBuffer( p_strArg);
		}
		template <> VertexAttribsBufferBoolPtr _createBuffer<VertexAttribsBufferBool, bool, 3>( const String & p_strArg)
		{
			return _create3BoolVertexAttribsBuffer( p_strArg);
		}
		template <> VertexAttribsBufferIntPtr _createBuffer<VertexAttribsBufferInt, int, 3>( const String & p_strArg)
		{
			return _create3IntVertexAttribsBuffer( p_strArg);
		}
		template <> VertexAttribsBufferUIntPtr _createBuffer<VertexAttribsBufferUInt, unsigned int, 3>( const String & p_strArg)
		{
			return _create3UIntVertexAttribsBuffer( p_strArg);
		}
		template <> VertexAttribsBufferFloatPtr _createBuffer<VertexAttribsBufferFloat, float, 3>( const String & p_strArg)
		{
			return _create3FloatVertexAttribsBuffer( p_strArg);
		}
		template <> VertexAttribsBufferDoublePtr _createBuffer<VertexAttribsBufferDouble, double, 3>( const String & p_strArg)
		{
			return _create3DoubleVertexAttribsBuffer( p_strArg);
		}
		template <> VertexAttribsBufferBoolPtr _createBuffer<VertexAttribsBufferBool, bool, 4>( const String & p_strArg)
		{
			return _create4BoolVertexAttribsBuffer( p_strArg);
		}
		template <> VertexAttribsBufferIntPtr _createBuffer<VertexAttribsBufferInt, int, 4>( const String & p_strArg)
		{
			return _create4IntVertexAttribsBuffer( p_strArg);
		}
		template <> VertexAttribsBufferUIntPtr _createBuffer<VertexAttribsBufferUInt, unsigned int, 4>( const String & p_strArg)
		{
			return _create4UIntVertexAttribsBuffer( p_strArg);
		}
		template <> VertexAttribsBufferFloatPtr _createBuffer<VertexAttribsBufferFloat, float, 4>( const String & p_strArg)
		{
			return _create4FloatVertexAttribsBuffer( p_strArg);
		}
		template <> VertexAttribsBufferDoublePtr _createBuffer<VertexAttribsBufferDouble, double, 4>( const String & p_strArg)
		{
			return _create4DoubleVertexAttribsBuffer( p_strArg);
		}

		template <typename T> typename SmartPtr< VertexAttribsBuffer<T> >::Shared _createAttribsBuffer( const String & p_strArg)
		{
			return BufferManager::CreateBuffer< T, VertexAttribsBuffer<T> >( p_strArg);
		}
	};
}

#endif
