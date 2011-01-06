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
#ifndef ___GL3_Buffer___
#define ___GL3_Buffer___

#include "Module_GLRender.h"
#include <Castor3D/render_system/Buffer.h>
#include <OpenGLCommon/GLBuffer.h>

namespace Castor3D
{
	//**************************************************************************
	// VBO Texture coords
	//**************************************************************************

	class C3D_GL3_API GLVBOTextureBuffer : public TextureBuffer
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
		void SetShaderProgram( ShaderProgramPtr p_pProgram);
	};

	//**************************************************************************
	// Uniform Buffer Objects
	//**************************************************************************

	class C3D_GL3_API GLUBOVariableBase
	{
		friend class GLUniformBufferObject;

	protected:
		size_t m_uiIndex;
		int m_iOffset;
		int m_iSize;
		void * m_pBuffer;
		bool m_bChanged;
		size_t m_uiCount;
		String m_strName;
		GLUniformBufferObject * m_pParent;

	public:
		GLUBOVariableBase( GLUniformBufferObject * p_pParent, const String & p_strName, size_t p_uiCount);
		~GLUBOVariableBase();

		void Activate();
		void Deactivate();

		virtual size_t		GetSize		()const = 0;
		inline void *		GetBuffer	()		{ return m_pBuffer; }
		inline const void *	GetBuffer	()const	{ return m_pBuffer; }

		inline void SetBuffer( void * p_pBuffer) { m_pBuffer = p_pBuffer; }
	};

	template <typename T>
	class C3D_GL3_API GLUBOOneVariable : public GLUBOVariableBase, public MemoryTraced< GLUBOOneVariable<T> >
	{
		friend class GLUniformBufferObject;

	private:
		T ** m_tValue;

	public:
		GLUBOOneVariable( GLUniformBufferObject * p_pParent, const String & p_strName, size_t p_uiCount)
			:	GLUBOVariableBase( p_pParent, p_strName, p_uiCount)
		{
			m_tValue = new T*[p_uiCount];
		}
		~GLUBOOneVariable()
		{
			delete [] m_tValue;
		}
		void SetValue( const T & p_tValue, size_t p_uiIndex)
		{
			m_pParent->SetModified( m_pParent->IsModified() || *(m_tValue[p_uiIndex]) != p_tValue);
			*(m_tValue[p_uiIndex]) = p_tValue;
		}
		void SetValues( T * p_tValues)
		{
			for (size_t i = 0 ; i < m_uiCount ; i++)
			{
				m_pParent->SetModified( m_pParent->IsModified() || *(m_tValue[i]) != p_tValues[i]);
				*(m_tValue[i]) = p_tValues[i];
			}
		}
		void SetBuffer( void * p_pBuffer)
		{
			GLUBOVariableBase::SetBuffer( p_pBuffer);

			for (size_t i = 0 ; i < m_uiCount ; i++)
			{
				m_tValue[i] = & ((T *)m_pBuffer)[i];
			}
		}
		inline const T &		GetValue	( size_t p_uiIndex)const	{ return m_tValue[p_uiIndex]; }
		inline T &				GetValue	( size_t p_uiIndex)			{ return m_tValue[p_uiIndex]; }
		virtual inline size_t	GetSize		()const						{ return sizeof( T) * m_uiCount; }
	};

	template <typename T, size_t Count>
	class C3D_GL3_API GLUBOPointVariable : public GLUBOVariableBase, public MemoryTraced< GLUBOPointVariable<T, Count> >
	{
		friend class GLUniformBufferObject;

	private:
		Point<T, Count> * m_ptValue;

	public:
		GLUBOPointVariable( GLUniformBufferObject * p_pParent, const String & p_strName, size_t p_uiCount)
			:	GLUBOVariableBase( p_pParent, p_strName, p_uiCount)
		{
			m_ptValue = new Point<T, Count>[p_uiCount];
		}
		~GLUBOPointVariable()
		{
			delete [] m_ptValue;
		}
		void SetValue( const Point<T, Count> & p_ptValue, size_t p_uiIndex)
		{
			m_pParent->SetModified( m_pParent->IsModified() || m_ptValue[p_uiIndex] != p_ptValue);
			m_ptValue[p_uiIndex] = p_ptValue;
		}
		void SetValues( Point<T, Count> * p_ptValues)
		{
			for (size_t i = 0 ; i < m_uiCount ; i++)
			{
				m_pParent->SetModified( m_pParent->IsModified() || m_ptValue[i] != p_ptValues[i]);
				m_ptValue[i] = p_ptValues[i];
			}
		}
		void SetBuffer( void * p_pBuffer)
		{
			GLUBOVariableBase::SetBuffer( p_pBuffer);

			for (size_t i = 0 ; i < m_uiCount ; i++)
			{
				m_ptValue[i].LinkCoords( & ((T *)p_pBuffer)[i * Count]);
			}
		}
		inline const Point<T, Count> &	GetValue	( size_t p_uiIndex)const	{ return m_ptValue[p_uiIndex]; }
		inline Point<T, Count> &		GetValue	( size_t p_uiIndex)			{ return m_ptValue[p_uiIndex]; }
		virtual inline size_t			GetSize		()const						{ return sizeof( T) * Count * m_uiCount; }
	};

	template <typename T, size_t Rows, size_t Columns>
	class C3D_GL3_API GLUBOMatrixVariable : public GLUBOVariableBase, public MemoryTraced< GLUBOMatrixVariable<T, Rows, Columns> >
	{
		friend class GLUniformBufferObject;

	private:
		Matrix<T, Rows, Columns> * m_mtxValue;

	public:
		GLUBOMatrixVariable( GLUniformBufferObject * p_pParent, const String & p_strName, size_t p_uiCount)
			:	GLUBOVariableBase( p_pParent, p_strName, p_uiCount)
		{
			m_mtxValue = new Matrix<T, Rows, Columns>[p_uiCount];
		}
		~GLUBOMatrixVariable()
		{
			delete [] m_mtxValue;
		}
		void SetValue( const Matrix<T, Rows, Columns> & p_mtxValue, size_t p_uiIndex)
		{
			m_pParent->SetModified( m_pParent->IsModified() || m_mtxValue[p_uiIndex] != p_mtxValue);
			m_mtxValue[p_uiIndex] = p_mtxValue;
		}
		void SetValues( Matrix<T, Rows, Columns> * p_mtxValues)
		{
			for (size_t i = 0 ; i < m_uiCount ; i++)
			{
				m_pParent->SetModified( m_pParent->IsModified() || m_mtxValue[i] != p_mtxValues[i]);
				m_mtxValue[i] = p_mtxValues[i];
			}
		}
		void SetBuffer( void * p_pBuffer)
		{
			GLUBOVariableBase::SetBuffer( p_pBuffer);

			for (size_t i = 0 ; i < m_uiCount ; i++)
			{
				m_mtxValue[i].LinkCoords( & ((T *)p_pBuffer)[i * Rows * Columns]);
			}
		}
		inline const Matrix<T, Rows, Columns> & GetValue	( size_t p_uiIndex)const	{ return m_mtxValue[p_uiIndex]; }
		inline Matrix<T, Rows, Columns> &		GetValue	( size_t p_uiIndex)			{ return m_mtxValue[p_uiIndex]; }
		virtual inline size_t					GetSize		()const						{ return sizeof( T) * Rows * Columns * m_uiCount; }
	};

	class C3D_GL3_API GLUniformBufferObject : public Buffer3D<unsigned char>
	{
	public:
		GLShaderProgram * m_pProgram;
		size_t m_uiIndex;
		int m_iUniformBlockIndex;
		int m_iUniformBlockSize;
		String m_strUniformBlockName;
		GLUBOVariablePtrStrMap m_mapVariables;
		bool m_bChanged;

	public:
		GLUniformBufferObject( const String & p_strUniformBlockName);
		virtual ~GLUniformBufferObject();

		virtual void Cleanup();
		virtual void Initialise( GLShaderProgram * p_pProgram);
		virtual void Activate();
		virtual void Deactivate();
		bool AddVariable( GLUBOVariablePtr p_pVariable);
		template <typename T>
		typename SmartPtr<T>::Shared CreateVariable( const String & p_strName, size_t p_uiCount)
		{
			typename SmartPtr<T>::Shared l_pReturn;

			if (m_mapVariables.find( p_strName) != m_mapVariables.end())
			{
				l_pReturn = static_pointer_cast<T>( m_mapVariables.find( p_strName)->second);
			}
			else
			{
				l_pReturn = typename SmartPtr<T>::Shared( new T( this, p_strName, p_uiCount));

				if (AddVariable( l_pReturn))
				{
					l_pReturn->SetBuffer( & m_buffer[l_pReturn->m_iOffset]);
				}
				else
				{
					l_pReturn.reset();
				}
			}

			return l_pReturn;
		}
		template <typename T>
		typename SmartPtr<T>::Shared GetVariable( const String & p_strName)
		{
			typename SmartPtr<T>::Shared l_pReturn;

			if (m_mapVariables.find( p_strName) != m_mapVariables.end())
			{
				l_pReturn = static_pointer_cast<T>( m_mapVariables.find( p_strName)->second);
			}

			return l_pReturn;
		}
		inline void	SetModified	( bool p_bChanged) { m_bChanged = p_bChanged; }
		inline bool	IsModified	()const { return m_bChanged; }
	};

	//**************************************************************************
	// Vertex Array Object
	//**************************************************************************

	class GLVertexArrayObjects
	{
	private:
		size_t m_uiIndex;

	public:
		GLVertexArrayObjects();
		~GLVertexArrayObjects();

		void Cleanup();
		void Initialise();
		void Activate();
		void Deactivate();

		inline size_t	GetIndex	()const { return m_uiIndex; }
	};

	//**************************************************************************
	// Texture Buffer Object
	//**************************************************************************

	class C3D_GL3_API GLTextureBufferObject : public TextureBufferObject
	{
	public:
		unsigned int m_uiIndex;

	public:
		GLTextureBufferObject();
		virtual ~GLTextureBufferObject();

		virtual void Cleanup();
		virtual void Initialise( const Castor::Resources::PixelFormat & p_format, size_t p_uiSize, const unsigned char * p_pBytes);

		inline unsigned int	GetIndex	()const { return m_uiIndex; }
	};
}

#endif