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
#ifndef ___C3D_BufferElement___
#define ___C3D_BufferElement___

namespace Castor3D
{
	//! Element usage enumerator
	/*!
	Different usages for buffer elements
	\author Sylvain DOREMUS
	\version 0.6.1.0
	\date 03/01/2011
	*/
	typedef enum
	{
		eUsagePosition,		//< Position vertex
		eUsageNormal,		//< Normal vertex
		eUsageTangent,		//< Tangent vertex
		eUsageDiffuse,		//!< Diffuse colour
		eUsageTexCoords0,	//< Texture coordinates 0
		eUsageTexCoords1,	//< Texture coordinates 1
		eUsageTexCoords2,	//< Texture coordinates 2
		eUsageTexCoords3,	//< Texture coordinates 3
		eNbUsages,
	}
	eELEMENT_USAGE;

	//! Element type enumerator
	/*!
	Different types for buffer elements
	\author Sylvain DOREMUS
	\version 0.6.1.0
	\date 03/01/2011
	*/
	typedef enum
	{
		eType1Float,	//< 1 float
		eType2Floats,	//< 2 floats
		eType3Floats,	//< 3 floats
		eType4Floats,	//< 4 floats
		eTypeColour,	//< colour (unsigned int)
		eNbTypes,
	}
	eELEMENT_TYPE;

	//! Buffer element description
	/*!
	Describes usage and type of an element in a vertex/index buffer
	\author Sylvain DOREMUS
	\version 0.6.1.0
	\date 03/01/2011
	*/
	struct BufferElementDeclaration
	{
		unsigned int	m_uiStream;		//< Stream index
		eELEMENT_USAGE	m_eUsage;		//< Element usage
		eELEMENT_TYPE	m_eDataType;	//< Element type
		size_t			m_uiOffset;		//< Offset in stream
	};

	class BufferDeclaration
	{
	public:
		typedef std::vector <BufferElementDeclaration> BufferElementDeclarationArray;
		typedef BufferElementDeclarationArray::const_iterator const_iterator;

	protected:
		BufferElementDeclarationArray m_arrayElements;
		size_t m_uiStride;

	public:
		template <size_t N> BufferDeclaration( const BufferElementDeclaration (& p_elements)[N])
		{
			_initialise( p_elements, N);
		}
		BufferDeclaration( const BufferElementDeclaration * p_pElements, size_t p_uiNbElements);
		virtual ~BufferDeclaration();

		inline const_iterator	Begin		()const	{ return m_arrayElements.begin(); }
		inline const_iterator	End			()const	{ return m_arrayElements.end(); }
		inline size_t			Size		()const { return m_arrayElements.size(); }
		inline size_t			GetStride	()const { return m_uiStride; }

	private:
		void _initialise( const BufferElementDeclaration * p_pElements, size_t p_uiNbElements);
	};

	class BufferElement
	{
	protected:
		template <typename T, size_t Count> struct ElementCopier;
		typedef PointBase element;
		typedef element * element_ptr;
		typedef std::vector<element> element_array;
		typedef std::vector<element_ptr> element_ptr_array;
		BufferDeclaration * m_pDeclaration;

		unsigned char * m_pBuffer;
		element_ptr_array m_arrayElements;
		size_t m_uiStride;
		bool m_bOwnBuffer;

	public:
		BufferElement( BufferDeclaration * p_pDeclaration);
		virtual ~BufferElement();
		/**
		 * Links the vertex coords to the ones in parameter.
		 * The vertex no longer owns it's coords
		 *@param p_pBuffer : [in] The coordinates buffer
		 */
		void LinkCoords( void * p_pBuffer);
		/**
		 * Unlinks the vertex coords.
		 * The vertex now owns it's coords
		 */
		void UnlinkCoords();

		BufferElement( const BufferElement & p_element);
		BufferElement & operator =( const BufferElement & p_bufferElement);

		template <typename T>
		inline T					&	GetElement	( size_t p_uiIndex)			{ return static_cast<T &>( * m_arrayElements[p_uiIndex]); }
		template <typename T>
		inline const T				&	GetElement	( size_t p_uiIndex)const	{ return static_cast<const T &>( * m_arrayElements[p_uiIndex]); }
		inline size_t					GetStride	()const						{ return m_uiStride; }
		inline unsigned char		*	ptr			()							{ return m_pBuffer; }
		inline const unsigned char	*	const_ptr	()const 					{ return m_pBuffer; }

	protected:
		void _link();
	};

#	include "BufferElement.inl"
}

#endif
