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
#ifndef ___Dx11_Buffer___
#define ___Dx11_Buffer___

#include "Module_DxRender.hpp"
#include "DxShaderProgram.hpp"

namespace Dx11Render
{
	//**************************************************************************
	// Vertex Buffer Objects
	//**************************************************************************

	class DxBufferObjectBase
	{
	protected:
		static uint32_t BuffersCount;

	public:
		DxBufferObjectBase()
		{
			BuffersCount++;
		}
		virtual ~DxBufferObjectBase()
		{
		}
	};

	template< typename T, class D3dBufferObject >
	class DxBufferObject : public DxBufferObjectBase, public Castor3D::GpuBuffer< T >
	{
	private:
		typedef std::shared_ptr< Castor3D::CpuBuffer< T > > HardwareBufferPtr;

	protected:
		D3dBufferObject	*	m_pBufferObject;
		HardwareBufferPtr	m_pBuffer;
		uint32_t			m_uiIndex;

	public:
		DxBufferObject( HardwareBufferPtr p_pBuffer );
		virtual ~DxBufferObject();
		
		virtual bool Fill( T const * p_pBuffer, ptrdiff_t p_iSize, Castor3D::eBUFFER_ACCESS_TYPE p_eType, Castor3D::eBUFFER_ACCESS_NATURE p_eNature );

		HardwareBufferPtr	GetCpuBuffer	()const	{ return m_pBuffer; }

	protected:
		void	DoDestroy	();
		void	DoCleanup	();
		T *		DoLock		( uint32_t p_uiOffset, uint32_t p_uiSize, uint32_t p_uiFlags );
		void	DoUnlock	();
	};

	class DxIndexBuffer : public DxBufferObject< uint32_t, ID3D11Buffer >
	{
	private:
		typedef std::shared_ptr< Castor3D::CpuBuffer< uint32_t > > HardwareBufferPtr;

	public:
		DxIndexBuffer( HardwareBufferPtr p_pBuffer);
		virtual ~DxIndexBuffer();

		virtual bool		Create		();
		virtual void		Destroy		();
		virtual bool		Initialise	( Castor3D::eBUFFER_ACCESS_TYPE p_eType, Castor3D::eBUFFER_ACCESS_NATURE p_eNature, Castor3D::ShaderProgramBaseSPtr p_pProgram );
		virtual void		Cleanup		();
		virtual bool		Bind		();
		virtual void		Unbind		();
		virtual uint32_t *	Lock		( uint32_t p_uiOffset, uint32_t p_uiCount, uint32_t p_uiFlags );
		virtual void		Unlock		();
	};

	class DxVertexBuffer : public DxBufferObject< uint8_t, ID3D11Buffer >, CuNonCopyable
	{
	private:
		typedef std::shared_ptr< Castor3D::CpuBuffer< uint8_t > > HardwareBufferPtr;
	
		ID3D11InputLayout *					m_pDxDeclaration;
		Castor3D::BufferDeclaration const &	m_declaration;
		DxShaderProgramWPtr					m_pProgram;

	public:
		DxVertexBuffer( Castor3D::BufferDeclaration const & p_declaration, HardwareBufferPtr p_pBuffer);
		virtual ~DxVertexBuffer();

		virtual bool		Create		();
		virtual void		Destroy		();
		virtual bool		Initialise	( Castor3D::eBUFFER_ACCESS_TYPE p_eType, Castor3D::eBUFFER_ACCESS_NATURE p_eNature, Castor3D::ShaderProgramBaseSPtr p_pProgram );
		virtual void		Cleanup		();
		virtual bool		Bind		();
		virtual void		Unbind		();
		virtual uint8_t *	Lock		( uint32_t p_uiOffset, uint32_t p_uiCount, uint32_t p_uiFlags );
		virtual void		Unlock		();

	private:
		bool DoCreateLayout();
		bool DoCreateBuffer( Castor3D::eBUFFER_ACCESS_TYPE p_eType, Castor3D::eBUFFER_ACCESS_NATURE p_eNature );
	};

	class DxMatrixBuffer : public DxBufferObject< real, ID3D11Buffer >
	{
	private:
		typedef std::shared_ptr< Castor3D::CpuBuffer< real > > HardwareBufferPtr;
		ID3D11InputLayout *		m_pDxDeclaration;
		DxShaderProgramWPtr		m_pProgram;

	public:
		DxMatrixBuffer( HardwareBufferPtr p_pBuffer);
		virtual ~DxMatrixBuffer();

		virtual bool		Create		();
		virtual void		Destroy		();
		virtual bool		Initialise	( Castor3D::eBUFFER_ACCESS_TYPE p_eType, Castor3D::eBUFFER_ACCESS_NATURE p_eNature, Castor3D::ShaderProgramBaseSPtr p_pProgram );
		virtual void		Cleanup		();
		virtual bool		Bind		( uint32_t p_uiCount );
		virtual void		Unbind		();
		virtual real *		Lock		( uint32_t p_uiOffset, uint32_t p_uiCount, uint32_t p_uiFlags );
		virtual void		Unlock		();

		private:
			virtual bool Bind() { return false; }
	};

	class DxGeometryBuffers : public Castor3D::GeometryBuffers
	{
	private:
		DxRenderSystem * m_pRenderSystem;

	public:
		DxGeometryBuffers( DxRenderSystem * p_pRenderSystem, Castor3D::VertexBufferSPtr p_pVertexBuffer, Castor3D::IndexBufferSPtr p_pIndexBuffer, Castor3D::MatrixBufferSPtr p_pMatrixBuffer );
		virtual ~DxGeometryBuffers();

		virtual bool Draw( Castor3D::eTOPOLOGY p_ePrimitiveType, Castor3D::ShaderProgramBaseSPtr p_pProgram, uint32_t p_uiSize, uint32_t p_uiIndex );
		virtual bool DrawInstanced( Castor3D::eTOPOLOGY p_ePrimitiveType, Castor3D::ShaderProgramBaseSPtr p_pProgram, uint32_t p_uiSize, uint32_t p_uiIndex, uint32_t p_uiCount );
		virtual bool Initialise();
		virtual void Cleanup();
	};

#	include "DxBuffer.inl"
}

#endif
