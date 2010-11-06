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
#ifndef ___CS3D_GL_Buffer___
#define ___CS3D_GL_Buffer___

#include "Module_GLRender.h"
#include "GLRenderSystem.h"
#include "GLSLProgram.h"
#include <Castor3D/render_system/Buffer.h>

namespace Castor3D
{

	//**************************************************************************
	// Vertex Attribs
	//**************************************************************************

	template <typename T, size_t Count>
	class GLVertexAttribs
	{
	protected:
		String m_strAttribName;
		int m_iAttribLocation;
		GLShaderProgram * m_pProgram;
		int m_iType;

	public:
		GLVertexAttribs( const String & p_strAttribName)
			:	m_strAttribName( p_strAttribName),
				m_iAttribLocation( GL_INVALID_INDEX),
				m_pProgram( NULL)
		{
			m_iType = _getType();
		}
		virtual ~GLVertexAttribs()
		{
			CleanupAttribute();
		}
		virtual void CleanupAttribute()
		{
			m_iAttribLocation = GL_INVALID_INDEX;
			m_pProgram = NULL;
		}
		bool InitialiseAttribute()
		{
			bool l_bReturn = false;

			if (m_pProgram != NULL)
			{
				m_iAttribLocation = glGetAttribLocation( m_pProgram->GetProgramObject(), m_strAttribName.char_str());
				CheckGLError( "GLVertexAttribs :: Initialise - glGetAttribLocation");

				if (m_iAttribLocation >= 0 && m_iAttribLocation != GL_INVALID_INDEX)
				{
					l_bReturn = true;
				}
			}

			return l_bReturn;
		}
		void ActivateAttribute()
		{
			glEnableVertexAttribArray( m_iAttribLocation);
			CheckGLError( "GLVertexAttribs :: Activate - " + m_strAttribName + " - glEnableVertexAttribArray");
			glVertexAttribPointer( m_iAttribLocation, Count, m_iType, GL_FALSE, 0, NULL);
			CheckGLError( "GLVertexAttribs :: Activate - " + m_strAttribName + " - glVertexAttribPointer");
		}
		void DeactivateAttribute()
		{
			glDisableVertexAttribArray( m_iAttribLocation);
			CheckGLError( "GLVertexAttribs :: Deactivate - " + m_strAttribName + " - glDisableVertexAttribArray");
		}

	private:
		int _getType()
		{
			if (typeid( T) == typeid( int))
			{
				return GL_INT;
			}
			if (typeid( T) == typeid( size_t))
			{
				return GL_UNSIGNED_INT;
			}
			if (typeid( T) == typeid( short))
			{
				return GL_SHORT;
			}
			if (typeid( T) == typeid( unsigned short))
			{
				return GL_UNSIGNED_SHORT;
			}
			if (typeid( T) == typeid( char))
			{
				return GL_BYTE;
			}
			if (typeid( T) == typeid( unsigned char))
			{
				return GL_UNSIGNED_BYTE;
			}
			if (typeid( T) == typeid( float))
			{
				return GL_FLOAT;
			}
			if (typeid( T) == typeid( double))
			{
				return GL_DOUBLE;
			}

			return 0;
		}
	};

	//**************************************************************************
	// Vertex Buffer Objects
	//**************************************************************************

	template <typename T>
	class CS3D_GL_API GLVertexBufferObject
	{
	private:
		T *& m_pBuffer;
		unsigned int & m_uiFilled;

	protected:
		unsigned int m_uiIndex;

	public:
		GLVertexBufferObject( unsigned int & p_uiFilled, T *& p_pBuffer)
			:	m_uiIndex( GL_INVALID_INDEX),
				m_uiFilled( p_uiFilled),
				m_pBuffer( p_pBuffer)
		{
		}
		virtual ~GLVertexBufferObject()
		{
			CleanupBufferObject();
		}
		virtual void CleanupBufferObject()
		{
			if (m_uiIndex != GL_INVALID_INDEX && glIsBuffer( m_uiIndex))
			{
				glDeleteBuffers( 1, & m_uiIndex);
				CheckGLError( "GLVertexBufferObject :: Cleanup - glDeleteBuffers");
			}

			m_uiIndex = GL_INVALID_INDEX;
		}
		virtual void InitialiseBufferObject()
		{
			glGenBuffers( 1, & m_uiIndex);
			CheckGLError( "GLVertexBufferObject :: Initialise - glGenBuffers");

			if (m_uiIndex != GL_INVALID_INDEX)
			{
				glBindBuffer( GL_ARRAY_BUFFER, m_uiIndex);
				CheckGLError( "GLVertexBufferObject :: Initialise - glBindBuffer");
				glBufferData( GL_ARRAY_BUFFER, m_uiFilled * sizeof( T), m_pBuffer, GL_STATIC_DRAW);
				CheckGLError( "GLVertexBufferObject :: Initialise - glBufferData");
			}
		}
		virtual void ActivateBufferObject()
		{
			glBindBuffer( GL_ARRAY_BUFFER, m_uiIndex);
			CheckGLError( "GLVertexBufferObject :: Activate - glBindBuffer");
		}
	};

