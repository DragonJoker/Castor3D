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
#ifndef ___Castor_Point___
#define ___Castor_Point___

#include "File.hpp"
#include <cmath>
#include "Serialisable.hpp"

namespace Castor
{	namespace Math
{
	/*!
	Base class for points, used to share functions without taking care of templates
	\author Sylvain DOREMUS
	\version 0.6.1.0
	\date 03/01/2011
	*/
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
		PointBase( const PointBase & p_ptBase)						{}
		virtual PointBase & operator =( const PointBase & p_ptBase)	{ return * this; }
		//@}

		virtual size_t	count		()const=0;
		virtual size_t	elem_size	()const=0;
		virtual void	link		( void const * p_pCoords)=0;
		virtual void 	unlink		()=0;
		inline	size_t	size		()const						{ return count() * elem_size(); }

		template <typename T> T			*	ptr			()		{ return (T *)_ptr(); }
		template <typename T> const T	*	const_ptr	()const	{ return (T const *)_const_ptr(); }

	protected:
		virtual void		*	_ptr		()		{ return NULL; }
		virtual const void	*	_const_ptr	()const	{ return NULL; }
	};

	/*!
	Templated dynamic dimensions point class
	\author Sylvain DOREMUS
	\version 0.6.1.0
	\date 03/01/2011
	*/
	template <typename T>
	class DynPoint : public PointBase
	{
	private:
		template <typename U> friend class DynPoint;

		typedef T								value_type;
		typedef value_type &					reference;
		typedef value_type *					pointer;
		typedef value_type const &				const_reference;
		typedef value_type const *				const_pointer;
		typedef DynPoint<value_type>			point;
		typedef DynPoint<value_type> &			point_reference;
		typedef DynPoint<value_type> *			point_pointer;
		typedef const DynPoint<value_type> &	const_point_reference;
		typedef const DynPoint<value_type> *	const_point_pointer;
		typedef value_type *					iterator;
		typedef value_type const *				const_iterator;

		size_t m_uiCount;
		bool m_bOwnCoords;
		T * m_coords;

	public:
		/**@name Construction / Destruction */
		//@{
		DynPoint( size_t p_uiCount);
		template <typename U> DynPoint( size_t p_uiCount, U const * p_pValues);
		template <typename U> DynPoint( U const & p_vA, U const & p_vB);
		template <typename U> DynPoint( U const & p_vA, U const & p_vB, U const & p_vC);
		template <typename U> DynPoint( U const & p_vA, U const & p_vB, U const & p_vC, U const & p_vD);
		template <typename U> DynPoint( DynPoint<U> const & p_ptPoint);
		virtual ~DynPoint();
		//@}

		//*@name Copy */
		//@{
		DynPoint( DynPoint<T> const & p_ptPoint);
		DynPoint<T> & operator =( DynPoint<T> const & p_pt);
		//@}

		/**@name Operators */
		//@{
		template <typename U> DynPoint<T> & operator =	( DynPoint<U>	const &	p_pt);
		template <typename U> DynPoint<T> & operator +=	( DynPoint<U>	const &	p_pt);
		template <typename U> DynPoint<T> & operator -=	( DynPoint<U>	const &	p_pt);
		template <typename U> DynPoint<T> & operator *=	( DynPoint<U>	const &	p_pt);
		template <typename U> DynPoint<T> & operator /=	( DynPoint<U>	const &	p_pt);
		template <typename U> DynPoint<T> & operator ^=	( DynPoint<U>	const &	p_pt);
		template <typename U> DynPoint<T> & operator +=	( U				const *	p_coords);
		template <typename U> DynPoint<T> & operator -=	( U				const *	p_coords);
		template <typename U> DynPoint<T> & operator *=	( U				const *	p_coords);
		template <typename U> DynPoint<T> & operator /=	( U				const *	p_coords);
		template <typename U> DynPoint<T> & operator +=	( U				const &	p_coord);
		template <typename U> DynPoint<T> & operator -=	( U				const &	p_coord);
		template <typename U> DynPoint<T> & operator *=	( U				const &	p_coord);
		template <typename U> DynPoint<T> & operator /=	( U				const &	p_coord);
		//@}

		/**@name Functions */
		//@{
		void				swap		( DynPoint<T> const & p_pt);
		void				flip		();
		//@}

