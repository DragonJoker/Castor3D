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
#ifndef ___C3D_Buffer___
#define ___C3D_Buffer___

#include "../material/Module_Material.h"
#include "../shader/Module_Shader.h"
#include "../geometry/basic/Vertex.h"

namespace Castor3D
{
	//! 3D Buffer management class
	/*!
	Class which holds the buffers used in 3D (texture, VBO ...)
	Not to be used to manage usual buffers of data, use Castor::Resource::Buffer for that
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	*/
	template <typename T>
	class C3D_API Buffer3D : public Castor::Resource::Buffer<T>
	{
	protected:
		bool m_bToDelete;
		bool m_bAssigned;

	public:
		Buffer3D()
			:	m_bAssigned( false),
				m_bToDelete( false)
		{
		}
		virtual ~Buffer3D()
		{
		}
		/**
		* Initialisation function, used by VBOs
		*/
		virtual void Initialise(){}
		/**
		* Activation function, to activate the buffer and show it's content in 3D
		*/
		virtual void Activate() = 0;
		/**
		* De-activation function, to deactivate the buffer and hide it's content in 3D
		*/
		virtual void Deactivate() = 0;

		virtual bool IsToDelete()const
		{
			return m_bToDelete;
		}
		virtual bool IsAssigned()const
		{
			return m_bAssigned;
		}
	};

	//! Vertex buffer representation
	/*!
	Holds the vertex coordinates of a submesh
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	*/
	class C3D_API VertexBuffer : public Buffer3D<real>
	{
	public:
		/**
		 * Activation function, to activate the buffer and show the vertex in it
		 */
		virtual void Activate(){}
		/**
		 * De-activation function, to deactivate the buffer and hide it's content
		 */
		virtual void Deactivate(){}
	};

	//! Attribs buffer representation
	/*!
	Holds vertex attributes such as normals, texture coordinates, ...
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	*/
	template <typename T>
	class C3D_API VertexAttribsBuffer : public Buffer3D<T>
	{
	public:
		/**
		 * Activation function, to activate the buffer and show the vertex in it
		 */
		virtual void Activate(){}
		/**
		 * De-activation function, to deactivate the buffer and hide it's content
		 */
		virtual void Deactivate(){}

		virtual void SetShaderProgram( ShaderProgram * p_pProgram)=0;
	};

	//! Normals buffer representation
	/*!
	Holds the normals coordinates of a submesh
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	*/
	class C3D_API NormalsBuffer : public Buffer3D<real>
	{
	public:
		/**
		 * Activation function, to activate the buffer and apply the normals in it
		 */
		virtual void Activate(){}
		/**
		 * De-activation function, to deactivate the buffer and hide it's content
		 */
		virtual void Deactivate(){}
	};

	//! Texture buffer representation, holds texture coordinates
	/*!
	Holds the texture coordinates of a submesh
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	*/
	class C3D_API TextureBuffer : public Buffer3D<real>
	{
	public:
		/**
		 * Activation function, void, use next
		 */
		virtual void Activate(){}
		/**
		 * Activation function, to activate the buffer and apply the textures in it
		 */
		virtual void Activate( PassPtr p_pass)=0;
		/**
		 * De-activation function, to deactivate the buffer and hide it's content
		 */
		virtual void Deactivate()=0;
	};

	//! Vertex infos buffer representation
	/*!
	Holds the vertex, normals, texture and tangent coordinates of a submesh
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	*/
	class C3D_API VertexInfosBuffer : public Buffer3D<real>
	{
	public:
		/**
		 * Activation function, to activate the buffer and apply the normals in it
		 */
		virtual void Activate(){}
		/**
		 * De-activation function, to deactivate the buffer and hide it's content
		 */
		virtual void Deactivate(){}

		virtual void AddVertex( Vertex & p_pVertex, bool p_bIncrease)
		{
			if (p_bIncrease)
			{
				IncreaseSize( Vertex::Size, true);
			}

			p_pVertex.LinkCoords( & m_buffer[m_filled]);
			m_filled += Vertex::Size;
		}
	};

