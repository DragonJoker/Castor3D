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
#ifndef ___Dx9_RenderSystem___
#define ___Dx9_RenderSystem___

#include "Module_Dx9Render.h"
#include "Dx9Pipeline.h"
#include "Dx9Buffer.h"
#include "Dx9Context.h"
#include "Dx9ShaderProgram.h"

namespace Castor3D
{
	class C3D_Dx9_API Dx9RenderSystem : public RenderSystem
	{
	protected:
		std::set <int>					m_setAvailableIndexes;
		std::set <int>					m_setAvailableGLIndexes;
		Dx9Pipeline						m_pipeline;
		std::set <LightRendererPtr>		m_setLightRenderers;
		IDirect3D9					*	m_pDirect3D;
		IDirect3DDevice9			*	m_pDevice;

	public:
		Dx9RenderSystem( SceneManager * p_pSceneManager);
		virtual ~Dx9RenderSystem();

		virtual void Initialise();
		virtual void Delete();
		virtual void Cleanup();
		virtual void RenderAmbientLight( const Colour & p_clColour);
		virtual int LockLight();
		virtual void UnlockLight( int p_iIndex);
		virtual void SetCurrentShaderProgram( ShaderProgramBase * p_pVal);
		virtual void DrawIndexedPrimitives( ePRIMITIVE_TYPE p_eType, size_t p_uiMinVertex, size_t p_uiVertexCount, size_t p_uiFirstIndex, size_t p_uiCount);
		virtual void BeginOverlaysRendering();

		void InitialiseDevice( HWND p_hWnd, D3DPRESENT_PARAMETERS * p_presentParameters);

		inline static Dx9RenderSystem	*	GetSingletonPtr			()		{ return RenderSystem::_getSingletonPtr<Dx9RenderSystem>(); }
		inline static IDirect3D9		*	GetD3dObject			()		{ return GetSingletonPtr()->m_pDirect3D; }
		inline static IDirect3DDevice9	*	GetDevice				()		{ return GetSingletonPtr()->m_pDevice; }
		inline Dx9Context				*	GetMainContext			()const	{ return _getMainContext<Dx9Context>(); }
		inline Dx9Context				*	GetCurrentContext		()const	{ return _getCurrentContext<Dx9Context>(); }
		inline Dx9ShaderProgram			*	GetCurrentShaderProgram	()const	{ return _getCurrentShaderProgram<Dx9ShaderProgram>(); }

	private:
		virtual HlslShaderObjectPtr		_createHlslVertexShader		();
		virtual HlslShaderObjectPtr		_createHlslFragmentShader	();
		virtual HlslShaderObjectPtr		_createHlslGeometryShader	();
		virtual CgShaderObjectPtr		_createCgVertexShader		();
		virtual CgShaderObjectPtr		_createCgFragmentShader		();
		virtual CgShaderObjectPtr		_createCgGeometryShader		();

		virtual GlslShaderObjectPtr		_createGlslVertexShader		() { return GlslShaderObjectPtr(); }
		virtual GlslShaderObjectPtr		_createGlslFragmentShader	() { return GlslShaderObjectPtr(); }
		virtual GlslShaderObjectPtr		_createGlslGeometryShader	() { return GlslShaderObjectPtr(); }
		virtual GlslShaderProgramPtr	_createGlslShaderProgram	( const String & p_vertexShaderFile, const String & p_fragmentShaderFile, const String & p_geometryShaderFile) { return GlslShaderProgramPtr(); }

		virtual CgShaderProgramPtr		_createCgShaderProgram		( const String & p_vertexShaderFile, const String & p_fragmentShaderFile, const String & p_geometryShaderFile);
		virtual HlslShaderProgramPtr	_createHlslShaderProgram	( const String & p_vertexShaderFile, const String & p_fragmentShaderFile, const String & p_geometryShaderFile);

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

		virtual SubmeshRendererPtr				_createSubmeshRenderer	();
		virtual TextureEnvironmentRendererPtr	_createTexEnvRenderer	();
		virtual TextureRendererPtr				_createTextureRenderer	();
		virtual PassRendererPtr					_createPassRenderer		();
		virtual CameraRendererPtr				_createCameraRenderer	();
		virtual LightRendererPtr				_createLightRenderer	();
		virtual WindowRendererPtr				_createWindowRenderer	();
		virtual OverlayRendererPtr				_createOverlayRenderer	();

		virtual IndexBufferPtr			_createIndexBuffer		();
		virtual VertexBufferPtr			_createVertexBuffer		( const BufferElementDeclaration * p_pElements, size_t p_uiNbElements);
		virtual TextureBufferObjectPtr	_createTextureBuffer	();

		template <typename T, size_t Count> shared_ptr< VertexAttribsBuffer<T> > _createAttribsBuffer( const String & p_strArg)
		{
			shared_ptr< VertexAttribsBuffer<T> > l_pReturn;
/*
			if (UseShaders())
			{
				l_pReturn = BufferManager::CreateBuffer< T, Dx9VertexAttribsBuffer<T, Count> >( p_strArg);
			}
*/
			return l_pReturn;
		}
	};
}

#endif