		/**@name Accessors */
		//@{
		virtual size_t		count		()const						{ return m_uiCount; }
		virtual size_t		elem_size	()const						{ return sizeof( T); }
		virtual void		link		( void const * p_pCoords);
		virtual void		unlink		();
		void 				to_values	( T * p_pResult)const;
		inline T const &	operator[]	( size_t p_pos)const		{ return m_coords[p_pos]; }
		inline T &			operator[]	( size_t p_pos)				{ return m_coords[p_pos]; }
		inline T const &	at			( size_t p_pos)const		{ return m_coords[p_pos]; }
		inline T &			at			( size_t p_pos)				{ return m_coords[p_pos]; }
		inline T *			ptr			()							{ return m_coords; }
		inline T const *	const_ptr	()const						{ return m_coords; }

		iterator			begin		()							{return m_coords;}
		iterator			end			()							{return m_coords + m_uiCount;}
		const_iterator		begin		()const						{return m_coords;}
		const_iterator		end			()const						{return m_coords + m_uiCount;}

	private:
		inline void *		_ptr		()							{ return m_coords; }
		inline void const *	_const_ptr	()const						{ return m_coords; }
		//@}
	};

	template <typename T, typename U	> bool				operator ==	( DynPoint<T>		const	& p_ptA,		DynPoint<U>	const	& p_ptB);
	template <typename T, typename U	> bool				operator !=	( DynPoint<T>		const	& p_ptA,		DynPoint<U>	const	& p_ptB);
	template <typename T				> std::ostream	&	operator <<	( std::ostream				& p_streamOut,	DynPoint<T>	const	& p_pt);
	template <typename T				> std::istream	&	operator >>	( std::istream				& p_streamIn,	DynPoint<T>			& p_pt);
	template <typename T, typename U	> DynPoint<T>		operator +	( DynPoint<T>		const	& p_ptA,		DynPoint<U>	const	& p_ptB);
	template <typename T, typename U	> DynPoint<T>		operator -	( DynPoint<T>		const	& p_ptA,		DynPoint<U>	const	& p_ptB);
	template <typename T, typename U	> DynPoint<T>		operator *	( DynPoint<T>		const	& p_ptA,		DynPoint<U>	const	& p_ptB);
	template <typename T, typename U	> DynPoint<T>		operator /	( DynPoint<T>		const	& p_ptA,		DynPoint<U>	const	& p_ptB);
	template <typename T, typename U	> DynPoint<T>		operator ^	( DynPoint<T>		const	& p_ptA,		DynPoint<U>	const	& p_ptB);
	template <typename T, typename U	> DynPoint<T>		operator +	( DynPoint<T>		const	& p_pt,			U			const	* p_coords);
	template <typename T, typename U	> DynPoint<T>		operator -	( DynPoint<T>		const	& p_pt,			U			const	* p_coords);
	template <typename T, typename U	> DynPoint<T>		operator *	( DynPoint<T>		const	& p_pt,			U			const	* p_coords);
	template <typename T, typename U	> DynPoint<T>		operator /	( DynPoint<T>		const	& p_pt,			U			const	* p_coords);
	template <typename T, typename U	> DynPoint<T>		operator +	( DynPoint<T>		const	& p_pt,			U			const	& p_coord);
	template <typename T, typename U	> DynPoint<T>		operator -	( DynPoint<T>		const	& p_pt,			U			const	& p_coord);
	template <typename T, typename U	> DynPoint<T>		operator *	( DynPoint<T>		const	& p_pt,			U			const	& p_coord);
	template <typename T, typename U	> DynPoint<T>		operator /	( DynPoint<T>		const	& p_pt,			U			const	& p_coord);

	template <typename T				> DynPoint<T>		operator *	( int p_value,							DynPoint<T>	const	& p_pt);
	template <typename T				> DynPoint<T>		operator +	( int p_value,							DynPoint<T>	const	& p_pt);
	template <typename T				> DynPoint<T>		operator -	( int p_value,							DynPoint<T>	const	& p_pt);
	template <typename T				> DynPoint<T>		operator /	( int p_value,							DynPoint<T>	const	& p_pt);
	template <typename T				> DynPoint<T>		operator *	( double p_value,						DynPoint<T>	const	& p_pt);
	template <typename T				> DynPoint<T>		operator +	( double p_value,						DynPoint<T>	const	& p_pt);
	template <typename T				> DynPoint<T>		operator -	( double p_value,						DynPoint<T>	const	& p_pt);
	template <typename T				> DynPoint<T>		operator /	( double p_value,						DynPoint<T>	const	& p_pt);

