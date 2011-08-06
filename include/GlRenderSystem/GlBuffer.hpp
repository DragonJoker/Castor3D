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
#include "OpenGl.hpp"

#define BUFFER_OFFSET( n) ((unsigned char *)NULL + ( n))

namespace Castor3D
{
	//! Vertex buffers base class
	/*!
	Holds the Gl variable type for the target buffer
	\author Sylvain DOREMUS
	\version 0.6.1.0
	\date 03/01/2011
	*/
	class GlBufferBase
	{
	private:
		GLuint	m_uiGlIndex;
		GLenum	m_target;

	public:
		GlBufferBase( GLenum p_target);
		virtual ~GlBufferBase();

		bool Create();
		bool Cleanup();
		bool Initialise( void * p_pBuffer, GLsizeiptr p_iSize, eBUFFER_MODE p_eMode);

		bool Bind();
		bool Unbind();
		bool Fill( void * p_pBuffer, GLsizeiptr p_iSize, GLenum p_mode);
		void * Lock( size_t p_uiOffset, size_t p_uiSize, size_t p_uiFlags);
		void * Lock( GLenum p_access);
		bool Unlock();

		inline GLuint GetGlIndex()const	{ return m_uiGlIndex; }
	};

	//! Vertex Buffer Object class
	/*!
	Implements OpenGl VBO
	\author Sylvain DOREMUS
	\version 0.6.1.0
	\date 03/01/2011
	*/
	template <typename T>
	class GlBuffer
	{
	private:
		T				*&	m_pBuffer;
		unsigned int	&	m_uiFilled;
		GlBufferBase		m_glBuffer;

	public:
		GlBuffer( unsigned int & p_uiFilled, T *& p_pBuffer, GLenum p_target);
		virtual ~GlBuffer();

		void BufferObjectCleanup( bool & p_bAssigned, Buffer3D<T> * p_pBuffer);
		bool BufferObjectInitialise( eBUFFER_MODE p_eMode, Buffer3D<T> * p_pBuffer);
		bool BufferObjectBind();
		bool BufferObjectUnbind();
		bool BufferObjectFill( void * p_pBuffer, GLsizeiptr p_iSize, GLenum p_mode);
		void * BufferObjectLock( size_t p_uiOffset, size_t p_uiSize, size_t p_uiFlags);
		void * BufferObjectLock( GLenum p_access);
		bool BufferObjectUnlock();

		inline GLuint				GetGlIndex	()const	{ return m_glBuffer.GetGlIndex(); }
	};

	//! Vertex attribute base class
	/*!
	Holds all vertex attribute variables : name, location in shader program, count
	\author Sylvain DOREMUS
	\version 0.6.1.0
	\date 03/01/2011
	*/
	class GlAttributeBase
	{
	protected:
		weak_ptr<ShaderProgramBase>	m_pProgram;
		String						m_strAttribName;
		GLuint						m_uiAttribLocation;
		GLint						m_iCount;
		size_t						m_uiOffset;
		GLsizei						m_iStride;
		GLenum						m_glType;

	public:
		GlAttributeBase( String const & p_strAttribName, GLenum p_glType, GLint p_iCount);
		virtual ~GlAttributeBase();

		virtual void SetShader( ShaderProgramPtr p_pProgram);
		virtual void Cleanup();
		virtual bool Initialise();
		virtual void Activate( GLboolean p_bNormalised = false);
		virtual void Deactivate();

		inline void SetOffset( size_t p_uiOffset) { m_uiOffset = p_uiOffset; }
		inline void SetStride( GLsizei p_iStride) { m_iStride = p_iStride; }

		inline GLuint GetLocation()const { return m_uiAttribLocation; }
	};

	//! Template vertex attribute class
	/*!
	Derivated from GlAttribsBase, allows to specify attribute type and count
	\author Sylvain DOREMUS
	\version 0.6.1.0
	\date 03/01/2011
	*/
	template <typename T, size_t Count>
	class GlAttribute : public GlAttributeBase
	{
	public:
		GlAttribute( String const & p_strAttribName);
		virtual ~GlAttribute();
	};
	

	//**************************************************************************
	// Vertex Arrays
	//**************************************************************************

	class GlIndexArray : public IndexBuffer
	{
	public:
		GlIndexArray();
		virtual ~GlIndexArray();

		virtual void Cleanup();
		virtual void Activate();
		virtual void Deactivate();
		virtual void * Lock( size_t p_uiOffset, size_t p_uiSize, size_t p_uiFlags) { return m_buffer; }
		virtual void Unlock(){}
	};

	class GlVertexArray : public VertexBuffer
	{
	private:
		const BufferDeclaration & m_declaration;

	public:
		GlVertexArray( const BufferElementDeclaration * p_pElements, size_t p_uiCount);
		virtual ~GlVertexArray();

