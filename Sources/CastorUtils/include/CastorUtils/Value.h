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
#ifndef ___Castor_Value___
#define ___Castor_Value___

#include <limits>

namespace Castor
{ namespace Templates
{
	template<class T, class U> struct is_same
	{
		static const bool value = false;
	};

	template<class T> struct is_same<T,T>
	{
		static const bool value = true;
	};

	template <typename T>
	struct CallTraits
	{
		template <typename U, bool Big> struct CallTraitsImpl;

		template <typename U>
		struct CallTraitsImpl<U, true>
		{
			typedef const U& Type;
		};

		template <typename U>
		struct CallTraitsImpl<U, false>
		{
			typedef U Type;
		};

		typedef typename CallTraitsImpl<T, (sizeof(T) > 8)>::Type ParamType;
	};

	template <typename T> class Value;

	template <> class Value<char>
	{
	public:
		static char	zero			()										{ return 0; }
		static char	unit			()										{ return 1; }
		static bool	equals			( const char & p_a, const char & p_b)	{ return p_a == p_b; }
		static char	assign			( char & p_a, const char & p_b)			{ return p_a = p_b; }
		static char	add				( const char & p_a, const char & p_b)	{ return p_a + p_b; }
		static char	substract		( const char & p_a, const char & p_b)	{ return p_a - p_b; }
		static char	multiply		( const char & p_a, const char & p_b)	{ return p_a; }
		static char	divide			( const char & p_a, const char & p_b)	{ return p_a; }
		static char	negate			( const char & p_a)						{ return -p_a; }
		static void	init			( char & p_a)							{ p_a = zero(); }
		static bool	is_null			( const char & p_a)						{ return equals( p_a, zero()); }
		static char	ass_add			( char & p_a, const char & p_b)			{ return assign( p_a, add( p_a, p_b)); }
		static char	ass_substract	( char & p_a, const char & p_b)			{ return assign( p_a, substract( p_a, p_b)); }
		static char	ass_multiply	( char & p_a, const char & p_b)			{ return assign( p_a, multiply( p_a, p_b)); }
		static char	ass_divide		( char & p_a, const char & p_b)			{ return assign( p_a, divide( p_a, p_b)); }
		static char	ass_negate		( char & p_a)							{ return assign( p_a, negate( p_a)); }
		static void	stick			( char & p_a)							{}
		template <typename Ty> 
		static char	assign			( char & p_a, const Ty & p_b)			{ return p_a = static_cast<char>( p_b); }
		template <>
		static char	assign<float>	( char & p_a, const float & p_b)		{ return p_a = char( int( p_b)); }
		template <>
		static char	assign<double>	( char & p_a, const double & p_b)		{ return p_a = char( int( p_b)); }
		template <>
		static char	assign<bool>	( char & p_a, const bool & p_b)			{ return p_a = (p_b ? 1 : 0); }
	};

	template <> class Value<unsigned char>
	{
	public:
		static unsigned char	zero			()														{ return 0; }
		static unsigned char	unit			()														{ return 1; }
		static bool				equals			( const unsigned char & p_a, const unsigned char & p_b)	{ return p_a == p_b; }
		static unsigned char	assign			( unsigned char & p_a, const unsigned char & p_b)		{ return p_a = p_b; }
		static unsigned char	add				( const unsigned char & p_a, const unsigned char & p_b)	{ return p_a + p_b; }
		static unsigned char	substract		( const unsigned char & p_a, const unsigned char & p_b)	{ return p_a - p_b; }
		static unsigned char	multiply		( const unsigned char & p_a, const unsigned char & p_b)	{ return p_a; }
		static unsigned char	divide			( const unsigned char & p_a, const unsigned char & p_b)	{ return p_a; }
		static unsigned char	negate			( const unsigned char & p_a)							{ return p_a; }
		static void				init			( unsigned char & p_a)									{ p_a = zero(); }
		static bool				is_null			( const unsigned char & p_a)							{ return equals( p_a, zero()); }
		static unsigned char	ass_add			( unsigned char & p_a, const unsigned char & p_b)		{ return assign( p_a, add( p_a, p_b)); }
		static unsigned char	ass_substract	( unsigned char & p_a, const unsigned char & p_b)		{ return assign( p_a, substract( p_a, p_b)); }
		static unsigned char	ass_multiply	( unsigned char & p_a, const unsigned char & p_b)		{ return assign( p_a, multiply( p_a, p_b)); }
		static unsigned char	ass_divide		( unsigned char & p_a, const unsigned char & p_b)		{ return assign( p_a, divide( p_a, p_b)); }
		static unsigned char	ass_negate		( unsigned char & p_a)									{ return assign( p_a, negate( p_a)); }
		static unsigned char	stick			( unsigned char & p_a)									{}
		template <typename Ty> 
		static unsigned char	assign			( unsigned char & p_a, const Ty & p_b)					{ return p_a = static_cast<unsigned char>( p_b); }
		template <> 
		static unsigned char	assign<float>	( unsigned char & p_a, const float & p_b)				{ return p_a = unsigned char( unsigned int( p_b)); }
		template <> 
		static unsigned char	assign<double>	( unsigned char & p_a, const double & p_b)				{ return p_a = unsigned char( unsigned int( p_b)); }
		template <> 
		static unsigned char	assign<bool>	( unsigned char & p_a, const bool & p_b)				{ return p_a = (p_b ? 1 : 0); }
	};

	template <> class Value<bool>
	{
	public:
		static bool	zero			()										{ return false; }
		static bool	unit			()										{ return true; }
		static bool	equals			( const bool & p_a, const bool & p_b)	{ return p_a == p_b; }
		static bool	assign			( bool & p_a, const bool & p_b)			{ return p_a = p_b; }
		static bool	add				( const bool & p_a, const bool & p_b)	{ return p_a || p_b; }
		static bool	substract		( const bool & p_a, const bool & p_b)	{ return p_a || p_b; }
		static bool	multiply		( const bool & p_a, const bool & p_b)	{ return p_a && p_b; }
		static bool	divide			( const bool & p_a, const bool & p_b)	{ return p_a && p_b; }
		static bool	negate			( const bool & p_a)						{ return ! p_a; }
		static void	init			( bool & p_a)							{ p_a = zero(); }
		static bool	is_null			( const bool & p_a)						{ return equals( p_a, zero()); }
		static bool	ass_add			( bool & p_a, const bool & p_b)			{ return assign( p_a, add( p_a, p_b)); }
		static bool	ass_substract	( bool & p_a, const bool & p_b)			{ return assign( p_a, substract( p_a, p_b)); }
		static bool	ass_multiply	( bool & p_a, const bool & p_b)			{ return assign( p_a, multiply( p_a, p_b)); }
		static bool	ass_divide		( bool & p_a, const bool & p_b)			{ return assign( p_a, divide( p_a, p_b)); }
		static bool	ass_negate		( bool & p_a)							{ return assign( p_a, negate( p_a)); }
		static void stick			( bool & p_a)							{}
		template <typename Ty> 
		static bool	assign			( bool & p_a, const Ty & p_b)			{ return p_a = static_cast<bool>( p_b); }
		template <>
		static bool	assign<float>	( bool & p_a, const float & p_b)		{ return p_a = bool( int( p_b) != 0); }
		template <>
		static bool	assign<double>	( bool & p_a, const double & p_b)		{ return p_a = bool( int( p_b) != 0); }
	};

	template <> class Value<int>
	{
	public:
		static int	zero			()									{ return 0; }
		static int	unit			()									{ return 1; }
		static bool	equals			( const int & p_a, const int & p_b)	{ return p_a == p_b; }
		static int	assign			( int & p_a, const int & p_b)		{ return p_a = p_b; }
		static int	add				( const int & p_a, const int & p_b)	{ return p_a + p_b; }
		static int	substract		( const int & p_a, const int & p_b)	{ return p_a - p_b; }
		static int	multiply		( const int & p_a, const int & p_b)	{ return p_a * p_b; }
		static int	divide			( const int & p_a, const int & p_b)	{ return p_a / p_b; }
		static int	negate			( const int & p_a)					{ return -p_a; }
		static void	init			( int & p_a)						{ p_a = zero(); }
		static bool	is_null			( const int & p_a)					{ return equals( p_a, zero()); }
		static int	ass_add			( int & p_a, const int & p_b)		{ return assign( p_a, add( p_a, p_b)); }
		static int	ass_substract	( int & p_a, const int & p_b)		{ return assign( p_a, substract( p_a, p_b)); }
		static int	ass_multiply	( int & p_a, const int & p_b)		{ return assign( p_a, multiply( p_a, p_b)); }
		static int	ass_divide		( int & p_a, const int & p_b)		{ return assign( p_a, divide( p_a, p_b)); }
		static int	ass_negate		( int & p_a)						{ return assign( p_a, negate( p_a)); }
		static void stick			( int & p_a)						{}
		template <typename Ty> 
		static int	assign			( int & p_a, const Ty & p_b)		{ return p_a = static_cast<int>( p_b); }
		template <> 
		static int	assign<bool>	( int & p_a, const bool & p_b)		{ return p_a = (p_b ? 1 : 0); }
	};

	template <> class Value<unsigned int>
	{
	public:
		static unsigned int	zero			()														{ return 0; }
		static unsigned int	unit			()														{ return 1; }
		static bool			equals			( const unsigned int & p_a, const unsigned int & p_b)	{ return p_a == p_b; }
		static unsigned int	assign			( unsigned int & p_a, const unsigned int & p_b)			{ return p_a = p_b; }
		static unsigned int	add				( const unsigned int & p_a, const unsigned int & p_b)	{ return p_a + p_b; }
		static unsigned int	substract		( const unsigned int & p_a, const unsigned int & p_b)	{ return p_a - p_b; }
		static unsigned int	multiply		( const unsigned int & p_a, const unsigned int & p_b)	{ return p_a * p_b; }
		static unsigned int	divide			( const unsigned int & p_a, const unsigned int & p_b)	{ return p_a / p_b; }
		static unsigned int	negate			( const unsigned int & p_a)								{ return p_a; }
		static void	init					( unsigned int & p_a)									{ p_a = zero(); }
		static bool	is_null					( const unsigned int & p_a)								{ return equals( p_a, zero()); }
		static unsigned int	ass_add			( unsigned int & p_a, const unsigned int & p_b)			{ return assign( p_a, add( p_a, p_b)); }
		static unsigned int	ass_substract	( unsigned int & p_a, const unsigned int & p_b)			{ return assign( p_a, substract( p_a, p_b)); }
		static unsigned int	ass_multiply	( unsigned int & p_a, const unsigned int & p_b)			{ return assign( p_a, multiply( p_a, p_b)); }
		static unsigned int	ass_divide		( unsigned int & p_a, const unsigned int & p_b)			{ return assign( p_a, divide( p_a, p_b)); }
		static unsigned int	ass_negate		( unsigned int & p_a)									{ return assign( p_a, negate( p_a)); }
		static void			stick			( unsigned int & p_a)									{}
		template <typename Ty> 
		static unsigned int	assign			( unsigned int & p_a, const Ty & p_b)					{ return p_a = static_cast<unsigned int>( p_b); }
		template <> 
		static unsigned int	assign<bool>	( unsigned int & p_a, const bool & p_b)					{ return p_a = (p_b ? 1 : 0); }
	};

	template <> class Value<long>
	{
	public:
		static long	zero			()										{ return 0; }
		static long	unit			()										{ return 1; }
		static bool	equals			( const long & p_a, const long & p_b)	{ return p_a == p_b; }
		static long assign			( long & p_a, const long & p_b)			{ return p_a = p_b; }
		static long	add				( const long & p_a, const long & p_b)	{ return p_a + p_b; }
		static long	substract		( const long & p_a, const long & p_b)	{ return p_a - p_b; }
		static long	multiply		( const long & p_a, const long & p_b)	{ return p_a * p_b; }
		static long	divide			( const long & p_a, const long & p_b)	{ return p_a / p_b; }
		static long	negate			( const long & p_a)						{ return -p_a; }
		static void	init			( long & p_a)							{ p_a = zero(); }
		static bool	is_null			( const long & p_a)						{ return equals( p_a, zero()); }
		static long	ass_add			( long & p_a, const long & p_b)			{ return assign( p_a, add( p_a, p_b)); }
		static long	ass_substract	( long & p_a, const long & p_b)			{ return assign( p_a, substract( p_a, p_b)); }
		static long	ass_multiply	( long & p_a, const long & p_b)			{ return assign( p_a, multiply( p_a, p_b)); }
		static long	ass_divide		( long & p_a, const long & p_b)			{ return assign( p_a, divide( p_a, p_b)); }
		static long	ass_negate		( long & p_a)							{ return assign( p_a, negate( p_a)); }
		static void stick			( long & p_a)							{}
		template <typename Ty> 
		static long	assign			( long & p_a, const Ty & p_b)			{ return p_a = static_cast<long>( p_b); }
		template <> 
		static long	assign<bool>	( long & p_a, const bool & p_b)			{ return p_a = (p_b ? 1 : 0); }
	};

	template <> class Value<unsigned long>
	{
	public:
		static unsigned long	zero			()														{ return 0; }
		static unsigned long	unit			()														{ return 1; }
		static bool				equals			( const unsigned long & p_a, const unsigned long & p_b)	{ return p_a == p_b; }
		static unsigned long	assign			( unsigned long & p_a, const unsigned long & p_b)		{ return p_a = p_b; }
		static unsigned long	add				( const unsigned long & p_a, const unsigned long & p_b)	{ return p_a + p_b; }
		static unsigned long	substract		( const unsigned long & p_a, const unsigned long & p_b)	{ return p_a - p_b; }
		static unsigned long	multiply		( const unsigned long & p_a, const unsigned long & p_b)	{ return p_a * p_b; }
		static unsigned long	divide			( const unsigned long & p_a, const unsigned long & p_b)	{ return p_a / p_b; }
		static unsigned long	negate			( const unsigned long & p_a)							{ return p_a; }
		static void				init			( unsigned long & p_a)									{ p_a = zero(); }
		static bool				is_null			( const unsigned long & p_a)							{ return equals( p_a, zero()); }
		static unsigned long	ass_add			( unsigned long & p_a, const unsigned long & p_b)		{ return assign( p_a, add( p_a, p_b)); }
		static unsigned long	ass_substract	( unsigned long & p_a, const unsigned long & p_b)		{ return assign( p_a, substract( p_a, p_b)); }
		static unsigned long	ass_multiply	( unsigned long & p_a, const unsigned long & p_b)		{ return assign( p_a, multiply( p_a, p_b)); }
		static unsigned long	ass_divide		( unsigned long & p_a, const unsigned long & p_b)		{ return assign( p_a, divide( p_a, p_b)); }
		static unsigned long	ass_negate		( unsigned long & p_a)									{ return assign( p_a, negate( p_a)); }
		static void				stick			( unsigned long & p_a)									{}
		template <typename Ty> 
		static unsigned long	assign			( unsigned long & p_a, const Ty & p_b)					{ return p_a = static_cast<unsigned long>( p_b); }
		template <> 
		static unsigned long	assign<bool>	( unsigned long & p_a, const bool & p_b)				{ return p_a = (p_b ? 1 : 0); }
	};

	template <> class Value<long long>
	{
	public:
		static long long	zero			()												{ return 0; }
		static long long	unit			()												{ return 1; }
		static bool			equals			( const long long & p_a, const long long & p_b)	{ return p_a == p_b; }
		static long long	assign			( long long & p_a, const long long & p_b)		{ return p_a = p_b; }
		static long long	add				( const long long & p_a, const long long & p_b)	{ return p_a + p_b; }
		static long long	substract		( const long long & p_a, const long long & p_b)	{ return p_a - p_b; }
		static long long	multiply		( const long long & p_a, const long long & p_b)	{ return p_a * p_b; }
		static long long	divide			( const long long & p_a, const long long & p_b)	{ return p_a / p_b; }
		static long long	negate			( const long long & p_a)						{ return -p_a; }
		static void			init			( long long & p_a)								{ p_a = zero(); }
		static bool			is_null			( const long long & p_a)						{ return equals( p_a, zero()); }
		static long long	ass_add			( long long & p_a, const long long & p_b)		{ return assign( p_a, add( p_a, p_b)); }
		static long long	ass_substract	( long long & p_a, const long long & p_b)		{ return assign( p_a, substract( p_a, p_b)); }
		static long long	ass_multiply	( long long & p_a, const long long & p_b)		{ return assign( p_a, multiply( p_a, p_b)); }
		static long long	ass_divide		( long long & p_a, const long long & p_b)		{ return assign( p_a, divide( p_a, p_b)); }
		static long long	ass_negate		( long long & p_a)								{ return assign( p_a, negate( p_a)); }
		static void			stick			( long long & p_a)								{}
		template <typename Ty> 
		static long long	assign			( long long & p_a, const Ty & p_b)				{ return p_a = static_cast<long long>( p_b); }
		template <> 
		static long long	assign<bool>	( long long & p_a, const bool & p_b)			{ return p_a = (p_b ? 1 : 0); }
	};

	template <> class Value<unsigned long long>
	{
	public:
		static unsigned long long	zero			()																	{ return 0; }
		static unsigned long long	unit			()																	{ return 1; }
		static bool					equals			( const unsigned long long & p_a, const unsigned long long & p_b)	{ return p_a == p_b; }
		static unsigned long long	assign			( unsigned long long & p_a, const unsigned long long & p_b)			{ return p_a = p_b; }
		static unsigned long long	add				( const unsigned long long & p_a, const unsigned long long & p_b)	{ return p_a + p_b; }
		static unsigned long long	substract		( const unsigned long long & p_a, const unsigned long long & p_b)	{ return p_a - p_b; }
		static unsigned long long	multiply		( const unsigned long long & p_a, const unsigned long long & p_b)	{ return p_a * p_b; }
		static unsigned long long	divide			( const unsigned long long & p_a, const unsigned long long & p_b)	{ return p_a / p_b; }
		static unsigned long long	negate			( const unsigned long long & p_a)									{ return p_a; }
		static void					init			( unsigned long long & p_a)											{ p_a = zero(); }
		static bool					is_null			( const unsigned long long & p_a)									{ return equals( p_a, zero()); }
		static unsigned long long	ass_add			( unsigned long long & p_a, const unsigned long long & p_b)			{ return assign( p_a, add( p_a, p_b)); }
		static unsigned long long	ass_substract	( unsigned long long & p_a, const unsigned long long & p_b)			{ return assign( p_a, substract( p_a, p_b)); }
		static unsigned long long	ass_multiply	( unsigned long long & p_a, const unsigned long long & p_b)			{ return assign( p_a, multiply( p_a, p_b)); }
		static unsigned long long	ass_divide		( unsigned long long & p_a, const unsigned long long & p_b)			{ return assign( p_a, divide( p_a, p_b)); }
		static unsigned long long	ass_negate		( unsigned long long & p_a)											{ return assign( p_a, negate( p_a)); }
		static void					stick			( unsigned long long & p_a)											{}
		template <typename Ty> 
		static unsigned long long	assign			( unsigned long long & p_a, const Ty & p_b)							{ return p_a = static_cast<unsigned long long>( p_b); }
		template <> 
		static unsigned long long	assign<bool>	( unsigned long long & p_a, const bool & p_b)						{ return p_a = (p_b ? 1 : 0); }
	};

	template <> class Value<float>
	{
	public:
		static float	zero			()										{ return float( 0); }
		static float	unit			()										{ return float( 1); }
		static bool		equals			( const float & p_a, const float & p_b)	{ return std::abs( p_a - p_b) < std::numeric_limits<float>::epsilon(); }
		static float	assign			( float & p_a, const float & p_b)		{ return p_a = p_b; }
		static float	add				( const float & p_a, const float & p_b)	{ return p_a + p_b; }
		static float	substract		( const float & p_a, const float & p_b)	{ return p_a - p_b; }
		static float	multiply		( const float & p_a, const float & p_b)	{ return p_a * p_b; }
		static float	divide			( const float & p_a, const float & p_b)	{ return p_a / p_b; }
		static float	negate			( const float & p_a)					{ return -p_a; }
		static void		init			( float & p_a)							{ p_a = zero(); }
		static bool		is_null			( const float & p_a)					{ return equals( p_a, zero()); }
		static float	ass_add			( float & p_a, const float & p_b)		{ return assign( p_a, add( p_a, p_b)); }
		static float	ass_substract	( float & p_a, const float & p_b)		{ return assign( p_a, substract( p_a, p_b)); }
		static float	ass_multiply	( float & p_a, const float & p_b)		{ return assign( p_a, multiply( p_a, p_b)); }
		static float	ass_divide		( float & p_a, const float & p_b)		{ return assign( p_a, divide( p_a, p_b)); }
		static float	ass_negate		( float & p_a)							{ return assign( p_a, negate( p_a)); }
		static void		stick			( float & p_a)
		{
			if (is_null( p_a))
			{
				init( p_a);
			}
			else if (p_a > 0)
			{
				if (equals( p_a, float( int( p_a + 0.5f))))
				{
					assign( p_a, float( int( p_a + 0.5f)));
				}
			}
			else
			{
				if (equals( p_a, float( int( p_a - 0.5f))))
				{
					assign( p_a, float( int( p_a - 0.5f)));
				}
			}
		}
		template <typename Ty> 
		static float	assign			( float & p_a, const Ty & p_b)			{ return p_a = static_cast<float>( p_b); }
		template <> 
		static float	assign<bool>	( float & p_a, const bool & p_b)		{ return p_a = (p_b ? 1.0f : 0.0f); }
	};

	template <> class Value<double>
	{
	public:
		static double	zero			()											{ return double( 0); }
		static double	unit			()											{ return double( 1); }
		static bool		equals			( const double & p_a, const double & p_b)	{ return std::abs( p_a - p_b) < std::numeric_limits<double>::epsilon(); }
		static double	assign			( double & p_a, const double & p_b)			{ return p_a = p_b; }
		static double	add				( const double & p_a, const double & p_b)	{ return p_a + p_b; }
		static double	substract		( const double & p_a, const double & p_b)	{ return p_a - p_b; }
		static double	multiply		( const double & p_a, const double & p_b)	{ return p_a * p_b; }
		static double	divide			( const double & p_a, const double & p_b)	{ return p_a / p_b; }
		static double	negate			( const double & p_a)						{ return -p_a; }
		static void		init			( double & p_a)								{ p_a = zero(); }
		static bool		is_null			( const double & p_a)						{ return equals( p_a, zero()); }
		static double	ass_add			( double & p_a, const double & p_b)			{ return assign( p_a, add( p_a, p_b)); }
		static double	ass_substract	( double & p_a, const double & p_b)			{ return assign( p_a, substract( p_a, p_b)); }
		static double	ass_multiply	( double & p_a, const double & p_b)			{ return assign( p_a, multiply( p_a, p_b)); }
		static double	ass_divide		( double & p_a, const double & p_b)			{ return assign( p_a, divide( p_a, p_b)); }
		static double	ass_negate		( double & p_a)								{ return assign( p_a, negate( p_a)); }
		static void		stick			( double & p_a)
		{
			if (is_null( p_a))
			{
				init( p_a);
			}
			else if (p_a > 0)
			{
				if (equals( p_a, double( int( p_a + 0.5f))))
				{
					assign( p_a, double( int( p_a + 0.5f)));
				}
			}
			else
			{
				if (equals( p_a, double( int( p_a - 0.5f))))
				{
					assign( p_a, double( int( p_a - 0.5f)));
				}
			}
		}
		template <typename Ty> 
		static double	assign			( double & p_a, const Ty & p_b)				{ return p_a = static_cast<double>( p_b); }
		template <> 
		static double	assign<bool>	( double & p_a, const bool & p_b)			{ return p_a = (p_b ? 1.0 : 0.0); }
	};
}
}

#endif