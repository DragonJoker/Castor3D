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
#include <cstdarg>

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

	public:
		value_type m_coords[Count];

	public:
		/**
		 * Constructor from T array
		 */
		Point( const_pointer p_coords = NULL)
		{
			if (p_coords == NULL)
			{
				for (size_t i = 0 ; i < Count ; i++)
				{
					value::init( m_coords[i]);
				}
			}
			else
			{
				for (size_t i = 0 ; i < Count ; i++)
				{
					value::assign( m_coords[i], p_coords[i]);
				}
			}
		}
		/**
		 * Constructor from at least one coord
		 */
		Point( value_type p_vA)
		{
			if (Count >= 1)
			{
				value::assign( m_coords[0], p_vA);

				for (size_t i = 1 ; i < Count ; i++)
				{
					value::init( m_coords[i]);
				}
			}
		}
		/**
		 * Constructor from at least one coord
		 */
		Point( value_type p_vA, value_type p_vB)
		{
			if (Count >= 2)
			{
				value::assign( m_coords[0], p_vA);
				value::assign( m_coords[1], p_vB);

				for (size_t i = 2 ; i < Count ; i++)
				{
					value::init( m_coords[i]);
				}
			}
		}
		/**
		 * Constructor from at least one coord
		 */
		Point( value_type p_vA, value_type p_vB, value_type p_vC)
		{
			if (Count >= 3)
			{
				value::assign( m_coords[0], p_vA);
				value::assign( m_coords[1], p_vB);
				value::assign( m_coords[2], p_vC);

				for (size_t i = 3 ; i < Count ; i++)
				{
					value::init( m_coords[i]);
				}
			}
		}
		/**
		 * Constructor from at least one coord
		 */
		Point( value_type p_vA, value_type p_vB, value_type p_vC, value_type p_vD)
		{
			if (Count >= 4)
			{
				value::assign( m_coords[0], p_vA);
				value::assign( m_coords[1], p_vB);
				value::assign( m_coords[2], p_vC);
				value::assign( m_coords[3], p_vD);

				for (size_t i = 4 ; i < Count ; i++)
				{
					value::init( m_coords[i]);
				}
			}
		}
		/**
		 * Constructor from another point
		 */
		Point( const_point_reference p_point)
		{
			for (size_t i = 0 ; i < Count ; i++)
			{
				value::assign( m_coords[i], p_point[i]);
			}
		}
		/**
		* Constructor from the difference between second and first argument
		*
		Point( const_point_reference p_v1, const_point_reference p_v2)
		{
			for (size_t i = 0 ; i < Count ; i++)
			{
				value::assign( m_coords[i], value::substract( p_v2[i], p_v1[i]));
			}
		}
		/**
		 * Constructor from at least one coord
		 */
		void ToValues( value_type * a, ...)const
		{
			if (Count >= 1)
			{
				value::assign( * a, m_coords[0]);

				va_list l_list;
				va_start( l_list, a);

				for (size_t i = 1 ; i < Count ; i++)
				{
					value::assign( (* va_arg( l_list, value_type *)), m_coords[i]);
				}

				va_end( l_list);
			}
		}
		/**
		 * += operators
		 */
		inline void operator +=( const_point_reference p_pt)
		{
			for (size_t i = 0 ; i < Count ; i++)
			{
				value::ass_add( m_coords[i], p_pt[i]);
			}
		}
		inline void operator +=( pointer p_coords)
		{
			for (size_t i = 0 ; i < Count ; i++)
			{
				value::ass_add( m_coords[i], p_coords[i]);
			}
		}
		/**
		 * -= operators
		 */
		inline void operator -=( const_point_reference p_pt)
		{
			for (size_t i = 0 ; i < Count ; i++)
			{
				value::ass_substract( m_coords[i], p_pt[i]);
			}
		}
		inline void operator -=( pointer p_coords)
		{
			for (size_t i = 0 ; i < Count ; i++)
			{
				value::ass_substract( m_coords[i], p_coords[i]);
			}
		}
		/**
		 * *= operators
		 */
		inline void operator *=( const_point_reference p_pt)
		{
			for (size_t i = 0 ; i < Count ; i++)
			{
				value::ass_multiply( m_coords[i], p_pt[i]);
			}
		}
		inline void operator *=( pointer p_coords)
		{
			for (size_t i = 0 ; i < Count ; i++)
			{
				value::ass_multiply( m_coords[i], p_coords[i]);
			}
		}
		inline void operator *=( value_type p_value)
		{
			for (size_t i = 0 ; i < Count ; i++)
			{
				value::ass_multiply( m_coords[i], p_value);
			}
		}
		/**
		 * /= operators
		 */
		inline void operator /=( value_type p_value)
		{
			if ( ! value::is_null( p_value))
			{
				for (size_t i = 0 ; i < Count ; i++)
				{
					value::ass_divide( m_coords[i], p_value);
				}
			}
		}
		/**
		 * = operators
		 */
		inline point operator =( const_point_reference p_pt)
		{
			for (size_t i = 0 ; i < Count ; i++)
			{
				value::assign( m_coords[i], p_pt[i]);
			}

			return *this;
		}
		/**
		 * + operators
		 */
		inline point operator +( const_point_reference p_pt)const
		{
			point l_ptReturn;

			for (size_t i = 0 ; i < Count ; i++)
			{
				value::assign( l_ptReturn[i], value::add( m_coords[i], p_pt[i]));
			}

			return l_ptReturn;
		}
		inline point operator +( value_type p_value)const
		{
			point l_ptReturn;

			for (size_t i = 0 ; i < Count ; i++)
			{
				value::assign( l_ptReturn[i], value::add( m_coords[i], p_value));
			}

			return l_ptReturn;
		}
		/**
		 * - operators
		 */
		inline point operator -( const_point_reference p_pt)const
		{
			point l_ptReturn;

			for (size_t i = 0 ; i < Count ; i++)
			{
				value::assign( l_ptReturn[i], value::substract( m_coords[i], p_pt[i]));
			}

			return l_ptReturn;
		}
		inline point operator -( value_type p_value)const
		{
			point l_ptReturn;

			for (size_t i = 0 ; i < Count ; i++)
			{
				value::assign( l_ptReturn[i], value::substract( m_coords[i], p_value));
			}

			return l_ptReturn;
		}
		/**
		 * * operators
		 */
		inline point operator *( const_point_reference p_pt)const
		{
			point l_ptReturn;

			for (size_t i = 0 ; i < Count ; i++)
			{
				value::assign( l_ptReturn[i], value::multiply( m_coords[i], p_pt[i]));
			}

			return l_ptReturn;
		}
		inline point operator *( value_type p_value)const
		{
			point l_ptReturn( * this);

			for (size_t i = 0 ; i < Count ; i++)
			{
				value::assign( l_ptReturn[i], value::multiply( m_coords[i], p_value));
			}

			return l_ptReturn;
		}
		/**
		 * / operators
		 */
		inline point operator /( value_type p_value)const
		{
			point l_ptReturn( * this);

			if ( ! value::is_null( p_value))
			{
				for (size_t i = 0 ; i < Count ; i++)
				{
					value::assign( l_ptReturn[i], value::divide( m_coords[i], p_value));
				}
			}

			return l_ptReturn;
		}
		/**
		 * == operators
		 */
		inline bool operator ==( const_point_reference p_pt)const
		{
			bool l_bReturn = true;

			for (size_t i = 0 ; i < Count && l_bReturn ; i++)
			{
				l_bReturn = value::equals( m_coords[i], p_pt[i]);
			}

			return l_bReturn;
		}
		/**
		 * != operators
		 */
		inline bool operator !=( const_point_reference p_pt)const
		{
			bool l_bReturn = true;

			for (size_t i = 0 ; i < Count && l_bReturn ; i++)
			{
				l_bReturn = ! value::equals( m_coords[i], p_pt[i]);
			}

			return l_bReturn;
		}
		/**
		* Vectorial product
		*/
		inline point operator ^( const_point_reference p_vertex)const
		{
			point l_result( * this);

			if (Count == 3)
			{
				value_type l_x, l_y, l_z, x, y, z;
				p_vertex.ToValues( & l_x, & l_y, & l_z);
				ToValues( & x, & y, & z);
				value::assign( l_result[0], value::substract( value::multiply( y, l_z), value::multiply( l_y, z)));
				value::assign( l_result[1], value::substract( value::multiply( z, l_x), value::multiply( l_z, x)));
				value::assign( l_result[2], value::substract( value::multiply( x, l_y), value::multiply( l_x, y)));
			}

			return l_result;
		}
		inline void Reverse()
		{
			for (size_t i = 0 ; i < Count ; i++)
			{
				value::ass_negate( m_coords[i]);
			}
		}
		inline void Normalise()
		{
			value_type l_length = GetLength();

			if (value::is_null( l_length))
			{
				return;
			}

			operator /=( l_length);
		}
		inline point GetNormalised()const
		{
			point l_ptReturn( * this);
			l_ptReturn.Normalise();
			return l_ptReturn;
		}
		inline point GetNormal( const_point_reference p_vertex)const
		{
			point l_ptReturn = this->operator ^( p_vertex);
			l_ptReturn.Normalise();
			return l_ptReturn;
		}
		inline value_type dotProduct( const_point_reference p_vertex)const
		{
			value_type l_tReturn;
			value::init( l_tReturn);

			for (size_t i = 0 ; i < Count ; i++)
			{
				value::ass_add( l_tReturn, value::multiply( m_coords[i], p_vertex[i]));
			}

			return l_tReturn;
		}
		inline value_type GetSquaredLength()const
		{
			value_type l_tReturn;
			value::init( l_tReturn);

			for (size_t i = 0 ; i < Count ; i++)
			{
				value::ass_add( l_tReturn, value::multiply( m_coords[i], m_coords[i]));
			}

			return l_tReturn;
		}
		inline real GetLength()const
		{
			return sqrt( (real)GetSquaredLength());
		}
		inline real GetCosTheta( const_point_reference p_vector)const
		{
			return dotProduct( p_vector) / (GetLength() * p_vector.GetLength());
		}

		virtual bool Write( Utils::File & p_file)const
		{
			if ( ! p_file.WriteArray<value_type>( m_coords, Count))
			{
				return false;
			}

			return true;
		}

		virtual bool Read( Utils::File & p_file)
		{
			if ( ! p_file.ReadArray<value_type>( m_coords, Count))
			{
				return false;
			}

			return true;
		}
		const_reference operator[]( size_t p_pos)const
		{
			return (*(m_coords + p_pos));
		}
		reference operator[]( size_t p_pos)
		{
			return (*(m_coords + p_pos));
		}
		inline pointer ptr()
		{
			return m_coords;
		}
		inline const_pointer const_ptr()const
		{
			return m_coords;
		}
		std::ostream & operator << ( std::ostream & l_streamOut)
		{
			for (size_t i = 0 ; i < Count ; i++)
			{
				l_streamOut << "\t" << m_coords[i];
			}

			l_streamOut << std::endl;
			return l_streamOut;
		}
		std::istream & operator >> ( std::istream & l_streamIn)
		{
			for (size_t i = 0 ; i < Count ; i++)
			{
				l_streamIn >> m_coords[i];
			}

			return l_streamIn;
		}
	};

	template <typename T, size_t Count>
	std::ostream & operator << ( std::ostream & l_streamOut, const Point <T, Count> & p_point)
	{
		for (size_t i = 0 ; i < Count ; i++)
		{
			l_streamOut << "\t" << p_point[i];
		}

		l_streamOut << std::endl;
		return l_streamOut;
	}
	template <typename T, size_t Count>
	std::istream & operator >> ( std::istream & l_streamIn, Point <T, Count> & p_point)
	{
		for (size_t i = 0 ; i < Count ; i++)
		{
			l_streamIn >> p_point[i];
		}

		return l_streamIn;
	}
}
}

#endif