		virtual void Cleanup();
		virtual void Activate();
		virtual void Deactivate();
		virtual void * Lock( size_t p_uiOffset, size_t p_uiSize, size_t p_uiFlags) { return m_buffer; }
		virtual void Unlock(){}
	};
	

	//**************************************************************************
	// Vertex Buffer Objects
	//**************************************************************************

	class GlIndexBufferObject : public IndexBuffer, public GlBuffer<unsigned int>
	{
	public:
		GlIndexBufferObject();
		virtual ~GlIndexBufferObject();

		virtual void Cleanup();
		virtual void Initialise( eBUFFER_MODE p_eMode);
		virtual void Activate();
		virtual void Deactivate();
		virtual void * Lock( size_t p_uiOffset, size_t p_uiSize, size_t p_uiFlags);
		virtual void Unlock();

		void CleanupBuffer();
	};

	class GlVertexBufferObject : public VertexBuffer, public GlBuffer<real>
	{
	protected:
		const BufferDeclaration			&	m_declaration;
		std::vector <GlAttributePtr>		m_arrayAttributes;
		GLuint								m_uiValid;
		weak_ptr<GlShaderProgram>			m_pProgram;

	public:
		GlVertexBufferObject( const BufferElementDeclaration * p_pElements, size_t uiNbElements);
		virtual ~GlVertexBufferObject();

		virtual void Cleanup();
		virtual void Initialise( eBUFFER_MODE p_eMode);
		virtual void Activate();
		virtual void Deactivate();
		virtual void SetShaderProgram( ShaderProgramPtr p_pProgram);
		virtual void * Lock( size_t p_uiOffset, size_t p_uiSize, size_t p_uiFlags);
		virtual void Unlock();

		void CleanupBuffer();

	protected:
		virtual void AttributeCleanup();
		virtual bool AttributeInitialise();
		virtual void AttributeActivate();
		virtual void AttributeDeactivate();
	};

	class Gl3VertexBufferObject : public GlVertexBufferObject
	{
	public:
		Gl3VertexBufferObject( const BufferElementDeclaration * p_pElements, size_t uiNbElements);
		virtual ~Gl3VertexBufferObject();

		virtual void Activate();
		virtual void Deactivate();
	};

	//**************************************************************************
	// Pixel Buffer Objects
	//**************************************************************************

	class GlPixelBuffer : public GlBufferBase
	{
	protected:
		GLenum				m_packMode;
		GLenum				m_drawMode;
		unsigned char	*	m_pPixels;
		size_t				m_uiPixelsSize;

	public:
		GlPixelBuffer( unsigned char * p_pPixels, size_t p_uiPixelsSize, GLenum p_packMode, GLenum p_drawMode);
		virtual ~GlPixelBuffer();

		virtual bool Activate();
		virtual void Deactivate();
		bool Fill( void * p_pBuffer, GLsizeiptr p_iSize);
	};

	class GlPackPixelBuffer : public GlPixelBuffer
	{
	public:
		GlPackPixelBuffer( unsigned char * p_pPixels, size_t p_uiPixelsSize);
		virtual ~GlPackPixelBuffer();

		virtual void Initialise();
	};

	class GlUnpackPixelBuffer : public GlPixelBuffer
	{
	public:
		GlUnpackPixelBuffer( unsigned char * p_pPixels, size_t p_uiPixelsSize);
		virtual ~GlUnpackPixelBuffer();

		virtual void Initialise();
	};

	//**************************************************************************
	// Uniform Buffer Objects
	//**************************************************************************

	class GlUniformBufferObject : public Buffer3D<unsigned char>
	{
	public:
		GlShaderProgram			*	m_pProgram;
		GlBufferBase				m_glBuffer;
		int							m_iUniformBlockIndex;
		int							m_iUniformBlockSize;
		String						m_strUniformBlockName;
		GlUboVariablePtrStrMap		m_mapVariables;
		bool						m_bChanged;

	public:
		GlUniformBufferObject( String const & p_strUniformBlockName);
		virtual ~GlUniformBufferObject();

		virtual void Cleanup();
		virtual void Initialise( GlShaderProgram * p_pProgram);
		virtual void Activate();
		virtual void Deactivate();
		virtual void * Lock( size_t p_uiOffset, size_t p_uiSize, size_t p_uiFlags);
		virtual void Unlock();
		bool AddVariable( GlUboVariablePtr p_pVariable);
		template <typename T> shared_ptr<T> CreateVariable( String const & p_strName, size_t p_uiCount);
		template <typename T> shared_ptr<T> GetVariable( String const & p_strName);
		inline void	SetModified	( bool p_bChanged) { m_bChanged = p_bChanged; }
		inline bool	IsModified	()const { return m_bChanged; }
	};

	class GlUboVariableBase
	{
		friend class GlUniformBufferObject;

