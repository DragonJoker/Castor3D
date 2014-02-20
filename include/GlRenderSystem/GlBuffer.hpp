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
#ifndef ___Gl_Buffer___
#define ___Gl_Buffer___

#include "Module_GlRender.hpp"
#include "GlShaderProgram.hpp"

#include <Castor3D/Buffer.hpp>

#define BUFFER_OFFSET( n) ((uint8_t *)NULL + ( n))

namespace GlRender
{ 
	/*!
	\author		Sylvain DOREMUS
	\version	0.6.1.0
	\date		03/01/2011
	\~english
	\brief		OpenGL buffer objects base class
	\~french
	\brief		Classe de base des buffers objects OpenGL
	*/
	template< typename T >
	class GlBufferBase : CuNonCopyable
	{
	private:
		uint32_t			m_uiGlIndex;
		eGL_BUFFER_TARGET	m_eTarget;

	protected:
		OpenGl &			m_gl;

	public:
		GlBufferBase( OpenGl & p_gl, eGL_BUFFER_TARGET p_eTarget);
		virtual ~GlBufferBase();

		bool	Create		();
		void	Destroy		();
		bool	Initialise	( T const * p_pBuffer, ptrdiff_t p_iSize, Castor3D::eBUFFER_ACCESS_TYPE p_eType, Castor3D::eBUFFER_ACCESS_NATURE p_eNature );
		void	Cleanup		();
		bool	Bind		();
		void	Unbind		();
		bool	Fill		( T const * p_pBuffer, ptrdiff_t p_iSize, Castor3D::eBUFFER_ACCESS_TYPE p_eType, Castor3D::eBUFFER_ACCESS_NATURE p_eNature );
		T *		Lock		( uint32_t p_uiOffset, uint32_t p_uiCount, uint32_t p_uiFlags );
		T *		Lock		( eGL_LOCK p_access );
		bool	Unlock		();

		inline uint32_t GetGlIndex()const { return m_uiGlIndex; }
	};

	//! Vertex Buffer Object class
	/*!
	Implements OpenGl VBO
	\author Sylvain DOREMUS
	\version 0.6.1.0
	\date 03/01/2011
	*/
	template< typename T >
	class GlBuffer : public Castor3D::GpuBuffer< T >
	{
	protected:
		typedef std::weak_ptr< Castor3D::CpuBuffer< T > >	HardwareBufferWPtr;
		typedef std::shared_ptr< Castor3D::CpuBuffer< T > >	HardwareBufferSPtr;

		GlBufferBase< T >	m_glBuffer;
		HardwareBufferWPtr	m_wpBuffer;
		OpenGl &			m_gl;

	public:
		GlBuffer( OpenGl & p_gl, eGL_BUFFER_TARGET p_eTarget, HardwareBufferSPtr p_pBuffer );
		virtual ~GlBuffer();
		
		virtual bool Fill( T const * p_pBuffer, ptrdiff_t p_iSize, Castor3D::eBUFFER_ACCESS_TYPE p_eType, Castor3D::eBUFFER_ACCESS_NATURE p_eNature );

		inline uint32_t GetGlIndex()const { return m_glBuffer.GetGlIndex(); }
		HardwareBufferSPtr GetCpuBuffer()const { return m_wpBuffer.lock(); }

	protected:
		virtual bool	DoCreate		();
		virtual void	DoDestroy		();
		virtual bool	DoInitialise	( Castor3D::eBUFFER_ACCESS_TYPE p_eType, Castor3D::eBUFFER_ACCESS_NATURE p_eNature );
		virtual void	DoCleanup		();
		virtual bool	DoBind			();
		virtual void	DoUnbind		();
		virtual bool	DoFill			( T * p_pBuffer, ptrdiff_t p_iSize, Castor3D::eBUFFER_ACCESS_TYPE p_eType, Castor3D::eBUFFER_ACCESS_NATURE p_eNature );
		virtual T *		DoLock			( uint32_t p_uiOffset, uint32_t p_uiCount, uint32_t p_uiFlags );
		virtual T *		DoLock			( eGL_LOCK p_access );
		virtual void	DoUnlock		();
	};

