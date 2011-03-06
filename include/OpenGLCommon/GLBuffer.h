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
#ifndef ___Gl_Buffer___
#define ___Gl_Buffer___

#include "Module_GlRender.h"
#include "GlShaderProgram.h"

#define BUFFER_OFFSET( n) ((char*)NULL + ( n))

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
	protected:
		int m_iGlType;

	public:
		GlBufferBase()
		{
		}
		virtual ~GlBufferBase()
		{
		}

	protected:
		template <typename T> int _getGlType();
	};

	//! Vertex attribute base class
	/*!
	Holds all vertex attribute variables : name, location in shader program, count
	\author Sylvain DOREMUS
	\version 0.6.1.0
	\date 03/01/2011
	*/
	class GlAttribsBase : public GlBufferBase
	{
	protected:
		weak_ptr<GlShaderProgram> m_pProgram;
		String m_strAttribName;
		int m_iAttribLocation;
		size_t m_uiCount;
		size_t m_uiOffset;
		size_t m_uiStride;

	public:
		GlAttribsBase( const String & p_strAttribName);
		virtual ~GlAttribsBase();

		virtual void SetShaderProgram( ShaderProgramPtr p_pProgram);

		inline void SetOffset( size_t p_uiOffset) { m_uiOffset = p_uiOffset; }
		inline void SetStride( size_t p_uiStride) { m_uiStride = p_uiStride; }
		int GetAttribLocation()const { return m_iAttribLocation; }

		virtual void CleanupAttribute();
		virtual bool InitialiseAttribute();
		virtual void ActivateAttribute( bool p_bNormalised = false);
		virtual void DeactivateAttribute();
	};

	//! Template vertex attribute class
	/*!
	Derivated from GlAttribsBase, allows to specify attribute type and count
	\author Sylvain DOREMUS
	\version 0.6.1.0
	\date 03/01/2011
	*/
	template <typename T, size_t Count>
	class GlVertexAttribs : public GlAttribsBase
	{
	public:
		GlVertexAttribs( const String & p_strAttribName);
		virtual ~GlVertexAttribs();
	};

	//! Vertex Buffer Object class
	/*!
	Implements OpenGl VBO
	\author Sylvain DOREMUS
	\version 0.6.1.0
	\date 03/01/2011
	*/
	template <typename T>
	class GlVertexBufferObject : public GlBufferBase
	{
	private:
		T *& m_pBuffer;
		unsigned int & m_uiFilled;
		const BufferDeclaration & m_declaration;

	protected:
		unsigned int m_uiIndex;

	public:
		GlVertexBufferObject( unsigned int & p_uiFilled, T *& p_pBuffer, const BufferDeclaration & p_declaration);
		virtual ~GlVertexBufferObject();
		virtual void * Lock( size_t p_uiOffset, size_t p_uiSize, size_t p_uiFlags);
		virtual void Unlock();

	protected:
		virtual void _cleanupBufferObject();
		virtual void _initialiseBufferObject( eBUFFER_MODE p_eMode);
		virtual void _activateBufferObject();
		virtual void _deactivateBufferObject();
	};

	template <typename T, size_t Count>
	class GlVertexAttribsBuffer : public VertexAttribsBuffer<T>, public GlVertexAttribs<T, Count>
	{
	private:
		T *& m_pBuffer;
		unsigned int & m_uiFilled;

	protected:
		unsigned int m_uiIndex;

	public:
		GlVertexAttribsBuffer( const String & p_strAttribName);
		virtual ~GlVertexAttribsBuffer();

		virtual void Cleanup();
		virtual void Initialise( eBUFFER_MODE p_eMode);
		virtual void Activate();
		virtual void Deactivate();
		virtual void SetShaderProgram( ShaderProgramPtr p_pProgram);
		virtual void * Lock( size_t p_uiOffset, size_t p_uiSize, size_t p_uiFlags);
		virtual void Unlock();

	protected:
		virtual void _cleanupBufferObject();
		virtual void _initialiseBufferObject( eBUFFER_MODE p_eMode);
		virtual void _activateBufferObject();
		virtual void _deactivateBufferObject();
	};

	class GlIndexBuffer : public IndexBuffer
	{
	public:
		GlIndexBuffer();
		virtual ~GlIndexBuffer();
	};

	class GlVertexBuffer : public VertexBuffer
	{
	public:
		GlVertexBuffer( const BufferElementDeclaration * p_pElements, size_t uiNbElements);
		virtual ~GlVertexBuffer();
	};

	class GlVboIndexBuffer : public GlIndexBuffer
	{
	private:
		unsigned int *& m_pBuffer;
		unsigned int & m_uiFilled;

	protected:
		unsigned int m_uiIndex;

	public:
		GlVboIndexBuffer();
		virtual ~GlVboIndexBuffer();

		virtual void Cleanup();
		virtual void Initialise( eBUFFER_MODE p_eMode);
		virtual void Activate();
		virtual void Deactivate();
		virtual void * Lock( size_t p_uiOffset, size_t p_uiSize, size_t p_uiFlags);
		virtual void Unlock();
		virtual void CleanupVbo();

	protected:
		virtual void _cleanupBufferObject();
	};

	class GlVboVertexBuffer : public GlVertexBuffer, public GlVertexBufferObject<real>
	{
	protected:
		std::vector <GlVertexAttribsPtr> m_arrayAttribs;
		size_t m_uiValid;
		weak_ptr<GlShaderProgram> m_pProgram;

	public:
		GlVboVertexBuffer( const BufferElementDeclaration * p_pElements, size_t uiNbElements);
		virtual ~GlVboVertexBuffer();

		virtual void Cleanup();
		virtual void Initialise( eBUFFER_MODE p_eMode);
		virtual void Activate();
		virtual void Deactivate();
		virtual void SetShaderProgram( ShaderProgramPtr p_pProgram);
		virtual void * Lock( size_t p_uiOffset, size_t p_uiSize, size_t p_uiFlags);
		virtual void Unlock();
		virtual void CleanupVbo();

	protected:
		virtual void _cleanupBufferObject();
		virtual void _activateBufferObject();

		virtual void _cleanupAttribute();
		virtual bool _initialiseAttribute();
		virtual void _activateAttribute();
		virtual void _deactivateAttribute();
	};

#	include "GlBuffer.inl"
}

#endif