	//! Texture buffer representation, holds texture data
	/*!
	Holds the texture coordinates of a submesh
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	*/
	class C3D_API TextureBufferObject : public Buffer3D<unsigned char>
	{
	protected:
		Castor::Resource::PixelFormat  m_pixelFormat;
		size_t m_uiSize;
		const unsigned char * m_pBytes;

	public:
		TextureBufferObject()
			:	m_pixelFormat( Castor::Resource::pxfR8G8B8A8),
				m_uiSize( 0),
				m_pBytes( NULL)
		{}
		~TextureBufferObject() { Cleanup(); }
		/**
		 * Activation function, void, use next
		 */
		virtual void Activate(){}
		/**
		 * Activation function, to activate the buffer and apply the textures in it
		 */
		virtual void Activate( PassPtr p_pass){}
		/**
		 * De-activation function, to deactivate the buffer and hide it's content
		 */
		virtual void Deactivate(){}
		/**
		* Initialisation function
		*/
		virtual void Initialise( const Castor::Resource::PixelFormat & p_format, size_t p_uiSize, const unsigned char * p_pBytes)
		{
			m_pixelFormat = p_format;
			m_uiSize = p_uiSize;
			m_pBytes = p_pBytes;
		}
		/**
		* Cleanup function
		*/
		virtual void Cleanup(){}
	};

	//! 3D Buffer manager
	/*!
	Holds the buffers used by all the meshes and other things.
	Manages the deletion of the buffers during the Update() function, in order to make it during a render loop (needed for OpenGL's VBO)
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	*/
	class C3D_API BufferManager : public Castor::Theory::AutoSingleton <BufferManager>
	{
		friend class Castor::Theory::AutoSingleton <BufferManager>;

	private:
		template <typename T>
		class TBuffers
		{
			friend class BufferManager;

		private:
			typedef Templates::SharedPtr< Buffer3D<T> > Buffer3DPtr;
			typedef bool (__thiscall * BufferFunction)( Buffer3DPtr p_pBuffer, size_t p_uiID);
			typedef BufferFunction PBufferFunction;

		public:
			C3DVector(	Buffer3DPtr)					m_arrayBuffersToDelete;
			C3DVector(	Buffer3DPtr)					m_arrayBuffers;
			C3DMap(		size_t,			Buffer3D<T> *)	m_mapBuffers;
			C3DMap(		Buffer3D<T> *,	size_t)			m_mapBuffersIndexes;

		public:
			void Cleanup()
			{
				for (size_t i = 0 ; i < m_arrayBuffers.size() ; i++)
				{
					DeleteBuffer( m_arrayBuffers[i]);
				}

				m_arrayBuffers.clear();
				m_mapBuffersIndexes.clear();
				m_mapBuffers.clear();
			}

			void Update()
			{
				for (size_t i = 0 ; i < m_arrayBuffersToDelete.size() ; i++)
				{
					m_arrayBuffersToDelete[i]->Cleanup();
				}

				m_arrayBuffersToDelete.clear();
			}

			bool AddBuffer( Buffer3DPtr p_pBuffer, size_t=0)
			{
				bool l_bReturn = false;

				CASTOR_ASSERT( ! p_pBuffer.null());

				if (m_mapBuffersIndexes.find( p_pBuffer.get()) == m_mapBuffersIndexes.end())
				{
					m_arrayBuffers.push_back( p_pBuffer);
					l_bReturn = true;
				}

				return l_bReturn;
			}

			bool AssignBuffer( Buffer3D<T> * p_pBuffer, size_t p_uiID)
			{
				bool l_bReturn = false;
				CASTOR_ASSERT( p_pBuffer != NULL && p_uiID != 0);

				if (m_mapBuffers.find( p_uiID) == m_mapBuffers.end())
				{
					for (size_t i = 0 ; i < m_arrayBuffers.size() && ! l_bReturn ; i++)
					{
						if (m_arrayBuffers[i] == p_pBuffer)
						{
							m_mapBuffers.insert( C3DMap( size_t, Buffer3D<T> *)::value_type( p_uiID, p_pBuffer));
							m_mapBuffersIndexes.insert( C3DMap( Buffer3D<T> *, size_t)::value_type( p_pBuffer, p_uiID));
							l_bReturn = true;
						}
					}
				}

				return l_bReturn;
			}

