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
#include <math.h>
#include "Serialisable.h"

namespace Castor
{	namespace Math
{
	class PointBase
	{
	public:
		/**@name Construction / Destruction */
		//@{
		PointBase(){}
		virtual ~PointBase(){}
		//@}

		/**@name Copy */
		//@{
		PointBase( const PointBase & p_ptBase){}
		virtual PointBase & operator =( const PointBase & p_ptBase){ return * this; }
		//@}

		virtual size_t			GetElementCount()const{ return 0; }
		virtual size_t			GetElementSize()const{ return 0; }
		virtual void			LinkCoords( const void * p_pCoords){}
		virtual void 			UnlinkCoords(){}

		template <typename T> T			*	ptr			()		{ return (T *)_ptr(); }
		template <typename T> const T	*	const_ptr	()const	{ return (const T *)_const_ptr(); }

	protected:
		virtual void		*	_ptr() { return NULL; }
		virtual const void	*	_const_ptr()const { return NULL; }
	};

	template <typename T>
	class TPointBase : public PointBase
	{
	private:
		template <typename _Ty> friend class TPointBase;
		bool m_bOwnCoords;
		T * m_coords;
		size_t m_uiCount;

	public:
		/**@name Construction / Destruction */
		//@{
		TPointBase( size_t p_uiCount);
		TPointBase( size_t p_uiCount, const T * p_pData);
		virtual ~TPointBase();
		//@}

		/**@name Copy */
		//@{
		template <typename _Ty> explicit TPointBase( const TPointBase<_Ty> & p_ptBase);
		template <typename _Ty> TPointBase<T> & operator =( const TPointBase<_Ty> & p_ptBase);
		//@}

		/**@name Accessors */
		//@{
		virtual size_t	GetElementCount()const { return m_uiCount; }
		virtual size_t	GetElementSize()const { return sizeof( T); }
		virtual void	LinkCoords( const void * p_pCoords);
		virtual void	UnlinkCoords();
		void 			ToValues( T * p_pResult)const;
		const T		&	operator[]( size_t p_pos)const;
		T			&	operator[]( size_t p_pos);
		const T		&	at( size_t p_pos)const;
		T			&	at( size_t p_pos);
		T			*	ptr			()		{ return m_coords; }
		const T		*	const_ptr	()const	{ return m_coords; }

	private:
		virtual void		*	_ptr		()		{ return m_coords; }
		virtual const void	*	_const_ptr	()const	{ return m_coords; }
		//@}
	};
	//! Templated point representation, with any number of coordinates
	/*!
	Can hold any type which has a defined Policy
	\author Sylvain DOREMUS
	\date 14/02/2010
	*/
	template <typename T, size_t Count>
	class Point : public TPointBase<T>, public Utils::Serialisable
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

	public:
		/**@name Constructors */
		//@{
		Point( const value_type * p_pValues = NULL);
		Point( const value_type & p_vA);
		Point( const value_type & p_vA, const value_type & p_vB);
		Point( const value_type & p_vA, const value_type & p_vB, const value_type & p_vC);
		Point( const value_type & p_vA, const value_type & p_vB, const value_type & p_vC, const value_type & p_vD);
		Point( const_point_reference p_ptPoint);
		template <typename _Ty> Point( const _Ty * p_pValues);
		template <typename _Ty> Point( const _Ty & p_vA, const _Ty & p_vB);
		template <typename _Ty> Point( const _Ty & p_vA, const _Ty & p_vB, const _Ty & p_vC);
		template <typename _Ty> Point( const _Ty & p_vA, const _Ty & p_vB, const _Ty & p_vC, const _Ty & p_vD);
		template <typename _Ty, size_t _Count> Point( const Point<_Ty, _Count> & p_ptPoint);
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
		template <typename _Ty> Point <T, Count> operator +=( const Point<_Ty, Count> & p_pt);
		template <size_t _Count> Point <T, Count> operator +=( const Point<value_type, _Count> & p_pt);
		template <typename _Ty, size_t _Count> Point <T, Count> operator +=( const Point<_Ty, _Count> & p_pt);
		template <typename _Ty> Point <T, Count> operator =( const Point<_Ty, Count> & p_pt);
		template <size_t _Count> Point <T, Count> operator =( const Point<value_type, _Count> & p_pt);
		template <typename _Ty, size_t _Count> Point <T, Count> operator =( const Point<_Ty, _Count> & p_pt);
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
		virtual bool 	Save( Utils::File & p_file)const;
		virtual bool 	Load( Utils::File & p_file);
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
