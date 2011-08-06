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
#ifndef ___CU_Serialisable___
#define ___CU_Serialisable___

#include "Buffer.hpp"
#include "Point.hpp"
#include "Matrix.hpp"

#pragma warning( push)
#pragma warning( disable : 4996)

namespace Castor
{	namespace Utils
{
	class Serialisable;
	/*!
	Serialisable helper class, for std::map
	\author Sylvain DOREMUS
	\version 0.6.1.0
	\date 01/04/2011
	*/
	class MapSerialiserBase
	{
	public:
		virtual bool Serialise( File & p_file)const=0;
		virtual bool Unserialise( File & p_file)=0;
	};
	/*!
	Serialisable helper class, for std::vector
	\author Sylvain DOREMUS
	\version 0.6.1.0
	\date 01/04/2011
	*/
	class VectorSerialiserBase
	{
	public:
		virtual bool Serialise( File & p_file)const=0;
		virtual bool Unserialise( File & p_file)=0;
	};
	typedef std::pair <void *, size_t> PtrSizePair;

	template <typename T> class ElementAdder;
	//! Object writable in binary file
	/*!
	This is the main class for objects which can be written into and read from text files
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2011
	*/
	class Serialisable
	{
	private:
		template <typename T> friend class ElementAdder;

		std::vector<	PtrSizePair						 >	m_arrayTypes;
		std::vector<	String							*>	m_arrayStrings;
		std::vector<	Serialisable					*>	m_arraySerialisables;
		std::vector<	shared_ptr<MapSerialiserBase>	 >	m_arrayMaps;
		std::vector<	shared_ptr<VectorSerialiserBase> >	m_arrayVectors;

	protected:
		mutable File	*	m_pFile;
		Serialisable	*	m_pParentSerialisable;

	public:
		Serialisable();
		Serialisable( Serialisable const & p_serialisable);
		Serialisable & operator =( Serialisable const & p_serialisable);
		~Serialisable();
		/**
		 * Serialises the object in a binary archive
		 *@param p_archive : [in] The archive into which the object is written
		 *@param p_uiVersion : [in] The version into which the object is written
		 */
		bool Serialise( File & p_file)const;
		/**
		 * Reads the object from a binary archive
		 *@param p_file : [in] The file from which the object is read
		 *@param p_uiVersion : [in] The version from which the object is read
		 */
		bool Unserialise( File & p_file, Serialisable * p_pParent=NULL);
		/**
		 * Adds an element to serialise
		 */
		template <typename T>
		inline void AddElement( T * p_element);
		/**
		 * Adds a buffer to serialise
		 */
		template <typename T>
		inline void AddElement( Resources::Buffer<T> * p_element);
		/**
		 * Adds a point to serialise
		 */
		template <typename T, size_t Count>
		inline void AddElement( Math::Point<T, Count> * p_element);
		/**
		 * Adds a matrix to serialise
		 */
		template <typename T, size_t Rows, size_t Columns>
		inline void AddElement( Math::Matrix<T, Rows, Columns> * p_element);
		/**
		 * Adds a map to serialise
		 */
		template <typename T>
		inline void AddElement( std::map< String, shared_ptr<T> > * p_map);
		/**
		 * Adds a map to serialise
		 */
		template <typename T>
		inline void AddElement( std::map< real, shared_ptr<T> > * p_map);
		/**
		 * Adds a map to serialise
		 */
		template <typename T>
		inline void AddElement( std::vector< shared_ptr<T> > * p_array);
		/**
		 * Function which creates the map of objects to serialise
		 */
		void CreateSerialiseMap()
		{
			_createSerialiseMap();
		}
		/**
		 * Function executed before serialising
		 */
		virtual void PreSerialise()const{}
		/**
		 * Function executed after serialising
		 */
		virtual void PostSerialise()const{}
		/**
		 * Function executed before unserialising
		 */
		virtual void PreUnserialise(){}
		/**
		 * Function executed after unserialising
		 */
		virtual void PostUnserialise(){}

	protected:
		virtual void _createSerialiseMap(){}
	};

#	define DECLARE_SERIALISE_MAP()	virtual void _createSerialiseMap();
#	define BEGIN_SERIALISE_MAP( class_name, base_class_name)\
	void class_name :: _createSerialiseMap()\
	{\
	base_class_name::_createSerialiseMap();
#	define END_SERIALISE_MAP()\
	}

#	define DECLARE_PRE_SERIALISE()	virtual void PreSerialise()const;
#	define BEGIN_PRE_SERIALISE( class_name, base_class_name)\
	void class_name :: PreSerialise()const\
	{\
		base_class_name::PreSerialise();
#	define END_PRE_SERIALISE()\
	}

#	define DECLARE_POST_SERIALISE()	virtual void PostSerialise()const;
#	define BEGIN_POST_SERIALISE( class_name, base_class_name)\
	void class_name :: PostSerialise()const\
	{\
		base_class_name::PostSerialise();
#	define END_POST_SERIALISE()\
	}

#	define DECLARE_PRE_UNSERIALISE()	virtual void PreUnserialise();
#	define BEGIN_PRE_UNSERIALISE( class_name, base_class_name)\
	void class_name :: PreUnserialise()\
	{\
		base_class_name::PreUnserialise();
#	define END_PRE_UNSERIALISE()\
	}

#	define DECLARE_POST_UNSERIALISE()	virtual void PostUnserialise();
#	define BEGIN_POST_UNSERIALISE( class_name, base_class_name)\
	void class_name :: PostUnserialise()\
	{\
		base_class_name::PostUnserialise();
#	define END_POST_UNSERIALISE()\
	}

#	define ADD_ELEMENT( x) AddElement( & (x));
#	define ADD_POINT( T, C, x) AddElement<T, C>( & (x));
#	define ADD_MATRIX( T, R, C, x) AddElement<T, R, C>( & (x));

#	include "Serialisable.inl"
}
}

#pragma warning( pop)

#endif