			bool UnassignBuffer( Buffer3D<T> * p_pBuffer, size_t p_uiID)
			{
				bool l_bReturn = false;
				CASTOR_ASSERT( p_pBuffer != NULL && p_uiID != 0);
				C3DMap( size_t, Buffer3D<T> *)::iterator l_it = m_mapBuffers.find( p_uiID);

				if (l_it != m_mapBuffers.end())
				{
/*
					for (C3DVector( Buffer3DPtr)::iterator i = m_arrayBuffers.begin() ; i != m_arrayBuffers.end() && ! l_bReturn ; i++)
					{
						if (( * i) == p_pBuffer)
						{
							m_arrayBuffers.erase( i);
							l_bReturn = true;
						}
					}
*/
					l_bReturn = true;

					if (m_mapBuffersIndexes.find( p_pBuffer) != m_mapBuffersIndexes.end())
					{
						m_mapBuffersIndexes.erase( m_mapBuffersIndexes.find( p_pBuffer));
					}
				}

				return l_bReturn;
			}

			bool DeleteBuffer( Buffer3DPtr p_pBuffer, size_t=0)
			{
				bool l_bReturn = false;

				CASTOR_ASSERT( ! p_pBuffer.null());

				C3DMap( Buffer3D<T> *, size_t)::iterator l_it = m_mapBuffersIndexes.find( p_pBuffer.get());

				if (l_it != m_mapBuffersIndexes.end())
				{
					size_t l_uiID = l_it->second;
					vector::eraseValue( m_arrayBuffers, p_pBuffer);
					C3DMap( size_t, Buffer3D<T> *)::iterator l_it2 = m_mapBuffers.find( l_it->second);

					if (l_it2 != m_mapBuffers.end())
					{
						if (p_pBuffer->IsAssigned())
						{
							UnassignBuffer( p_pBuffer.get(), l_uiID);
						}

						m_mapBuffers.erase( m_mapBuffers.find( l_uiID));
					}

					m_arrayBuffersToDelete.push_back( p_pBuffer);
				}

				return l_bReturn;
			}
		};

	private:
		TBuffers<float> m_stFloatBuffers;
		TBuffers<double> m_stDoubleBuffers;
		TBuffers<int> m_stIntBuffers;
		TBuffers<size_t> m_stUIntBuffers;
		TBuffers<bool> m_stBoolBuffers;
		TBuffers<char> m_stCharBuffers;
		TBuffers<unsigned char> m_stUCharBuffers;
		TBuffers<long> m_stLongBuffers;
		TBuffers<unsigned long> m_stULongBuffers;
		TBuffers<long long> m_stLongLongBuffers;
		TBuffers<unsigned long long> m_stULongLongBuffers;

	private:
		BufferManager(){}
		~BufferManager(){} // cleanup must be made with the update function and by all owners of buffers