	protected:
		GLuint						m_uiIndex;
		int							m_iOffset;
		int							m_iSize;
		void					*	m_pBuffer;
		bool						m_bChanged;
		size_t						m_uiCount;
		size_t						m_uiElemSize;
		String						m_strName;
		GlUniformBufferObject	*	m_pParent;

	public:
		GlUboVariableBase( GlUniformBufferObject * p_pParent, String const & p_strName, size_t p_uiElemSize, size_t p_uiCount);
		virtual ~GlUboVariableBase();

		void Activate();
		void Deactivate();

		inline size_t		GetSize		()const { return m_uiElemSize * m_uiCount; }
		inline void *		GetBuffer	()		{ return m_pBuffer; }
		inline void const *	GetBuffer	()const	{ return m_pBuffer; }

		inline void SetBuffer( void * p_pBuffer) { m_pBuffer = p_pBuffer; }
	};

	template <typename T>
	class GlUboOneVariable : public GlUboVariableBase, public MemoryTraced< GlUboOneVariable<T> >
	{
		friend class GlUniformBufferObject;

	private:
		T ** m_tValue;

	public:
		GlUboOneVariable( GlUniformBufferObject * p_pParent, String const & p_strName, size_t p_uiCount);
		virtual ~GlUboOneVariable();

		void SetValue( T const & p_tValue, size_t p_uiIndex);
		void SetValues( T * p_tValues);
		void SetBuffer( void * p_pBuffer);

		inline T const &		GetValue	( size_t p_uiIndex)const	{ return m_tValue[p_uiIndex]; }
		inline T &				GetValue	( size_t p_uiIndex)			{ return m_tValue[p_uiIndex]; }
	};

	template <typename T, size_t Count>
	class GlUboPointVariable : public GlUboVariableBase, public MemoryTraced< GlUboPointVariable<T, Count> >
	{
		friend class GlUniformBufferObject;

	private:
		Point<T, Count> * m_ptValue;

	public:
		GlUboPointVariable( GlUniformBufferObject * p_pParent, String const & p_strName, size_t p_uiCount);
		virtual ~GlUboPointVariable();

		void SetValue( Point<T, Count> const & p_ptValue, size_t p_uiIndex);
		void SetValues( Point<T, Count> * p_ptValues);
		void SetBuffer( void * p_pBuffer);

		inline Point<T, Count> const &	GetValue	( size_t p_uiIndex)const	{ return m_ptValue[p_uiIndex]; }
		inline Point<T, Count> &		GetValue	( size_t p_uiIndex)			{ return m_ptValue[p_uiIndex]; }
	};

	template <typename T, size_t Rows, size_t Columns>
	class GlUboMatrixVariable : public GlUboVariableBase, public MemoryTraced< GlUboMatrixVariable<T, Rows, Columns> >
	{
		friend class GlUniformBufferObject;

	private:
		Matrix<T, Rows, Columns> * m_mtxValue;

	public:
		GlUboMatrixVariable( GlUniformBufferObject * p_pParent, String const & p_strName, size_t p_uiCount);
		virtual ~GlUboMatrixVariable();

		void SetValue( Matrix<T, Rows, Columns> const & p_mtxValue, size_t p_uiIndex);
		void SetValues( Matrix<T, Rows, Columns> * p_mtxValues);
		void SetBuffer( void * p_pBuffer);

		inline Matrix<T, Rows, Columns> const & GetValue	( size_t p_uiIndex)const	{ return m_mtxValue[p_uiIndex]; }
		inline Matrix<T, Rows, Columns> &		GetValue	( size_t p_uiIndex)			{ return m_mtxValue[p_uiIndex]; }
	};

	//**************************************************************************
	// Vertex Array Object
	//**************************************************************************

	class GlVertexArrayObjects
	{
	private:
		GLuint m_uiIndex;

	public:
		GlVertexArrayObjects();
		~GlVertexArrayObjects();

		void Cleanup();
		void Initialise();
		void Activate();
		void Deactivate();

		inline GLuint	GetIndex	()const { return m_uiIndex; }
	};

	//**************************************************************************
	// Texture Buffer Object
	//**************************************************************************

	class GlTextureBufferObject : public TextureBufferObject
	{
	public:
		unsigned int m_uiIndex;

	public:
		GlTextureBufferObject();
		virtual ~GlTextureBufferObject();

		virtual void Cleanup();
		virtual void Initialise( const Castor::Resources::ePIXEL_FORMAT & p_format, size_t p_uiSize, unsigned char const * p_pBytes);
		virtual void Activate();
		virtual void Activate( PassPtr p_pPass);
		virtual void Deactivate();
		virtual void * Lock( size_t p_uiOffset, size_t p_uiSize, size_t p_uiFlags);
		virtual void Unlock();

		inline unsigned int	GetIndex	()const { return m_uiIndex; }
	};

#	include "GlBuffer.inl"
}

#endif