	//! Vertex attribute base class
	/*!
	Holds all vertex attribute variables : name, location in shader program, count
	\author Sylvain DOREMUS
	\version 0.6.1.0
	\date 03/01/2011
	*/
	class GlAttributeBase : CuNonCopyable
	{
	protected:
		Castor3D::ShaderProgramBaseWPtr	m_pProgram;
		Castor::String					m_strAttribName;
		uint32_t						m_uiAttribLocation;
		int								m_iCount;
		uint32_t						m_uiOffset;
		int								m_iStride;
		eGL_TYPE						m_eGlType;
		GlRenderSystem *				m_pRenderSystem;
		OpenGl &						m_gl;

	public:
		GlAttributeBase( OpenGl & p_gl, GlRenderSystem * p_pRenderSystem, Castor::String const & p_strAttribName, eGL_TYPE p_eGlType, int p_iCount );
		virtual ~GlAttributeBase();

		virtual void SetShader	( Castor3D::ShaderProgramBaseSPtr p_pProgram );
		virtual void Cleanup	();
		virtual bool Initialise	();
		virtual bool Bind		( bool p_bNormalised = false );
		virtual void Unbind		();

		inline void SetOffset	( uint32_t p_uiOffset )		{ m_uiOffset = p_uiOffset; }
		inline void SetStride	( int p_iStride )			{ m_iStride = p_iStride; }

		inline uint32_t GetLocation()const { return m_uiAttribLocation; }
	};

	//! Template vertex attribute class
	/*!
	Derivated from GlAttribsBase, allows to specify attribute type and count
	\author Sylvain DOREMUS
	\version 0.6.1.0
	\date 03/01/2011
	*/
	template <typename T, uint32_t Count>
	class GlAttribute : public GlAttributeBase
	{
	public:
		GlAttribute( OpenGl & p_gl, GlRenderSystem * p_pRenderSystem, Castor::String const & p_strAttribName );
		virtual ~GlAttribute();
	};


	//**************************************************************************
	// Vertex Arrays
	//**************************************************************************

	class GlIndexArray : public Castor3D::GpuBuffer< uint32_t >, CuNonCopyable
	{
	private:
		typedef std::weak_ptr< Castor3D::CpuBuffer< uint32_t > >	HardwareBufferWPtr;
		typedef std::shared_ptr< Castor3D::CpuBuffer< uint32_t > >	HardwareBufferSPtr;

		HardwareBufferWPtr	m_wpBuffer;
		OpenGl &			m_gl;

	public:
		GlIndexArray( OpenGl & p_gl, HardwareBufferSPtr p_pBuffer );
		virtual ~GlIndexArray();

		virtual bool		Create		();
		virtual void		Destroy		();
		virtual bool		Initialise	( Castor3D::eBUFFER_ACCESS_TYPE p_eType, Castor3D::eBUFFER_ACCESS_NATURE p_eNature, Castor3D::ShaderProgramBaseSPtr p_pProgram );
		virtual void		Cleanup		();
		virtual uint32_t *	Lock		( uint32_t CU_PARAM_UNUSED( p_uiOffset ), uint32_t CU_PARAM_UNUSED( p_uiCount ), uint32_t CU_PARAM_UNUSED( p_uiFlags ) ) { return NULL; }
		virtual void		Unlock		(){}
		virtual bool		Bind		();
		virtual void		Unbind		();
		virtual bool		Fill		( uint32_t const * p_pBuffer, ptrdiff_t p_iSize, Castor3D::eBUFFER_ACCESS_TYPE p_eType, Castor3D::eBUFFER_ACCESS_NATURE p_eNature ) { return false; }

		HardwareBufferSPtr GetCpuBuffer()const	{ return m_wpBuffer.lock(); }
	};

	class GlVertexArray : public Castor3D::GpuBuffer< uint8_t >, CuNonCopyable
	{
	private:
		typedef std::weak_ptr< Castor3D::CpuBuffer< uint8_t > >		HardwareBufferWPtr;
		typedef std::shared_ptr< Castor3D::CpuBuffer< uint8_t > >	HardwareBufferSPtr;