	public:
		/**
		 * Cleans up the manager
		 */
		static void Cleanup()
		{
			BufferManager & l_pThis = GetSingleton();
			l_pThis.m_stFloatBuffers.Cleanup();
			l_pThis.m_stDoubleBuffers.Cleanup();
			l_pThis.m_stIntBuffers.Cleanup();
			l_pThis.m_stBoolBuffers.Cleanup();
			l_pThis.m_stCharBuffers.Cleanup();
			l_pThis.m_stUCharBuffers.Cleanup();
			l_pThis.m_stUIntBuffers.Cleanup();
			l_pThis.m_stLongBuffers.Cleanup();
			l_pThis.m_stULongBuffers.Cleanup();
			l_pThis.m_stLongLongBuffers.Cleanup();
			l_pThis.m_stULongLongBuffers.Cleanup();
		}
		/**
		 * Updates the manager, id est deletes unused buffers
		 */
		static void Update()
		{
			BufferManager & l_pThis = GetSingleton();
			l_pThis.m_stFloatBuffers.Update();
			l_pThis.m_stDoubleBuffers.Update();
			l_pThis.m_stIntBuffers.Update();
			l_pThis.m_stBoolBuffers.Update();
			l_pThis.m_stCharBuffers.Update();
			l_pThis.m_stUCharBuffers.Update();
			l_pThis.m_stUIntBuffers.Update();
			l_pThis.m_stLongBuffers.Update();
			l_pThis.m_stULongBuffers.Update();
			l_pThis.m_stLongLongBuffers.Update();
			l_pThis.m_stULongLongBuffers.Update();
		}
		/**
		 * Creates a buffer and adds it to the manager
		 *@return The newly created buffer
		 */
		template <typename T, typename _Ty>
		static Templates::SharedPtr<_Ty> CreateBuffer()
		{
			Templates::SharedPtr<_Ty> l_pBuffer = Templates::SharedPtr<_Ty>( new _Ty);

			if ( ! _applyFunction( Templates::SharedPtr< Buffer3D<T> >( l_pBuffer), 0, & TBuffers<T>::AddBuffer))
			{
				l_pBuffer.reset();
			}

			return l_pBuffer;
		}
		/**
		 * Creates a buffer to be linked in a shader and adds it to the manager
		 *@param p_strArg : [in] The buffer's name (as it appears in the shader)
		 *@return The newly created buffer
		 */
		template <typename T, typename _Ty>
		static Templates::SharedPtr<_Ty> CreateBuffer( const String & p_strArg)
		{
			Templates::SharedPtr<_Ty> l_pBuffer = Templates::SharedPtr<_Ty>( new _Ty( p_strArg));

			if ( ! _applyFunction( Templates::SharedPtr< Buffer3D<T> >( l_pBuffer), 0, & TBuffers<T>::AddBuffer))
			{
				l_pBuffer.reset();
			}

			return l_pBuffer;
		}
		/**
		 * Assigns the buffer
		 *@param p_uiID : the ID of the assigned buffer
		 *@param p_pBuffer : pointer to the buffer
		 *@return true if successful, false if not
		 */
		template <typename T>
		static bool AssignBuffer( size_t p_uiID, Buffer3D<T> * p_pBuffer)
		{
			return _applyFunctionPtr( p_pBuffer, p_uiID, & TBuffers<T>::AssignBuffer);
		}
		/**
		 * Unassigns the buffer
		 *@param p_uiID : the ID of the assigned buffer
		 *@param p_pBuffer : pointer to the buffer
		 *@return true if successful, false if not
		 */
		template <typename T>
		static bool UnassignBuffer( size_t p_uiID, Buffer3D<T> * p_pBuffer)
		{
			return _applyFunctionPtr( p_pBuffer, p_uiID, & TBuffers<T>::UnassignBuffer);
		}
		/**
		 * Deletes the buffer, id est mark it as unused
		 *@param p_pBuffer : pointer to the buffer
		 *@return true if successful, false if not
		 */
		template <typename T>
		static bool DeleteBuffer( Templates::SharedPtr< Buffer3D<T> > p_pBuffer)
		{
			return _applyFunction( p_pBuffer, 0, & TBuffers<T>::DeleteBuffer);
		}

	private:
		template <typename T>
		static bool _applyFunction( Templates::SharedPtr< Buffer3D<T> > p_pBuffer, size_t p_uiID,
			bool( TBuffers<T>::* p_function)( Templates::SharedPtr< Buffer3D<T> >, size_t))
		{
			BufferManager & l_pThis = GetSingleton();
			return l_pThis._apply( p_pBuffer, p_uiID, p_function);
		}
		template <typename T>
		static bool _applyFunctionPtr( Buffer3D<T> * p_pBuffer, size_t p_uiID, bool( TBuffers<T>::* p_function)( Buffer3D<T> *, size_t))
		{
			BufferManager & l_pThis = GetSingleton();
			return l_pThis._applyPtr( p_pBuffer, p_uiID, p_function);
		}

		// Déclaration template de la fonction d'application de la fonction du buffer :P
		template <typename T> bool _apply( Templates::SharedPtr< Buffer3D<T> > p_pBuffer, size_t p_uiID, bool( TBuffers<T>::* p_function)( Templates::SharedPtr< Buffer3D<T> >, size_t));
		// Déclaration template de la fonction d'application de la fonction du buffer :P
		template <typename T> bool _applyPtr( Buffer3D<T> * p_pBuffer, size_t p_uiID, bool( TBuffers<T>::* p_function)( Buffer3D<T> *, size_t));



