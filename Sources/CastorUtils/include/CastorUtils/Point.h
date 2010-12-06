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
	/*!
	The representation of a 2D Point, with X and Y which can be of a wanted type (like int, real...)
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
		typedef Templates::Value<value_type>		value;

	private:
		bool m_bOwnCoords;

	public:
		value_type * m_coords;

	public:
		Point( const_pointer p_coords = NULL);
		Point( value_type * p_pValues);
		Point( const value_type & p_vA);
		Point( const value_type & p_vA, const value_type & p_vB);
		Point( const value_type & p_vA, const value_type & p_vB, const value_type & p_vC);
		Point( const value_type & p_vA, const value_type & p_vB, const value_type & p_vC, const value_type & p_vD);
		Point( const_point_reference p_point);
		virtual ~Point();
		/**
		 * operators
		 */
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
		/**
		 * Operations
		 */
		void 			Reverse();
		void 			Normalise();
		point 			GetNormalised()const;
		value_type 		Dot( const_point_reference p_vertex)const;
		value_type 		GetSquaredLength()const;
		real 			GetLength()const;
		real 			GetCosTheta( const_point_reference p_vector)const;
		virtual bool 	Write( Utils::File & p_file)const;
		virtual bool 	Read( Utils::File & p_file);
		/**
		 * Accessors
		 */
		void 			LinkCoords( void * p_pCoords);
		void 			UnlinkCoords();
		void 			ToValues( value_type * p_pResult)const;
		const_reference	operator[]( size_t p_pos)const;
		reference		operator[]( size_t p_pos);
		pointer			ptr();
		const_pointer	const_ptr()const;

		template <typename _Ty>
		inline Point <T, Count> operator +=( const Point<_Ty, Count> & p_pt)
		{
			for (size_t i = 0 ; i < Count ; i++)
			{
				value::ass_add( m_coords[i], value_type( p_pt[i]));
			}

			return * this;
		}
		template <size_t _Count>
		inline Point <T, Count> operator +=( const Point<value_type, _Count> & p_pt)
		{
			for (size_t i = 0 ; i < std::min<size_t>( Count, _Count) ; i++)
			{
				value::ass_add( m_coords[i], p_pt[i]);
			}

			return * this;
		}
		template <typename _Ty, size_t _Count>
		inline Point <T, Count> operator +=( const Point<_Ty, _Count> & p_pt)
		{
			for (size_t i = 0 ; i < std::min<size_t>( Count, _Count) ; i++)
			{
				value::ass_add( m_coords[i], value_type( p_pt[i]));
			}

			return * this;
		}
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