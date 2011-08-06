/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

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

#include "Prerequisites.hpp"
#include "Logger.hpp"
#include "Buffer.hpp"
#include "ShaderProgram.hpp"

namespace Castor3D
{
	template <class Ty> struct RendererFunctor;
	template <class Ty> struct BufferCreator;
	template <class Ty> struct ShaderProgramFunctor;
	template <class Ty, typename T, size_t Count> struct VertexAttribCreator;
	//! The render system representation
	/*!
	This is the class C3D_API which is the link between Castor3D and the renderer driver (OpenGL or Direct3D)
	It is also the only class which can create the renderers
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	*/
	class C3D_API RenderSystem : public MemoryTraced<RenderSystem>
	{
	protected:
		template <class Ty> friend struct RendererFunctor;
		template <class Ty> friend struct BufferCreator;
		template <class Ty> friend struct ShaderProgramFunctor;
		template <class Ty, typename T, size_t Count> friend struct VertexAttribCreator;

		static Castor::MultiThreading::RecursiveMutex	m_mutex;
		static RenderSystem		*	sm_singleton;
		static bool					sm_useMultiTexturing;				//!< Tells whether or not we can use multitexturing
		static bool					sm_useShaders;						//!< Tells whether or not we can use shaders
		static bool					sm_forceShaders;					//!< Tells whether or not we must use shaders
		static bool					sm_useShader[eSHADER_TYPE_COUNT];		//!< Tells whether or not we can use shaders of each type
		static bool					sm_initialised;						//!< tells whether or not it is initialised

	protected:
		SubmeshRendererPtrArray		m_submeshesRenderers;
		TextureRendererPtrArray		m_textureRenderers;
		PassRendererPtrArray		m_passRenderers;
		LightRendererPtrArray		m_lightRenderers;
		WindowRendererPtrArray		m_windowRenderers;
		CameraRendererPtrArray		m_cameraRenderers;
		OverlayRendererPtrArray		m_overlayRenderers;

		TextureRendererPtrArray		m_textureRenderersToCleanup;

		ContextPtrMap				m_contextMap;
		Context					*	m_mainContext;
		Context					*	m_currentContext;
		ShaderProgramBase		*	m_pCurrentProgram;

		CGcontext					m_cgContext;	//!< Cg context, used for Cg programs creation
		CGprofile					m_mapSupportedProfiles[eSHADER_TYPE_COUNT];

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
		virtual	void					Initialise					()=0;
		virtual	void					Delete						()=0;
		virtual	void					CleanupRenderers			();
		virtual	void					CleanupRenderersToCleanup	();
		/**
		 * Creates a renderer
		 */
		template <class Ty>
		static	shared_ptr<Ty>			CreateRenderer				();
		/**
		 * Creates a vertex buffer
		 */
		template <size_t N>
		static	VertexBufferPtr			CreateVertexBuffer			( const BufferElementDeclaration (& p_elements)[N])
		{
			return CreateVertexBuffer( p_elements, N);
		}
		/**
		* Creates a vertex buffer
		 */
		static	VertexBufferPtr			CreateVertexBuffer			( const BufferElementDeclaration * p_elements, size_t p_uiNbElements);
		/**
		 * Creates an index buffer
		 */
		static	IndexBufferPtr			CreateIndexBuffer			();
		/**
		 * Creates a texture buffer
		 */
		static	TextureBufferObjectPtr	CreateTextureBuffer			();
		/**
		 * Creates a ShaderProgram, only the render system can do that
		 */
		template <class T> 
		static	shared_ptr<T>			CreateShaderProgram			();
		/**
		 * Begins the overlay (2d elements) rendering
		 */
		virtual	void					BeginOverlaysRendering		();
		/**
		 * Ends the overlay (2d elements) rendering
		 */
		virtual	void					EndOverlaysRendering		();
		/**
		 * Renders the ambient lighting
		 */
		virtual	void					RenderAmbientLight			( Colour const & p_clColour)=0;
		/**
		 * Adds a rendering context
		 */
				void					AddContext					( Context * p_context, RenderWindow * p_window);

		virtual	int						LockLight					()=0;
		virtual	void					UnlockLight					( int p_iIndex)=0;
		virtual	void					DrawIndexedPrimitives		( ePRIMITIVE_TYPE p_eType, size_t p_uiMinVertex, size_t p_uiVertexCount, size_t p_uiFirstIndex, size_t p_uiCount)=0;

		static	RenderSystem *			GetSingletonPtr				() { return _getSingletonPtr<RenderSystem>(); }

	public:
		/**@name Accessors */
		//@{
		static inline bool				UseMultiTexturing			()								{ return sm_useMultiTexturing; }
		static inline bool				UseShaders					()								{ return sm_useShaders; }
		static inline bool				ForceShaders				()								{ return sm_forceShaders; }
		static inline bool				HasShaderType				( eSHADER_TYPE p_eType)			{ return sm_useShader[p_eType]; }
		static inline bool				IsInitialised				()								{ return sm_initialised; }
		static inline CGcontext			GetCgContext				()								{ return sm_singleton->m_cgContext; }
		static inline CGprofile			GetCgProfile				( eSHADER_TYPE p_eType)			{ return sm_singleton->m_mapSupportedProfiles[p_eType]; }

		virtual void					SetCurrentShaderProgram		( ShaderProgramBase * p_pVal)	{ m_pCurrentProgram = p_pVal; }
		inline void						SetMainContext				( Context * p_context)			{ m_mainContext = p_context; }
		//@}

	protected:
		template <class Ty>	Ty	*		_getMainContext				()const							{ return static_cast <Ty *>( m_mainContext); }
		template <class Ty>	Ty	*		_getCurrentContext			()const							{ return static_cast <Ty *>( m_currentContext); }
		template <class Ty>	Ty	*		_getCurrentShaderProgram	()const							{ return static_cast <Ty *>( m_pCurrentProgram); }

		template <class Ty> static Ty *	_getSingletonPtr			()
		{
			return static_cast <Ty *>( sm_singleton);
		}

		void							_addShaderProgram			( ShaderProgramPtr p_pToAdd);

		virtual GlslShaderProgramPtr	_createGlslShaderProgram	()=0;
		virtual HlslShaderProgramPtr	_createHlslShaderProgram	()=0;
		virtual CgShaderProgramPtr		_createCgShaderProgram		()=0;

		virtual SubmeshRendererPtr		_createSubmeshRenderer		()=0;
		virtual TextureRendererPtr		_createTextureRenderer		()=0;
		virtual PassRendererPtr			_createPassRenderer			()=0;
		virtual CameraRendererPtr		_createCameraRenderer		()=0;
		virtual LightRendererPtr		_createLightRenderer		()=0;
		virtual WindowRendererPtr		_createWindowRenderer		()=0;
		virtual OverlayRendererPtr		_createOverlayRenderer		()=0;

		virtual IndexBufferPtr			_createIndexBuffer			()=0;
		virtual VertexBufferPtr			_createVertexBuffer			( const BufferElementDeclaration * p_elements, size_t p_uiNbElements)=0;
		virtual TextureBufferObjectPtr	_createTextureBuffer		()=0;
	};

#include "RenderSystem.inl"
}

#endif
