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
#ifndef ___Gl_RenderSystem___
#define ___Gl_RenderSystem___

#include "Module_GlRender.hpp"
#include "GlPipeline.hpp"
#include "GlBuffer.hpp"
#include "GlContext.hpp"

namespace Castor3D
{
	class GlRenderSystem : public RenderSystem
	{
	public:
		typedef std::set <LightRendererPtr> LightRendererPtrSet;
		typedef LightRendererPtrSet::iterator iterator;
		typedef LightRendererPtrSet::const_iterator const_iterator;

	protected:
		static bool sm_useVertexBufferObjects;
		static bool sm_extensionsInit;

	protected:
		std::set <int>		m_setAvailableIndexes;
		std::set <int>		m_setAvailableGLIndexes;
		GlPipeline			m_pipeline;
		LightRendererPtrSet	m_setLightRenderers;
		int 				m_iOpenGlMajor;
		int 				m_iOpenGlMinor;

	public:
		GlRenderSystem();
		virtual ~GlRenderSystem();

		virtual void					Initialise					();
		virtual void					Delete						();
		virtual void					Cleanup						();

		bool							InitOpenGlExtensions		();	//!< Initialize OpenGL Extensions (using glew) \ingroup GLSL
		void							CheckShaderSupport			();		//!< Checks the different shader types support. \ingroup GLSL

		virtual void					RenderAmbientLight			( Colour const & p_clColour);
		virtual int						LockLight					();
		virtual void					UnlockLight					( int p_iIndex);
		virtual void					SetCurrentShaderProgram		( ShaderProgramBase * p_pVal);
		virtual void					DrawIndexedPrimitives		( ePRIMITIVE_TYPE p_eType, size_t p_uiMinVertex, size_t p_uiVertexCount, size_t p_uiFirstIndex, size_t p_uiCount);

	public:
		static inline iterator			GetLightRenderersIterator	()								{ return GetSingletonPtr()->m_setLightRenderers.begin(); }
		static inline const_iterator	GetLightRenderersEnd		()								{ return GetSingletonPtr()->m_setLightRenderers.end(); }
		static inline bool				UseVertexBufferObjects		()								{ return sm_useVertexBufferObjects; }
		static inline bool				IsExtensionInit				()								{ return sm_extensionsInit; }
		static inline int				GetOpenGlMajor				()								{ return GetSingletonPtr()->m_iOpenGlMajor; }
		static inline int				GetOpenGlMinor				()								{ return GetSingletonPtr()->m_iOpenGlMinor; }
		static inline void				SetOpenGlVersion			( int p_iMajor, int p_iMinor)	{ GetSingletonPtr()->m_iOpenGlMajor = p_iMajor;GetSingletonPtr()->m_iOpenGlMinor = p_iMinor; }

		virtual void					BeginOverlaysRendering		();

		inline static GlRenderSystem *	GetSingletonPtr				()								{ return RenderSystem::_getSingletonPtr<GlRenderSystem>(); }

		GlContext *						GetMainContext				()const							{ return _getMainContext<GlContext>(); }
		GlContext *						GetCurrentContext			()const							{ return _getCurrentContext<GlContext>(); }
		GlShaderProgram	*				GetCurrentShaderProgram		()const							{ return reinterpret_cast<GlShaderProgram *>( m_pCurrentProgram); }

	private:
		void							_isCgProfileSupported		( eSHADER_TYPE p_eType, char const * p_szName);
		void							_checkCgAvailableProfiles	();

		virtual GlslShaderProgramPtr	_createGlslShaderProgram	();
		virtual CgShaderProgramPtr		_createCgShaderProgram		();
		virtual HlslShaderProgramPtr	_createHlslShaderProgram	() { return HlslShaderProgramPtr(); }

		virtual SubmeshRendererPtr		_createSubmeshRenderer		();
		virtual TextureRendererPtr		_createTextureRenderer		();
		virtual PassRendererPtr			_createPassRenderer			();
		virtual CameraRendererPtr		_createCameraRenderer		();
		virtual LightRendererPtr		_createLightRenderer		();
		virtual WindowRendererPtr		_createWindowRenderer		();
		virtual OverlayRendererPtr		_createOverlayRenderer		();

		virtual IndexBufferPtr			_createIndexBuffer			();
		virtual VertexBufferPtr			_createVertexBuffer			( const BufferElementDeclaration * p_pElements, size_t p_uiCount);
		virtual TextureBufferObjectPtr	_createTextureBuffer		();
	};
}

#endif
