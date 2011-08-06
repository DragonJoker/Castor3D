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
#ifndef ___Dx9_RenderSystem___
#define ___Dx9_RenderSystem___

#include "Module_Dx9Render.hpp"
#include "Dx9Pipeline.hpp"
#include "Dx9Buffer.hpp"
#include "Dx9Context.hpp"
#include "Dx9ShaderProgram.hpp"

namespace Castor3D
{
	class Dx9RenderSystem : public RenderSystem
	{
	protected:
		std::set <int>					m_setAvailableIndexes;
		std::set <int>					m_setAvailableGLIndexes;
		Dx9Pipeline						m_pipeline;
		std::set <LightRendererPtr>		m_setLightRenderers;
		IDirect3D9					*	m_pDirect3D;
		IDirect3DDevice9			*	m_pDevice;

	public:
		Dx9RenderSystem();
		virtual ~Dx9RenderSystem();

		virtual void						Initialise					();
		virtual void						Delete						();
		virtual void						Cleanup						();
		virtual void						RenderAmbientLight			( Colour const & p_clColour);
		virtual int							LockLight					();
		virtual void						UnlockLight					( int p_iIndex);
		virtual void						SetCurrentShaderProgram		( ShaderProgramBase * p_pVal);
		virtual void						DrawIndexedPrimitives		( ePRIMITIVE_TYPE p_eType, size_t p_uiMinVertex, size_t p_uiVertexCount, size_t p_uiFirstIndex, size_t p_uiCount);
		virtual void						BeginOverlaysRendering		();
		void								CheckShaderSupport			();

		void								Resize						( Point2i const & p_ptNewSize);
		void								InitialiseDevice			( HWND p_hWnd, D3DPRESENT_PARAMETERS * p_presentParameters);

		inline static Dx9RenderSystem	*	GetSingletonPtr				()		{ return RenderSystem::_getSingletonPtr<Dx9RenderSystem>(); }
		inline static IDirect3D9		*	GetD3dObject				()		{ return GetSingletonPtr()->m_pDirect3D; }
		inline static IDirect3DDevice9	*	GetDevice					()		{ return GetSingletonPtr()->m_pDevice; }
		inline Dx9Context				*	GetMainContext				()const	{ return _getMainContext<Dx9Context>(); }
		inline Dx9Context				*	GetCurrentContext			()const	{ return _getCurrentContext<Dx9Context>(); }
		inline Dx9ShaderProgram			*	GetCurrentShaderProgram		()const	{ return _getCurrentShaderProgram<Dx9ShaderProgram>(); }

	private:
		void								_isCgProfileSupported		( eSHADER_TYPE p_eType, char const * p_szName);
		void								_checkCgAvailableProfiles	();
		virtual GlslShaderProgramPtr		_createGlslShaderProgram	() { return GlslShaderProgramPtr(); }
		virtual CgShaderProgramPtr			_createCgShaderProgram		();
		virtual HlslShaderProgramPtr		_createHlslShaderProgram	();

		virtual SubmeshRendererPtr			_createSubmeshRenderer		();
		virtual TextureRendererPtr			_createTextureRenderer		();
		virtual PassRendererPtr				_createPassRenderer			();
		virtual CameraRendererPtr			_createCameraRenderer		();
		virtual LightRendererPtr			_createLightRenderer		();
		virtual WindowRendererPtr			_createWindowRenderer		();
		virtual OverlayRendererPtr			_createOverlayRenderer		();

		virtual IndexBufferPtr				_createIndexBuffer			();
		virtual VertexBufferPtr				_createVertexBuffer			( const BufferElementDeclaration * p_pElements, size_t p_uiNbElements);
		virtual TextureBufferObjectPtr		_createTextureBuffer		();
	};
}

#endif