		// Spécialisation pour les float
		template <> bool _apply( Templates::SharedPtr< Buffer3D<float> > p_pBuffer, size_t p_uiID, bool( TBuffers<float>::* p_function)( Templates::SharedPtr< Buffer3D<float> >, size_t))
		{
			return (m_stFloatBuffers.*p_function)( p_pBuffer, p_uiID);
		}
		// Spécialisation pour les double
		template <> bool _apply( Templates::SharedPtr< Buffer3D<double> > p_pBuffer, size_t p_uiID, bool( TBuffers<double>::* p_function)( Templates::SharedPtr< Buffer3D<double> >, size_t))
		{
			return (m_stDoubleBuffers.*p_function)( p_pBuffer, p_uiID);
		}
		// Spécialisation pour les int
		template <> bool _apply( Templates::SharedPtr< Buffer3D<int> > p_pBuffer, size_t p_uiID, bool( TBuffers<int>::* p_function)( Templates::SharedPtr< Buffer3D<int> >, size_t))
		{
			return (m_stIntBuffers.*p_function)( p_pBuffer, p_uiID);
		}
		// Spécialisation pour les unsigned int
		template <> bool _apply( Templates::SharedPtr< Buffer3D<size_t> > p_pBuffer, size_t p_uiID, bool( TBuffers<size_t>::* p_function)( Templates::SharedPtr< Buffer3D<size_t> >, size_t))
		{
			return (m_stUIntBuffers.*p_function)( p_pBuffer, p_uiID);
		}
		// Spécialisation pour les bool
		template <> bool _apply( Templates::SharedPtr< Buffer3D<bool> > p_pBuffer, size_t p_uiID, bool( TBuffers<bool>::* p_function)( Templates::SharedPtr< Buffer3D<bool> >, size_t))
		{
			return (m_stBoolBuffers.*p_function)( p_pBuffer, p_uiID);
		}
		// Spécialisation pour les char
		template <> bool _apply( Templates::SharedPtr< Buffer3D<char> > p_pBuffer, size_t p_uiID, bool( TBuffers<char>::* p_function)( Templates::SharedPtr< Buffer3D<char> >, size_t))
		{
			return (m_stCharBuffers.*p_function)( p_pBuffer, p_uiID);
		}
		// Spécialisation pour les unsigned char
		template <> bool _apply( Templates::SharedPtr< Buffer3D<unsigned char> > p_pBuffer, size_t p_uiID, bool( TBuffers<unsigned char>::* p_function)( Templates::SharedPtr< Buffer3D<unsigned char> >, size_t))
		{
			return (m_stUCharBuffers.*p_function)( p_pBuffer, p_uiID);
		}
		// Spécialisation pour les long
		template <> bool _apply( Templates::SharedPtr< Buffer3D<long> > p_pBuffer, size_t p_uiID, bool( TBuffers<long>::* p_function)( Templates::SharedPtr< Buffer3D<long> >, size_t))
		{
			return (m_stLongBuffers.*p_function)( p_pBuffer, p_uiID);
		}
		// Spécialisation pour les unsigned long
		template <> bool _apply( Templates::SharedPtr< Buffer3D<unsigned long> > p_pBuffer, size_t p_uiID, bool( TBuffers<unsigned long>::* p_function)( Templates::SharedPtr< Buffer3D<unsigned long> >, size_t))
		{
			return (m_stULongBuffers.*p_function)( p_pBuffer, p_uiID);
		}
		// Spécialisation pour les long long
		template <> bool _apply( Templates::SharedPtr< Buffer3D<long long> > p_pBuffer, size_t p_uiID, bool( TBuffers<long long>::* p_function)( Templates::SharedPtr< Buffer3D<long long> >, size_t))
		{
			return (m_stLongLongBuffers.*p_function)( p_pBuffer, p_uiID);
		}
		// Spécialisation pour les unsigned long long
		template <> bool _apply( Templates::SharedPtr< Buffer3D<unsigned long long> > p_pBuffer, size_t p_uiID, bool( TBuffers<unsigned long long>::* p_function)( Templates::SharedPtr< Buffer3D<unsigned long long> >, size_t))
		{
			return (m_stULongLongBuffers.*p_function)( p_pBuffer, p_uiID);
		}