		HardwareBufferWPtr					m_wpBuffer;
		Castor3D::BufferDeclaration const &	m_bufferDeclaration;
		OpenGl &							m_gl;

	public:
		GlVertexArray( OpenGl & p_gl, Castor3D::BufferDeclaration const & p_declaration, HardwareBufferSPtr p_pBuffer );
		virtual ~GlVertexArray();

		virtual bool		Create		();
		virtual void		Destroy		();
		virtual bool		Initialise	( Castor3D::eBUFFER_ACCESS_TYPE p_eType, Castor3D::eBUFFER_ACCESS_NATURE p_eNature, Castor3D::ShaderProgramBaseSPtr p_pProgram );
		virtual void		Cleanup		();
		virtual uint8_t *	Lock		( uint32_t CU_PARAM_UNUSED( p_uiOffset ), uint32_t CU_PARAM_UNUSED( p_uiCount ), uint32_t CU_PARAM_UNUSED( p_uiFlags ) ) { return NULL; }
		virtual void		Unlock		(){}
		virtual bool		Bind		();
		virtual void		Unbind		();
		virtual bool		Fill		( uint8_t const * p_pBuffer, ptrdiff_t p_iSize, Castor3D::eBUFFER_ACCESS_TYPE p_eType, Castor3D::eBUFFER_ACCESS_NATURE p_eNature ) { return false; }

		HardwareBufferSPtr GetCpuBuffer()const	{ return m_wpBuffer.lock(); }
	};

	//**************************************************************************
	// Vertex Buffer Objects
	//**************************************************************************

	class GlIndexBufferObject : public GlBuffer< uint32_t >, CuNonCopyable
	{
	public:
		GlIndexBufferObject( OpenGl & p_gl, HardwareBufferSPtr p_pBuffer );
		virtual ~GlIndexBufferObject();

		virtual bool		Create		();
		virtual void		Destroy		();
		virtual bool		Initialise	( Castor3D::eBUFFER_ACCESS_TYPE p_eType, Castor3D::eBUFFER_ACCESS_NATURE p_eNature, Castor3D::ShaderProgramBaseSPtr p_pProgram );
		virtual void		Cleanup		();
		virtual uint32_t *	Lock		( uint32_t p_uiOffset, uint32_t p_uiCount, uint32_t p_uiFlags );
		virtual void		Unlock		();
		virtual bool		Bind		();
		virtual void		Unbind		();
	};

	class GlVertexBufferObject : public GlBuffer< uint8_t >, CuNonCopyable
	{
	protected:
		Castor3D::BufferDeclaration const &	m_bufferDeclaration;
		GlAttributePtrArray					m_arrayAttributes;
		uint32_t							m_uiValid;
		GlShaderProgramWPtr					m_pProgram;

	public:
		GlVertexBufferObject( OpenGl & p_gl, Castor3D::BufferDeclaration const & p_declaration, HardwareBufferSPtr p_pBuffer );
		virtual ~GlVertexBufferObject();

		virtual bool		Create		();
		virtual void		Destroy		();
		virtual bool		Initialise	( Castor3D::eBUFFER_ACCESS_TYPE p_eType, Castor3D::eBUFFER_ACCESS_NATURE p_eNature, Castor3D::ShaderProgramBaseSPtr p_pProgram );
		virtual void		Cleanup		();
		virtual uint8_t *	Lock		( uint32_t p_uiOffset, uint32_t p_uiCount, uint32_t p_uiFlags );
		virtual void		Unlock		();
		virtual bool		Bind		();
		virtual void		Unbind		();

	protected:
		virtual void DoAttributesCleanup	();
		virtual bool DoAttributesInitialise	();
		virtual bool DoAttributesBind		();
		virtual void DoAttributesUnbind		();
	};

	class Gl3VertexBufferObject : public GlVertexBufferObject
	{
	public:
		Gl3VertexBufferObject( OpenGl & p_gl, Castor3D::BufferDeclaration const & p_declaration, HardwareBufferSPtr p_pBuffer );
		virtual ~Gl3VertexBufferObject();

