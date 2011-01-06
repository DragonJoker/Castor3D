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
#ifndef ___Castor_Buffer___
#define ___Castor_Buffer___

#include "Module_Resource.h"
#include "Assertion.h"

namespace Castor
{	namespace Resources
{
	//! Buffer management class
	/*!
	Class which manages the buffers used in 3D (texture, VBO ...)
	Not to be used to manage usual buffers
	*/
	template <typename T>
	class Buffer : public MemoryTraced< Buffer<T> >
	{
	public:
		T * m_buffer;				//!< The data buffer
		unsigned int m_totalSize;	//!< The buffer size
		unsigned int m_filled;		//!< The buffer current filling

	public:
		/** 
		* Constructor, initialises its buffer at NULL
		*/
		Buffer()
			:	m_buffer( NULL),
				m_totalSize( 0),
				m_filled( 0)
		{
		}
		/** 
		* Copy Constructor
		*/
		Buffer( const Buffer<T> & p_buffer)
			:	m_buffer( new T[p_buffer.m_totalSize]),
				m_totalSize( p_buffer.m_totalSize),
				m_filled( p_buffer.m_filled)
		{
			memcpy( m_buffer, p_buffer.m_buffer, m_filled * sizeof( T));
		}
		/** 
		* Constructor from a buffer
		*/
		Buffer( T * p_pBuffer, unsigned int p_uiFilled)
			:	m_buffer( new T[p_uiFilled]),
				m_totalSize( p_uiFilled),
				m_filled( p_uiFilled)
		{
			memcpy( m_buffer, p_pBuffer, p_uiFilled * sizeof( T));
		}
		/**
		* Destructor, cleans the buffer
		*/
		virtual ~Buffer()
		{
			Cleanup();
		}
		/**
		* Cleanup function, deletes the buffer
		*/
		virtual void Cleanup()
		{
			delete [] m_buffer;
			m_buffer = NULL;
			m_totalSize = 0;
			m_filled = 0;
		}
		/**
		* Initialisation function, initialises the buffer with the size in parameters
		*@param p_size : the new size to allocate the buffer
		*@param p_filled : tells the buffer must be considered filled from this
		*/
		virtual void InitialiseBuffer( unsigned int p_size, unsigned int p_filled)
		{
			Buffer::Cleanup();

			if (p_size > 0)
			{
				m_buffer = new T[p_size];
				memset( m_buffer, 0, sizeof( T) * p_size);
				m_totalSize = p_size;
				m_filled = p_filled;
			}
		}
		/**
		* Initialisation function, initialises the buffer from another one
		*@param p_buffer : The buffer from which the initialisation must be done
		*/
		virtual void InitialiseBuffer( const Buffer<T> & p_buffer)
		{
			Buffer::Cleanup();

			if (p_buffer.m_filled > 0)
			{
				m_buffer = new T[p_buffer.m_filled];
				memcpy( m_buffer, p_buffer.m_buffer, p_buffer.m_filled * sizeof( T));
				m_totalSize = p_buffer.m_filled;
				m_filled = p_buffer.m_filled;
			}
		}
		/**
		 * Adds an element to the buffer
		 *@param p_element : [in] the element to add
		 */
		void Add( const T & p_element)
		{
			CASTOR_ASSERT( m_filled < m_totalSize);
			m_buffer[m_filled++] = p_element;
		}
		/**
		 * Adds an array of bytes to the buffer
		 *@param p_pArray : [in] The array to add
		 *@param p_uiNbElements : [in] The array size in bytes
		 */
		void AddArray( const void * p_pArray, size_t p_uiNbElements)
		{
			CASTOR_ASSERT( m_filled + p_uiNbElements / sizeof( T) <= m_totalSize);

			if (p_pArray != NULL)
			{
				memcpy( & m_buffer[m_filled], p_pArray, p_uiNbElements);
			}

			m_filled += p_uiNbElements / sizeof( T);
		}
		/**
		 * Adds an array of elements to the buffer
		 *@param p_pArray : [in] The array of elements to add
		 *@param p_uiNbElements : [in] The number of elements
		 */
		void AddArray( const T * p_pArray, size_t p_uiNbElements)
		{
			CASTOR_ASSERT( m_filled + p_uiNbElements <= m_totalSize);

			if (p_pArray != NULL)
			{
				memcpy( & m_buffer[m_filled], p_pArray, p_uiNbElements * sizeof( T));
			}

			m_filled += p_uiNbElements;
		}
		/**
		 * Changes m_filled to the given index
		 *@param p_index : [in] The index
		 */
		void Seek( unsigned int p_index)
		{
			CASTOR_ASSERT( p_index < m_totalSize);
			m_filled = p_index;
		}
		/**
		 * Increases the size of the buffer
		 *@param p_inc : [in] 
		 *@param p_bResizeBuffer : [in] tells if the buffer
		 */
		void IncreaseSize( unsigned int p_inc, bool p_bResizeBuffer)
		{
			m_totalSize += p_inc;

			if (p_bResizeBuffer)
			{
				if (m_buffer != NULL)
				{
					T * l_newBuf = new T[m_totalSize];

					if (m_filled > 0)
					{
						memcpy( l_newBuf, m_buffer, m_filled);
					}

					delete [] m_buffer;
					m_buffer = l_newBuf;
				}
				else
				{
					m_buffer = new T[m_totalSize];
				}
			}
		}
		/**@name Accessors */
		//@{
		inline unsigned int GetSize()const
		{
			return m_totalSize;
		}
		inline unsigned int GetFilled()const
		{
			return m_filled;
		}
		inline const T * GetBuffer()const
		{
			return m_buffer;
		}
		inline T * GetBuffer()
		{
			return m_buffer;
		}
		inline T * GetCurrent()const
		{
			return & m_buffer[m_filled];
		}
		inline void Offset( int p_iOffset)
		{
			m_filled = unsigned int( Castor::Math::clamp( int( 0), int( m_filled) + p_iOffset, int( m_totalSize)));
		}
		//@}
		/**@name Operators */
		//@{
		inline const T & operator []( unsigned int p_uiIndex)const
		{
			CASTOR_ASSERT( p_uiIndex < m_filled);
			return m_buffer[p_uiIndex];
		}
		inline T & operator []( unsigned int p_uiIndex)
		{
			CASTOR_ASSERT( p_uiIndex < m_filled);
			return m_buffer[p_uiIndex];
		}
		inline Buffer & operator =( const Buffer<T> & p_buffer)
		{
			InitialiseBuffer( p_buffer);
			return * this;
		}
		//@}
	};
}
}

#endif