	//! Templated static dimensions point representation
	/*!
	Can hold any type which has a defined Policy
	\author Sylvain DOREMUS
	\date 14/02/2010
	*/
	template <typename T, size_t Count>
	class Point : public PointBase
	{
	private:
		template <typename U, size_t _Count> friend class Point;

		typedef T									value_type;
		typedef value_type &						reference;
		typedef value_type *						pointer;
		typedef value_type const &					const_reference;
		typedef value_type const *					const_pointer;
		typedef Point<value_type, Count>			point;
		typedef Point<value_type, Count> &			point_reference;
		typedef Point<value_type, Count> *			point_pointer;
		typedef Point<value_type, Count> const &	const_point_reference;
		typedef Point<value_type, Count> const *	const_point_pointer;
		typedef value_type *						iterator;
		typedef value_type const *					const_iterator;

		bool m_bOwnCoords;
		T * m_coords;

	public:
		/**@name Constructors */
		//@{
		Point();
		Point( Point<T, Count> const & p_ptPoint);
		template <typename U> Point( U const * p_pValues);
		template <typename U> Point( U const & p_vA, U const & p_vB);
		template <typename U> Point( U const & p_vA, U const & p_vB, U const & p_vC);
		template <typename U> Point( U const & p_vA, U const & p_vB, U const & p_vC, U const & p_vD);
		virtual ~Point();
		//@}

		/**@name Operators */
		//@{
		Point <T, Count> & operator =( Point<T, Count> const & p_pt);
		template <typename U, size_t _Count	>	Point <T, Count> & operator +=	( Point<U, _Count>	const	& p_pt);
		template <typename U, size_t _Count	>	Point <T, Count> & operator -=	( Point<U, _Count>	const	& p_pt);
		template <typename U, size_t _Count	>	Point <T, Count> & operator *=	( Point<U, _Count>	const	& p_pt);
		template <typename U, size_t _Count	>	Point <T, Count> & operator /=	( Point<U, _Count>	const	& p_pt);
		template <typename U, size_t _Count	>	Point <T, Count> & operator ^=	( Point<U, _Count>	const	& p_pt);
		template <typename U				>	Point <T, Count> & operator +=	( U					const	* p_coords);
		template <typename U				>	Point <T, Count> & operator -=	( U					const	* p_coords);
		template <typename U				>	Point <T, Count> & operator *=	( U					const	* p_coords);
		template <typename U				>	Point <T, Count> & operator /=	( U					const	* p_coords);
												Point <T, Count> & operator +=	( T					const	& p_coord);
												Point <T, Count> & operator -=	( T					const	& p_coord);
												Point <T, Count> & operator *=	( T					const	& p_coord);
												Point <T, Count> & operator /=	( T					const	& p_coord);
		//@}

		/**@name Operations */
		//@{
		void 				negate				();
		void 				normalise			();
		point 				get_normalised		()const;
		value_type 			dot					( const_point_reference p_vertex)const;
		double 				length_squared		()const;
		double 				length				()const;
		double 				length_manhattan	()const;
		double 				length_minkowski	( double p_dOrder)const;
		double 				length_chebychev	()const;
		double 				cos_theta			( const_point_reference p_vector)const;
		void				link				( void const * p_pCoords);
		void				unlink				();
		void				swap				( Point<T, Count> & p_pt);
		void				flip				();
		/**
		 * Creates a clone of this Point.
		 * Clone's coords point to this Point's coords
		 */
		void				clone				( Point<T, Count> const & p_ptPoint);
		/**
		 * Mix between Point::clone and Point::copy.
		 * If this Point owns his coords, allocates new coords for the clone
		 * If this Point doesn't own his coords, clone's coords point to this Point's coords
		 */
		void				clopy				( Point<T, Count> const & p_ptPoint);
		/**
		 * Copies point coords into this one's
		 */
		void				copy				( Point<T, Count> const & p_ptPoint);
		bool				Save				( Castor::Utils::File & p_file)const;
		bool				Load				( Castor::Utils::File & p_file);
		//@}

		/**@name Accessors */
		//@{
		void 				to_values			( T * p_pResult)const;
		inline size_t		count				()const					{ return Count; }
		inline size_t		elem_size			()const					{ return sizeof( T); }
		inline T const &	operator[]			( size_t p_pos)const	{ return m_coords[p_pos]; }
		inline T &			operator[]			( size_t p_pos)			{ return m_coords[p_pos]; }
		inline T const &	at					( size_t p_pos)const	{ return m_coords[p_pos]; }
		inline T &			at					( size_t p_pos)			{ return m_coords[p_pos]; }
		inline T *			ptr					()						{ return m_coords; }
		inline T const *	const_ptr			()const					{ return m_coords; }

