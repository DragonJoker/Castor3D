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

#include "../shader/Module_Shader.h"
#include "../material/Module_Material.h"
#include "../render_system/Module_Render.h"

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
		static RenderSystem *							sm_singleton;			//!< The singleton
		static bool										sm_useMultiTexturing;	//!< Tells whether or not we can use multitexturing
		static bool										sm_useShaders;			//!< Tells whether or not we can use shaders
		static bool										sm_forceShaders;		//!< Tells whether or not we can use shaders
		static bool										sm_useGeometryShaders;	//!< Tells whether or not we can use geometry shaders
		static bool										sm_initialised;			//!< tells whether or not it is initialised

	protected:
		SubmeshRendererPtrArray				m_submeshesRenderers;
		TextureEnvironmentRendererPtrArray	m_texEnvRenderers;
		TextureRendererPtrArray				m_textureRenderers;
		PassRendererPtrArray				m_passRenderers;
		LightRendererPtrArray				m_lightRenderers;
		WindowRendererPtrArray				m_windowRenderers;
		CameraRendererPtrArray				m_cameraRenderers;
		OverlayRendererPtrArray				m_overlayRenderers;

		TextureRendererPtrArray				m_textureRenderersToCleanup;

		ContextPtrMap						m_contextMap;
		Context	*							m_mainContext;
		Context	*							m_currentContext;
		ShaderProgram *						m_pCurrentProgram;

	public:
		/**
		 * Constructor
		 */
		RenderSystem();
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
		/**
		 * Display an arc in a given display mode
		 */
		virtual void DisplayArc( eDRAW_TYPE p_displayMode, const Point3rPtrList & p_vertex) = 0;
		/**
		 * Creates a renderer
		 */
		template <typename Ty> static Templates::SharedPtr<Ty> CreateRenderer()
		{
			CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
			Templates::SharedPtr<Ty> l_pReturn;

			if (sm_singleton != NULL)
			{
				l_pReturn = sm_singleton->_createRendererNS<Ty>();
			}

			return l_pReturn;
		}
		/**
		 * Creates a Vertex Shader, only the render system can do that
		 */
		virtual ShaderObject *	CreateVertexShader		() = 0;
		/**
		 * Creates a Fragment Shader, only the render system can do that
		 */
		virtual ShaderObject *	CreateFragmentShader	() = 0;
		/**
		 * Creates a Geometry Shader, only the render system can do that
		 */
		virtual ShaderObject *	CreateGeometryShader	() = 0;
		/**
		 * Creates a ShaderProgram, only the render system can do that
		 */
		virtual ShaderProgram *	CreateShaderProgram		( const String & p_vertexShaderFile, const String & p_fragmentShaderFile, const String & p_geometryShaderFile) = 0;
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

	public:
		static inline RenderSystem	*	GetSingletonPtr			()	{ return _getSingletonPtr<RenderSystem>(); }
		static inline bool				UseMultiTexturing		()	{ return sm_useMultiTexturing; }
		static inline bool				UseShaders				()	{ return sm_useShaders; }
		static inline bool				ForceShaders			()	{ return sm_forceShaders; }
		static inline bool				HasGeometryShader		()	{ return sm_useGeometryShaders; }
		static inline bool				IsInitialised			()	{ return sm_initialised; }
		inline Context				*	GetCurrentContext		()	{ return m_currentContext; }
		inline Context				*	GetMainContext			()	{ return m_mainContext; }
		inline ShaderProgram		*	GetCurrentShaderProgram	()	{ return m_pCurrentProgram; }

		virtual void	SetCurrentShaderProgram	( ShaderProgram * p_pVal)	{ m_pCurrentProgram = p_pVal; }
		inline void		SetMainContext			( Context * p_context)		{ m_mainContext = p_context; }

	protected:
		template <typename T>
		static inline T			*	_getSingletonPtr	()	{ return (T *)sm_singleton; }

	private:
		template <typename Ty> Templates::SharedPtr<Ty> _createRendererNS()
		{
			Logger::LogWarning( CU_T( "Can't create a renderer of unknown type"));
		}

		template <> Templates::SharedPtr<SubmeshRenderer> _createRendererNS()
		{
			return _createSubmeshRenderer();
		}
		template <> Templates::SharedPtr<TextureEnvironmentRenderer> _createRendererNS()
		{
			return _createTextureEnvironmentRenderer();
		}
		template <> Templates::SharedPtr<TextureRenderer> _createRendererNS()
		{
			return _createTextureRenderer();
		}
		template <> Templates::SharedPtr<PassRenderer> _createRendererNS()
		{
			return _createPassRenderer();
		}
		template <> Templates::SharedPtr<LightRenderer> _createRendererNS()
		{
			return _createLightRenderer();
		}
		template <> Templates::SharedPtr<CameraRenderer> _createRendererNS()
		{
			return _createCameraRenderer();
		}
		template <> Templates::SharedPtr<WindowRenderer> _createRendererNS()
		{
			return _createWindowRenderer();
		}
		template <> Templates::SharedPtr<OverlayRenderer> _createRendererNS()
		{
			return _createOverlayRenderer();
		}

		virtual SubmeshRendererPtr				_createSubmeshRenderer				()=0;
		virtual TextureEnvironmentRendererPtr	_createTextureEnvironmentRenderer	()=0;
		virtual TextureRendererPtr				_createTextureRenderer				()=0;
		virtual PassRendererPtr					_createPassRenderer					()=0;
		virtual LightRendererPtr				_createLightRenderer				()=0;
		virtual CameraRendererPtr				_createCameraRenderer				()=0;
		virtual WindowRendererPtr				_createWindowRenderer				()=0;
		virtual OverlayRendererPtr				_createOverlayRenderer				()=0;
	};
}

#endif