	template <typename T, size_t Count>
	class CS3D_GL_API GLVertexAttribsBuffer : public VertexAttribsBuffer<T>, public GLVertexAttribs<T, Count>, public GLVertexBufferObject<T>
	{
	public:
		GLVertexAttribsBuffer( const String & p_strAttribName)
			:	GLVertexAttribs( p_strAttribName),
				GLVertexBufferObject( VertexAttribsBuffer<T>::m_filled, VertexAttribsBuffer<T>::m_buffer)
		{
		}
		virtual ~GLVertexAttribsBuffer()
		{
			Cleanup();
		}
		virtual void Cleanup()
		{
			Buffer::Cleanup();
			CleanupBufferObject();
			CleanupAttribute();
		}
		virtual void CleanupBufferObject()
		{
			if (m_bAssigned)
			{
				BufferManager::UnassignBuffer<T>( m_uiIndex, this);
			}

			GLVertexBufferObject::CleanupBufferObject();
			m_bAssigned = false;
		}
		virtual void SetShaderProgram( ShaderProgram * p_pProgram)
		{
			GLShaderProgram * l_pProgram = (GLShaderProgram *)p_pProgram;

			if (m_pProgram != l_pProgram)
			{
				CleanupAttribute();
				CleanupBufferObject();
				m_pProgram = l_pProgram;
				Initialise();
			}
		}
		virtual void Initialise()
		{
			if (InitialiseAttribute())
			{
				InitialiseBufferObject();
				m_bAssigned = (m_uiIndex != GL_INVALID_INDEX);

				if (m_bAssigned)
				{
					BufferManager::AssignBuffer<T>( m_uiIndex, this);
				}
			}
		}
		virtual void Activate()
		{
			if (m_bAssigned)
			{
				ActivateBufferObject();
				ActivateAttribute();
			}
		}
		virtual void Deactivate()
		{
			if (m_bAssigned)
			{
				DeactivateAttribute();
			}
		}
	};

	class CS3D_GL_API GLVBOVertexBuffer : public VertexBuffer, public GLVertexAttribs<real, 3>, public GLVertexBufferObject<real>
	{
	public:
		GLVBOVertexBuffer();
		virtual ~GLVBOVertexBuffer();

		virtual void Cleanup();
		virtual void CleanupBufferObject();
		virtual void SetShaderProgram( ShaderProgram * p_pProgram);
		virtual void Initialise();
		virtual void Activate();
		virtual void Deactivate();
	};

	class CS3D_GL_API GLVBONormalsBuffer : public NormalsBuffer, public GLVertexAttribs<real, 3>, public GLVertexBufferObject<real>
	{
	public:
		GLVBONormalsBuffer();
		virtual ~GLVBONormalsBuffer();

		virtual void Cleanup();
		virtual void CleanupBufferObject();
		virtual void SetShaderProgram( ShaderProgram * p_pProgram);
		virtual void Initialise();
		virtual void Activate();
		virtual void Deactivate();
	};

	class CS3D_GL_API GLVBOTextureBuffer : public TextureBuffer
	{
	private:
		std::vector <GLVertexAttribsBuffer2rPtr> m_arrayBuffers;

	public:
		GLVBOTextureBuffer();
		virtual ~GLVBOTextureBuffer();

		virtual void Cleanup();
		virtual void CleanupBufferObject();
		virtual void CleanupAttribute();
		virtual void Initialise( PassPtr p_pass);
		virtual void Activate( PassPtr p_pass);
		virtual void Deactivate();
		void SetShaderProgram( ShaderProgram * p_pProgram);
	};

	class CS3D_GL_API GLUBOVariable
	{
		friend class GLUniformBufferObject;

	protected:
		size_t m_uiIndex;
		int m_iOffset;
		int m_iSize;
		unsigned char * m_pValue;
		size_t m_uiValueSize;

	public:
		GLUBOVariable();
		~GLUBOVariable();

		void Activate();
		void Deactivate();
		void SetValue( const void * p_pValue);
		void SetIndexValue( const void * p_pValue, int p_iIndex);
	};

	class CS3D_GL_API GLUniformBufferObject : public Buffer3D<real>
	{
	public:
		GLShaderProgram * m_pProgram;
		size_t m_uiIndex;
		int m_iUniformBlockIndex;
		int m_iUniformBlockSize;
		String m_strUniformBlockName;
		GLUBOVariablePtrStrMap m_mapVariables;

	public:
		GLUniformBufferObject( const String & p_strUniformBlockName, GLShaderProgram * p_pProgram);
		virtual ~GLUniformBufferObject();

		virtual void Cleanup();
		virtual void Initialise();
		virtual void Activate();
		virtual void Deactivate();
		GLUBOVariablePtr AddVariable( const String & p_strName, unsigned char * p_pValue, size_t p_uiValueSize);
		GLUBOVariablePtr GetVariable( const String & p_strName);
		void SetVariableValue( const String & p_strName, const void * p_pValue);
	};
}

#endif