		virtual bool	Bind		();
		virtual void	Unbind		();
	};

	class GlMatrixBufferObject : public GlBuffer< real >, CuNonCopyable
	{
	private:
		GlAttributeBaseSPtr	m_pAttribute;
		uint32_t			m_uiValid;
		GlShaderProgramWPtr	m_pProgram;

	public:
		GlMatrixBufferObject( OpenGl & p_gl, HardwareBufferSPtr p_pBuffer );
		virtual ~GlMatrixBufferObject();

		virtual bool		Create		();
		virtual void		Destroy		();
		virtual bool		Initialise	( Castor3D::eBUFFER_ACCESS_TYPE p_eType, Castor3D::eBUFFER_ACCESS_NATURE p_eNature, Castor3D::ShaderProgramBaseSPtr p_pProgram );
		virtual void		Cleanup		();
		virtual real *		Lock		( uint32_t p_uiOffset, uint32_t p_uiCount, uint32_t p_uiFlags );
		virtual void		Unlock		();
		virtual bool		Bind		( uint32_t p_uiCount );
		virtual void		Unbind		();

	protected:
		virtual bool Bind() { return false; }
		virtual void DoAttributeCleanup		();
		virtual bool DoAttributeInitialise	();
		virtual bool DoAttributeBind		( bool p_bInstanced );
		virtual void DoAttributeUnbind		();

	private:
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0.0
	\date		18/11/2012
	\~english
	\brief		Geometry buffers holder
	\remark		Allows implementations to use API specific optimisations (like OpenGL Vertex array objects)
	\~french
	\brief		Conteneur de buffers de géométries
	\remark		Permet aux implémentations d'utiliser les optimisations spécifiques aux API (comme les Vertex arrays objects OpenGL)
	*/
	class GlGeometryBuffers : public Castor3D::GeometryBuffers, CuNonCopyable
	{
	private:
		typedef std::function< bool() >	PFnBind;
		typedef std::function< void() >	PFnUnbind;

		uint32_t	m_uiIndex;
		PFnBind		m_pfnBind;
		PFnUnbind	m_pfnUnbind;
		OpenGl &	m_gl;

	public:
		GlGeometryBuffers( OpenGl & p_gl, Castor3D::VertexBufferSPtr p_pVertexBuffer, Castor3D::IndexBufferSPtr p_pIndexBuffer, Castor3D::MatrixBufferSPtr p_pMatrixBuffer );
		virtual ~GlGeometryBuffers();

		virtual bool Initialise();
		virtual void Cleanup();
		virtual bool Draw( Castor3D::eTOPOLOGY p_ePrimitiveType, Castor3D::ShaderProgramBaseSPtr p_pProgram, uint32_t p_uiSize, uint32_t p_uiIndex );
		virtual bool DrawInstanced( Castor3D::eTOPOLOGY p_ePrimitiveType, Castor3D::ShaderProgramBaseSPtr p_pProgram, uint32_t p_uiSize, uint32_t p_uiIndex, uint32_t p_uiCount );
		virtual bool Bind();
		virtual void Unbind();

		inline uint32_t	GetIndex	()const { return m_uiIndex; }
	};

	//**************************************************************************
	// Texture Buffer Objects
	//**************************************************************************

	class GlTextureBufferObject : public GlBuffer< uint8_t >, CuNonCopyable
	{
	public:
		GlTextureBufferObject( OpenGl & p_gl, HardwareBufferSPtr p_pBuffer );
		virtual ~GlTextureBufferObject();

		virtual bool		Create		();
		virtual void		Destroy		();
		virtual bool		Initialise	( Castor3D::eBUFFER_ACCESS_TYPE p_eType, Castor3D::eBUFFER_ACCESS_NATURE p_eNature, Castor3D::ShaderProgramBaseSPtr p_pProgram );
		virtual void		Cleanup		();
		virtual bool		Bind		();
		virtual void		Unbind		();
		virtual uint8_t *	Lock		( uint32_t p_uiOffset, uint32_t p_uiCount, uint32_t p_uiFlags);
		virtual void		Unlock		();
	};

#	include "GlBuffer.inl"
}

#endif
