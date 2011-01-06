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
#ifndef ___GL_Buffer___
#define ___GL_Buffer___

#include "Module_GLRender.h"
#include "GLShaderProgram.h"

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
		SmartPtr<GLShaderProgram>::Weak m_pProgram;
		int m_iType;

	public:
		GLVertexAttribs( const String & p_strAttribName)
			:	m_strAttribName( p_strAttribName),
				m_iAttribLocation( GL_INVALID_INDEX)
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
			m_pProgram.reset();
		}
		bool InitialiseAttribute()
		{
			bool l_bReturn = ! RenderSystem::UseShaders();

			if ( ! m_pProgram.expired() && m_pProgram.lock()->IsLinked() && ! l_bReturn)
			{
				m_iAttribLocation = glGetAttribLocation( m_pProgram.lock()->GetProgramObject(), m_strAttribName.char_str());
				CheckGLError( CU_T( "GLVertexAttribs :: Initialise - glGetAttribLocation"));

				if (m_iAttribLocation >= 0 && m_iAttribLocation != GL_INVALID_INDEX)
				{
					l_bReturn = true;
				}
			}

			return l_bReturn;
		}
		void ActivateAttribute( bool p_bNormalised = false, size_t p_uiStride = 0, size_t p_uiOffset=0)
		{
			glEnableVertexAttribArray( m_iAttribLocation);
			CheckGLError( CU_T( "GLVertexAttribs :: Activate - ") + m_strAttribName + CU_T( " - glEnableVertexAttribArray"));
			glVertexAttribPointer( m_iAttribLocation, Count, m_iType, p_bNormalised, p_uiStride, (char *)NULL + p_uiOffset);
			CheckGLError( CU_T( "GLVertexAttribs :: Activate - ") + m_strAttribName + CU_T( " - glVertexAttribPointer"));
		}
		void DeactivateAttribute()
		{
			glDisableVertexAttribArray( m_iAttribLocation);
			CheckGLError( CU_T( "GLVertexAttribs :: Deactivate - ") + m_strAttribName + CU_T( " - glDisableVertexAttribArray"));
		}

		inline void SetShaderProgram( ShaderProgramPtr p_pProgram)
		{
			m_pProgram = static_pointer_cast<GLShaderProgram>( p_pProgram);
		}
		inline int GetAttribLocation()const
		{
			return m_iAttribLocation;
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
			if (typeid( T) == typeid( real))
			{
				return GL_REAL;
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
	class GLVertexBufferObject
	{
	private:
		T *& m_pBuffer;
		unsigned int & m_uiFilled;

	protected:
		size_t m_uiTarget;

	protected:
		unsigned int m_uiIndex;

	public:
		GLVertexBufferObject( unsigned int & p_uiFilled, T *& p_pBuffer)
			:	m_uiIndex( GL_INVALID_INDEX),
				m_uiFilled( p_uiFilled),
				m_pBuffer( p_pBuffer),
				m_uiTarget( GL_ARRAY_BUFFER)
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
				CheckGLError( CU_T( "GLVertexBufferObject :: Cleanup - glDeleteBuffers"));
			}

			m_uiIndex = GL_INVALID_INDEX;
		}
		virtual void InitialiseBufferObject()
		{
			glGenBuffers( 1, & m_uiIndex);
			CheckGLError( CU_T( "GLVertexBufferObject :: Initialise - glGenBuffers"));

			if (m_uiIndex != GL_INVALID_INDEX && glIsBuffer( m_uiIndex))
			{
				glBindBuffer( m_uiTarget, m_uiIndex);
				CheckGLError( CU_T( "GLVertexBufferObject :: Initialise - glBindBuffer"));
				glBufferData( m_uiTarget, m_uiFilled * sizeof( T), m_pBuffer, GL_STREAM_DRAW);
				CheckGLError( CU_T( "GLVertexBufferObject :: Initialise - glBufferData"));
				glBindBuffer( m_uiTarget, 0);
			}
			else
			{
				m_uiIndex = GL_INVALID_INDEX;
			}
		}
		virtual void ActivateBufferObject()
		{
			glBindBuffer( m_uiTarget, m_uiIndex);
			CheckGLError( CU_T( "GLVertexBufferObject :: Activate - glBindBuffer"));
		}
	};

	template <typename T, size_t Count>
	class GLVertexAttribsBuffer : public VertexAttribsBuffer<T>, public GLVertexAttribs<T, Count>, public GLVertexBufferObject<T>
	{
	public:
		GLVertexAttribsBuffer( const String & p_strAttribName)
			:	VertexAttribsBuffer( p_strAttribName)
			,	GLVertexAttribs( p_strAttribName)
			,	GLVertexBufferObject( VertexAttribsBuffer<T>::m_filled, VertexAttribsBuffer<T>::m_buffer)
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
		virtual void SetShaderProgram( ShaderProgramPtr p_pProgram)
		{
			GLShaderProgramPtr l_pProgram = static_pointer_cast<GLShaderProgram>( p_pProgram);

			if ((m_pProgram.expired() && ! l_pProgram == NULL) || (l_pProgram == NULL && ! m_pProgram.expired()) || m_pProgram.lock() != l_pProgram)
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

	class GLVBOIndicesBuffer : public IndicesBuffer, public GLVertexBufferObject<unsigned int>
	{
	public:
		GLVBOIndicesBuffer();
		virtual ~GLVBOIndicesBuffer();

		virtual void Cleanup();
		virtual void CleanupBufferObject();
		virtual void Initialise();
		virtual void Activate();
		virtual void Deactivate();
	};

	class GLVBOVertexBuffer : public VertexBuffer, public GLVertexAttribs<real, 3>, public GLVertexBufferObject<real>
	{
	public:
		GLVBOVertexBuffer();
		virtual ~GLVBOVertexBuffer();

		virtual void Cleanup();
		virtual void CleanupBufferObject();
		virtual void SetShaderProgram( ShaderProgramPtr p_pProgram);
		virtual void Initialise();
		virtual void Activate();
		virtual void Deactivate();
	};

	class GLVBONormalsBuffer : public NormalsBuffer, public GLVertexAttribs<real, 3>, public GLVertexBufferObject<real>
	{
	public:
		GLVBONormalsBuffer();
		virtual ~GLVBONormalsBuffer();

		virtual void Cleanup();
		virtual void CleanupBufferObject();
		virtual void SetShaderProgram( ShaderProgramPtr p_pProgram);
		virtual void Initialise();
		virtual void Activate();
		virtual void Deactivate();
	};

	class GLVertexInfosBufferObject : public VertexInfosBuffer, public GLVertexBufferObject<real>
	{
	private:
		GLVertexAttribs3r m_vertex;
		GLVertexAttribs3r m_normal;
		GLVertexAttribs3r m_tangent;
		GLVertexAttribs2r m_texture;
		SmartPtr<GLShaderProgram>::Weak m_pProgram;
		size_t m_uiValid;

	public:
		GLVertexInfosBufferObject();
		virtual ~GLVertexInfosBufferObject();

		virtual void Cleanup();
		virtual void CleanupBufferObject();
		virtual void SetShaderProgram( ShaderProgramPtr p_pProgram);
		virtual void Initialise();
		virtual void Activate();
		virtual void Deactivate();

		void CleanupAttribute();
		bool InitialiseAttribute();
		void ActivateAttribute();
		void DeactivateAttribute();

		inline size_t GetNbComponents() { return 11; }
	};
}

#endif