		iterator			begin				()						{return m_coords;}
		iterator			end					()						{return m_coords + Count;}
		const_iterator		begin				()const					{return m_coords;}
		const_iterator		end					()const					{return m_coords + Count;}

	private:
		inline void *		_ptr				()						{ return m_coords; }
		inline void const *	_const_ptr			()const					{ return m_coords; }
		//@}
	};

	template <typename T, size_t Count, typename U, size_t _Count	> bool					operator ==	( Point<T, Count>	const	& p_ptA,		Point<U, _Count>	const	& p_ptB);
	template <typename T, size_t Count, typename U, size_t _Count	> bool					operator !=	( Point<T, Count>	const	& p_ptA,		Point<U, _Count>	const	& p_ptB);
	template <typename T, size_t Count								> std::ostream		&	operator <<	( std::ostream				& p_streamOut,	Point<T, Count>		const	& p_pt);
	template <typename T, size_t Count								> std::istream		&	operator >>	( std::istream				& p_streamIn,	Point<T, Count>				& p_pt);
	template <typename T, size_t Count, typename U, size_t _Count	> Point <T, Count>		operator +	( Point<T, Count>	const	& p_ptA,		Point<U, _Count>	const	& p_ptB);
	template <typename T, size_t Count, typename U, size_t _Count	> Point <T, Count>		operator -	( Point<T, Count>	const	& p_ptA,		Point<U, _Count>	const	& p_ptB);
	template <typename T, size_t Count, typename U, size_t _Count	> Point <T, Count>		operator *	( Point<T, Count>	const	& p_ptA,		Point<U, _Count>	const	& p_ptB);
	template <typename T, size_t Count, typename U, size_t _Count	> Point <T, Count>		operator /	( Point<T, Count>	const	& p_ptA,		Point<U, _Count>	const	& p_ptB);
	template <typename T, size_t Count, typename U, size_t _Count	> Point <T, Count>		operator ^	( Point<T, Count>	const	& p_ptA,		Point<U, _Count>	const	& p_ptB);
	template <typename T, size_t Count, typename U					> Point <T, Count>		operator +	( Point<T, Count>	const	& p_pt,			U					const	* p_coords);
	template <typename T, size_t Count, typename U					> Point <T, Count>		operator -	( Point<T, Count>	const	& p_pt,			U					const	* p_coords);
	template <typename T, size_t Count, typename U					> Point <T, Count>		operator *	( Point<T, Count>	const	& p_pt,			U					const	* p_coords);
	template <typename T, size_t Count, typename U					> Point <T, Count>		operator /	( Point<T, Count>	const	& p_pt,			U					const	* p_coords);
	template <typename T, size_t Count								> Point <T, Count>		operator +	( Point<T, Count>	const	& p_pt,			T					const	& p_coord);
	template <typename T, size_t Count								> Point <T, Count>		operator -	( Point<T, Count>	const	& p_pt,			T					const	& p_coord);
	template <typename T, size_t Count								> Point <T, Count>		operator *	( Point<T, Count>	const	& p_pt,			T					const	& p_coord);
	template <typename T, size_t Count								> Point <T, Count>		operator /	( Point<T, Count>	const	& p_pt,			T					const	& p_coord);

	template <typename T, size_t Count								> Point <T, Count>		operator *	( int p_value,								Point <T, Count>	const	& p_pt);
	template <typename T, size_t Count								> Point <T, Count>		operator +	( int p_value,								Point <T, Count>	const	& p_pt);
	template <typename T, size_t Count								> Point <T, Count>		operator -	( int p_value,								Point <T, Count>	const	& p_pt);
	template <typename T, size_t Count								> Point <T, Count>		operator /	( int p_value,								Point <T, Count>	const	& p_pt);
	template <typename T, size_t Count								> Point <T, Count>		operator *	( double p_value,							Point <T, Count>	const	& p_pt);
	template <typename T, size_t Count								> Point <T, Count>		operator +	( double p_value,							Point <T, Count>	const	& p_pt);
	template <typename T, size_t Count								> Point <T, Count>		operator -	( double p_value,							Point <T, Count>	const	& p_pt);
	template <typename T, size_t Count								> Point <T, Count>		operator /	( double p_value,							Point <T, Count>	const	& p_pt);
}
}

#include "Point.inl"

#endif
