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
#ifndef ___Gl3_Buffer___
#define ___Gl3_Buffer___

#include "Module_Gl3Render.h"
#include <Castor3D/render_system/Buffer.h>
#include <OpenGlCommon/GlBuffer.h>

namespace Castor3D
{

	//**************************************************************************
	// Uniform Buffer Objects
	//**************************************************************************

	class Gl3VboVertexBuffer : public GlVboVertexBuffer
	{
	public:
		Gl3VboVertexBuffer( const BufferElementDeclaration * p_pElements, size_t uiNbElements);
		virtual ~Gl3VboVertexBuffer();

		virtual void Activate();
		virtual void Deactivate();

	private:
		virtual void _activateBufferObject();
	};

	class C3D_Gl3_API GlUniformBufferObject : public Buffer3D<unsigned char>
	{
	public:
		GlShaderProgram * m_pProgram;
		size_t m_uiIndex;
		int m_iUniformBlockIndex;
		int m_iUniformBlockSize;
		String m_strUniformBlockName;
		GlUboVariablePtrStrMap m_mapVariables;
		bool m_bChanged;

	public:
		GlUniformBufferObject( const String & p_strUniformBlockName);
		virtual ~GlUniformBufferObject();

		virtual void Cleanup();
		virtual void Initialise( GlShaderProgram * p_pProgram);
		virtual void Activate();
		virtual void Deactivate();
		virtual void * Lock( size_t p_uiOffset, size_t p_uiSize, size_t p_uiFlags);
		virtual void Unlock();
		bool AddVariable( GlUboVariablePtr p_pVariable);
		template <typename T>
		shared_ptr<T> CreateVariable( const String & p_strName, size_t p_uiCount)
		{
			shared_ptr<T> l_pReturn;

			if (m_mapVariables.find( p_strName) != m_mapVariables.end())
			{
				l_pReturn = static_pointer_cast<T>( m_mapVariables.find( p_strName)->second);
			}
			else
			{
				l_pReturn = shared_ptr<T>( new T( this, p_strName, p_uiCount));

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
		shared_ptr<T> GetVariable( const String & p_strName)
		{
			shared_ptr<T> l_pReturn;

			if (m_mapVariables.find( p_strName) != m_mapVariables.end())
			{
				l_pReturn = static_pointer_cast<T>( m_mapVariables.find( p_strName)->second);
			}

			return l_pReturn;
		}
		inline void	SetModified	( bool p_bChanged) { m_bChanged = p_bChanged; }
		inline bool	IsModified	()const { return m_bChanged; }
	};

	class C3D_Gl3_API GlUboVariableBase
	{
		friend class GlUniformBufferObject;

	protected:
		size_t m_uiIndex;
		int m_iOffset;
		int m_iSize;
		void * m_pBuffer;
		bool m_bChanged;
		size_t m_uiCount;
		String m_strName;
		GlUniformBufferObject * m_pParent;

	public:
		GlUboVariableBase( GlUniformBufferObject * p_pParent, const String & p_strName, size_t p_uiCount);
		~GlUboVariableBase();

		void Activate();
		void Deactivate();

		virtual size_t		GetSize		()const = 0;
		inline void *		GetBuffer	()		{ return m_pBuffer; }
		inline const void *	GetBuffer	()const	{ return m_pBuffer; }

		inline void SetBuffer( void * p_pBuffer) { m_pBuffer = p_pBuffer; }
	};

	template <typename T>
	class C3D_Gl3_API GlUboOneVariable : public GlUboVariableBase, public MemoryTraced< GlUboOneVariable<T> >
	{
		friend class GlUniformBufferObject;

	private:
		T ** m_tValue;

	public:
		GlUboOneVariable( GlUniformBufferObject * p_pParent, const String & p_strName, size_t p_uiCount)
			:	GlUboVariableBase( p_pParent, p_strName, p_uiCount)
		{
			m_tValue = new T*[p_uiCount];
		}
		~GlUboOneVariable()
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
			GlUboVariableBase::SetBuffer( p_pBuffer);

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
	class C3D_Gl3_API GlUboPointVariable : public GlUboVariableBase, public MemoryTraced< GlUboPointVariable<T, Count> >
	{
		friend class GlUniformBufferObject;

	private:
		Point<T, Count> * m_ptValue;

	public:
		GlUboPointVariable( GlUniformBufferObject * p_pParent, const String & p_strName, size_t p_uiCount)
			:	GlUboVariableBase( p_pParent, p_strName, p_uiCount)
		{
			m_ptValue = new Point<T, Count>[p_uiCount];
		}
		~GlUboPointVariable()
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
			GlUboVariableBase::SetBuffer( p_pBuffer);

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
	class C3D_Gl3_API GlUboMatrixVariable : public GlUboVariableBase, public MemoryTraced< GlUboMatrixVariable<T, Rows, Columns> >
	{
		friend class GlUniformBufferObject;

	private:
		Matrix<T, Rows, Columns> * m_mtxValue;

	public:
		GlUboMatrixVariable( GlUniformBufferObject * p_pParent, const String & p_strName, size_t p_uiCount)
			:	GlUboVariableBase( p_pParent, p_strName, p_uiCount)
		{
			m_mtxValue = new Matrix<T, Rows, Columns>[p_uiCount];
		}
		~GlUboMatrixVariable()
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
			GlUboVariableBase::SetBuffer( p_pBuffer);

			for (size_t i = 0 ; i < m_uiCount ; i++)
			{
				m_mtxValue[i].LinkCoords( & ((T *)p_pBuffer)[i * Rows * Columns]);
			}
		}
		inline const Matrix<T, Rows, Columns> & GetValue	( size_t p_uiIndex)const	{ return m_mtxValue[p_uiIndex]; }
		inline Matrix<T, Rows, Columns> &		GetValue	( size_t p_uiIndex)			{ return m_mtxValue[p_uiIndex]; }
		virtual inline size_t					GetSize		()const						{ return sizeof( T) * Rows * Columns * m_uiCount; }
	};

	//**************************************************************************
	// Vertex Array Object
	//**************************************************************************

	class GlVertexArrayObjects
	{
	private:
		size_t m_uiIndex;

	public:
		GlVertexArrayObjects();
		~GlVertexArrayObjects();

		void Cleanup();
		void Initialise();
		void Activate();
		void Deactivate();

		inline size_t	GetIndex	()const { return m_uiIndex; }
	};

	//**************************************************************************
	// Texture Buffer Object
	//**************************************************************************

	class C3D_Gl3_API GlTextureBufferObject : public TextureBufferObject
	{
	public:
		unsigned int m_uiIndex;

	public:
		GlTextureBufferObject();
		virtual ~GlTextureBufferObject();

		virtual void Cleanup();
		virtual void Initialise( const Castor::Resources::ePIXEL_FORMAT & p_format, size_t p_uiSize, const unsigned char * p_pBytes);
		virtual void * Lock( size_t p_uiOffset, size_t p_uiSize, size_t p_uiFlags);
		virtual void Unlock();

		inline unsigned int	GetIndex	()const { return m_uiIndex; }
	};
}

#endif