		// Spécialisation pour les float
		template <> bool _applyPtr( Buffer3D<float> * p_pBuffer, size_t p_uiID, bool( TBuffers<float>::* p_function)( Buffer3D<float> *, size_t))
		{
			return (m_stFloatBuffers.*p_function)( p_pBuffer, p_uiID);
		}
		// Spécialisation pour les double
		template <> bool _applyPtr( Buffer3D<double> * p_pBuffer, size_t p_uiID, bool( TBuffers<double>::* p_function)( Buffer3D<double> *, size_t))
		{
			return (m_stDoubleBuffers.*p_function)( p_pBuffer, p_uiID);
		}
		// Spécialisation pour les int
		template <> bool _applyPtr( Buffer3D<int> * p_pBuffer, size_t p_uiID, bool( TBuffers<int>::* p_function)( Buffer3D<int> *, size_t))
		{
			return (m_stIntBuffers.*p_function)( p_pBuffer, p_uiID);
		}
		// Spécialisation pour les unsigned int
		template <> bool _applyPtr( Buffer3D<size_t> * p_pBuffer, size_t p_uiID, bool( TBuffers<size_t>::* p_function)( Buffer3D<size_t> *, size_t))
		{
			return (m_stUIntBuffers.*p_function)( p_pBuffer, p_uiID);
		}
		// Spécialisation pour les bool
		template <> bool _applyPtr( Buffer3D<bool> * p_pBuffer, size_t p_uiID, bool( TBuffers<bool>::* p_function)( Buffer3D<bool> *, size_t))
		{
			return (m_stBoolBuffers.*p_function)( p_pBuffer, p_uiID);
		}
		// Spécialisation pour les char
		template <> bool _applyPtr( Buffer3D<char> * p_pBuffer, size_t p_uiID, bool( TBuffers<char>::* p_function)( Buffer3D<char> *, size_t))
		{
			return (m_stCharBuffers.*p_function)( p_pBuffer, p_uiID);
		}
		// Spécialisation pour les unsigned char
		template <> bool _applyPtr( Buffer3D<unsigned char> * p_pBuffer, size_t p_uiID, bool( TBuffers<unsigned char>::* p_function)( Buffer3D<unsigned char> *, size_t))
		{
			return (m_stUCharBuffers.*p_function)( p_pBuffer, p_uiID);
		}
		// Spécialisation pour les long
		template <> bool _applyPtr( Buffer3D<long> * p_pBuffer, size_t p_uiID, bool( TBuffers<long>::* p_function)( Buffer3D<long> *, size_t))
		{
			return (m_stLongBuffers.*p_function)( p_pBuffer, p_uiID);
		}
		// Spécialisation pour les unsigned long
		template <> bool _applyPtr( Buffer3D<unsigned long> * p_pBuffer, size_t p_uiID, bool( TBuffers<unsigned long>::* p_function)( Buffer3D<unsigned long> *, size_t))
		{
			return (m_stULongBuffers.*p_function)( p_pBuffer, p_uiID);
		}
		// Spécialisation pour les long long
		template <> bool _applyPtr( Buffer3D<long long> * p_pBuffer, size_t p_uiID, bool( TBuffers<long long>::* p_function)( Buffer3D<long long> *, size_t))
		{
			return (m_stLongLongBuffers.*p_function)( p_pBuffer, p_uiID);
		}
		// Spécialisation pour les unsigned long long
		template <> bool _applyPtr( Buffer3D<unsigned long long> * p_pBuffer, size_t p_uiID, bool( TBuffers<unsigned long long>::* p_function)( Buffer3D<unsigned long long> *, size_t))
		{
			return (m_stULongLongBuffers.*p_function)( p_pBuffer, p_uiID);
		}
	};
}

#endif
