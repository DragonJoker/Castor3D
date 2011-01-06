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
#ifndef ___Castor_Point___
#define ___Castor_Point___

#include "File.h"

namespace Castor
{	namespace Math
{
	//! Templated point representation, with any number of coordinates
	/*!
	Can hold any type which has a defined Policy
	\author Sylvain DOREMUS
	\date 14/02/2010
	*/
	template <typename T, size_t Count>
	class Point
	{
	private:
		typedef T									value_type;
		typedef value_type &						reference;
		typedef value_type *						pointer;
		typedef const value_type &					const_reference;
		typedef const value_type *					const_pointer;
		typedef Point<value_type, Count>			point;
		typedef Point<value_type, Count> &			point_reference;
		typedef Point<value_type, Count> *			point_pointer;
		typedef const Point<value_type, Count> &	const_point_reference;
		typedef const Point<value_type, Count> *	const_point_pointer;
		typedef Templates::Policy<value_type>		policy;

	private:
		bool m_bOwnCoords;

	public:
		value_type * m_coords;

	public:
		/**@name Constructors */
		//@{
		Point( const value_type * p_pValues = NULL);
		Point( const value_type & p_vA);
		Point( const value_type & p_vA, const value_type & p_vB);
		Point( const value_type & p_vA, const value_type & p_vB, const value_type & p_vC);
		Point( const value_type & p_vA, const value_type & p_vB, const value_type & p_vC, const value_type & p_vD);
		Point( const_point_reference p_ptPoint);
		template <typename _Ty>
		Point( const _Ty * p_pValues)
			:	m_coords( new T[Count]),
				m_bOwnCoords( true)
		{
			if (p_pValues == NULL)
			{
				for (size_t i = 0 ; i < Count ; i++)
				{
					policy::init( m_coords[i]);
				}
			}
			else
			{
				for (size_t i = 0 ; i < Count ; i++)
				{
					policy::assign( m_coords[i], p_pValues[i]);
				}
			}
		}
		template <typename _Ty>
		Point( const _Ty & p_vA, const _Ty & p_vB)
			:	m_coords( new T[Count]),
				m_bOwnCoords( true)
		{
			if (Count > 0)
			{
				policy::assign( m_coords[0], p_vA);
			}
			if (Count > 1)
			{
				policy::assign( m_coords[1], p_vB);

				for (size_t i = 2 ; i < Count ; i++)
				{
					policy::init( m_coords[i]);
				}
			}
		}
		template <typename _Ty>
		Point( const _Ty & p_vA, const _Ty & p_vB, const _Ty & p_vC)
			:	m_coords( new T[Count]),
				m_bOwnCoords( true)
		{
			if (Count > 0)
			{
				policy::assign( m_coords[0], p_vA);
			}
			if (Count > 1)
			{
				policy::assign( m_coords[1], p_vB);
			}
			if (Count > 2)
			{
				policy::assign( m_coords[2], p_vC);

				for (size_t i = 3 ; i < Count ; i++)
				{
					policy::init( m_coords[i]);
				}
			}
		}
		template <typename _Ty>
		Point( const _Ty & p_vA, const _Ty & p_vB, const _Ty & p_vC, const _Ty & p_vD)
			:	m_coords( new T[Count]),
				m_bOwnCoords( true)
		{
			if (Count > 0)
			{
				policy::assign( m_coords[0], p_vA);
			}
			if (Count > 1)
			{
				policy::assign( m_coords[1], p_vB);
			}
			if (Count > 2)
			{
				policy::assign( m_coords[2], p_vC);
			}
			if (Count > 3)
			{
				policy::assign( m_coords[3], p_vD);

				for (size_t i = 4 ; i < Count ; i++)
				{
					policy::init( m_coords[i]);
				}
			}
		}
		template <typename _Ty, size_t _Count>
		Point( const Point<_Ty, _Count> & p_ptPoint)
			:	m_coords( new T[Count]),
				m_bOwnCoords( true)
		{
			for (size_t i = 0 ; i < std::min<size_t>( Count, _Count) ; i++)
			{
				policy::assign( m_coords[i], p_ptPoint[i]);
			}

			for (size_t i = _Count ; i < Count ; i++)
			{
				policy::init( m_coords[i]);
			}
		}
		virtual ~Point();
		//@}
		/**@name Operators */
		//@{
		point_reference operator +=( const_point_reference p_pt);
		point_reference operator +=( const_pointer p_coords);
		point_reference operator -=( const_point_reference p_pt);
		point_reference operator -=( const_pointer p_coords);
		point_reference operator *=( const_point_reference p_pt);
		point_reference operator *=( const_pointer p_coords);
		point_reference operator *=( const_reference p_value);
		point_reference operator /=( const_reference p_value);
		point_reference operator /=( const_point_reference p_value);
		point_reference operator = ( const_point_reference p_pt);
		point 			operator + ( const_point_reference p_pt)const;
		point 			operator + ( const_reference p_value)const;
		point 			operator - ( const_point_reference p_pt)const;
		point 			operator - ( const_reference p_value)const;
		point 			operator * ( const_point_reference p_pt)const;
		point 			operator * ( const_reference p_value)const;
		point 			operator / ( const_reference p_value)const;
		point 			operator / ( const_point_reference p_value)const;
		bool			operator ==( const_point_reference p_pt)const;
		bool			operator !=( const_point_reference p_pt)const;
		point			operator ^ ( const_point_reference p_vertex)const;
		std::ostream & 	operator <<( std::ostream & l_streamOut);
		std::istream & 	operator >>( std::istream & l_streamIn);
		template <typename _Ty>
		inline Point <T, Count> operator +=( const Point<_Ty, Count> & p_pt)
		{
			for (size_t i = 0 ; i < Count ; i++)
			{
				policy::ass_add<_Ty>( m_coords[i], p_pt[i]);
			}

			return * this;
		}
		template <size_t _Count>
		inline Point <T, Count> operator +=( const Point<value_type, _Count> & p_pt)
		{
			for (size_t i = 0 ; i < std::min<size_t>( Count, _Count) ; i++)
			{
				policy::ass_add( m_coords[i], p_pt[i]);
			}

			return * this;
		}
		template <typename _Ty, size_t _Count>
		inline Point <T, Count> operator +=( const Point<_Ty, _Count> & p_pt)
		{
			for (size_t i = 0 ; i < std::min<size_t>( Count, _Count) ; i++)
			{
				policy::ass_add<_Ty>( m_coords[i], p_pt[i]);
			}

			return * this;
		}

		template <typename _Ty>
		inline Point <T, Count> operator =( const Point<_Ty, Count> & p_pt)
		{
			for (size_t i = 0 ; i < Count ; i++)
			{
				policy::assign<_Ty>( m_coords[i], p_pt[i]);
			}

			return * this;
		}
		template <size_t _Count>
		inline Point <T, Count> operator =( const Point<value_type, _Count> & p_pt)
		{
			for (size_t i = 0 ; i < std::min<size_t>( Count, _Count) ; i++)
			{
				policy::assign( m_coords[i], p_pt[i]);
			}

			if (Count > _Count)
			{
				for (size_t i = _Count ; i < Count ; i++)
				{
					policy::init( m_coords[i]);
				}
			}

			return * this;
		}
		template <typename _Ty, size_t _Count>
		inline Point <T, Count> operator =( const Point<_Ty, _Count> & p_pt)
		{
			for (size_t i = 0 ; i < std::min<size_t>( Count, _Count) ; i++)
			{
				policy::assign<_Ty>( m_coords[i], p_pt[i]);
			}

			if (Count > _Count)
			{
				for (size_t i = _Count ; i < Count ; i++)
				{
					policy::init( m_coords[i]);
				}
			}

			return * this;
		}
		//@}
		/**@name Operations */
		//@{
		void 			Reverse();
		void 			Normalise();
		point 			GetNormalised()const;
		value_type 		Dot( const_point_reference p_vertex)const;
		value_type 		GetSquaredLength()const;
		real 			GetLength()const;
		real 			GetCosTheta( const_point_reference p_vector)const;
		virtual bool 	Write( Utils::File & p_file)const;
		virtual bool 	Read( Utils::File & p_file);
		//@}
		/**@name Accessors */
		//@{
		void 			LinkCoords( const void * p_pCoords);
		void 			UnlinkCoords();
		void 			ToValues( value_type * p_pResult)const;
		const_reference	operator[]( size_t p_pos)const;
		reference		operator[]( size_t p_pos);
		pointer			ptr();
		const_pointer	const_ptr()const;
		//@}
	};

	template <typename T, size_t Count>
	std::ostream & operator << ( std::ostream & l_streamOut, const Point <T, Count> & p_point);
	template <typename T, size_t Count>
	std::istream & operator >> ( std::istream & l_streamIn, Point <T, Count> & p_point);
	template <typename T, size_t Count>
	Point <T, Count> operator *( int p_value, const Point <T, Count> & p_ptPoint);
	template <typename T, size_t Count>
	Point <T, Count> operator +( int p_value, const Point <T, Count> & p_ptPoint);
	template <typename T, size_t Count>
	Point <T, Count> operator -( int p_value, const Point <T, Count> & p_ptPoint);
	template <typename T, size_t Count>
	Point <T, Count> operator /( int p_value, const Point <T, Count> & p_ptPoint);
	template <typename T, size_t Count>
	Point <T, Count> operator *( real p_value, const Point <T, Count> & p_ptPoint);
	template <typename T, size_t Count>
	Point <T, Count> operator +( real p_value, const Point <T, Count> & p_ptPoint);
	template <typename T, size_t Count>
	Point <T, Count> operator -( real p_value, const Point <T, Count> & p_ptPoint);
	template <typename T, size_t Count>
	Point <T, Count> operator /( real p_value, const Point <T, Count> & p_ptPoint);
	template <typename T, size_t Count, typename _Ty>
	Point <T, Count> operator +( const Point <T, Count> & p_ptA, const Point<_Ty, Count> & p_ptB);
	template <typename T, size_t Count, size_t _Count>
	Point <T, Count> operator +( const Point <T, Count> & p_ptA, const Point<T, _Count> & p_ptB);
	template <typename T, size_t Count, typename _Ty, size_t _Count>
	Point <T, Count> operator +( const Point <T, Count> & p_ptA, const Point<_Ty, _Count> & p_ptB);
}
}

#include "Point.inl"

#endif