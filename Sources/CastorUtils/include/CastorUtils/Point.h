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
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___C3D_Point___
#define ___C3D_Point___

#include "Module_Utils.h"

namespace General
{	namespace Math
{
	/*!
	The representation of a 2D Point, with X and Y which can be of a wanted type (like int, float...)
	\author Sylvain DOREMUS
	\date 14/02/2010
	*/
	template <typename T>
	class Point2D
	{
	public:
		union
		{
			float pPtr[2];
			struct
			{
				T x;
				T y;
			};
			struct
			{
				T u;
				T v;
			};
		};

	public:
		/**
		* Specified T constructor
		*/
		Point2D( T p_x=T( 0), T p_y=T( 0))
			:	x( p_x),
				y( p_y)
		{
		}
		inline void operator +=( const Point2D & p)
		{
			x += p.x;
			y += p.y;
		}
		inline void operator -=( const Point2D & p)
		{
			x -= p.x;
			y -= p.y;
		}
		inline void operator *=( const Point2D & p)
		{
			x *= p.x;
			y *= p.y;
		}
		inline void operator *=( T p_value)
		{
			x *= p_value;
			y *= p_value;
		}
		inline void operator /=( T p_value)
		{
			x /= p_value;
			y /= p_value;
		}
		inline Point2D operator	=( const Point2D & p)
		{
			x=p.x;
			y=p.y;
			return *this;
		}
		inline Point2D operator	+( const Point2D & p)const
		{
			return Point2D<T>( x + p.x, y + p.y);
		}
		inline Point2D operator	-( const Point2D & p)const
		{
			return Point2D<T>( x - p.x, y - p.y);
		}
		inline Point2D operator	/( T p_value)const
		{
			return Point2D<T>( x / p_value, y / p_value);
		}
		inline bool operator ==( const Point2D<T> & p_ptPoint)const
		{
			return (abs( x - p_ptPoint.x) < T( 0.0001f)) && (abs( y - p_ptPoint.y) < T( 0.0001f));
		}
		inline bool operator !=( const Point2D<T> & p_ptPoint)const
		{
			return (abs( x - p_ptPoint.x) >= T( 0.0001f)) || (abs( y - p_ptPoint.y) >= T( 0.0001f)) ;
		}
		inline float & operator[]( size_t p_uiIndex) { return ptr()[p_uiIndex]; }
		inline T * ptr() { return &x; }
		inline const T * const_ptr()const { return &x; }
	};
	/*!
	The representation of a 3D Point, with X, Y and Z which can be of a wanted type (like int, float...)
	\author Sylvain DOREMUS
	\date 14/02/2010
	*/
	template <typename T>
	class Point3D
	{
	public:
		union
		{
			float pPtr[3];
			struct
			{
				T x;
				T y;
				T z;
			};
			struct
			{
				T u;
				T v;
				T w;
			};
			struct
			{
				T r;
				T g;
				T b;
			};
		};

	public:
		/**
		* Specified T constructor
		*/
		Point3D( T p_x=T( 0), T p_y=T( 0), T p_z=T( 0))
			:	x( p_x),
				y( p_y),
				z( p_z)
		{
		}
		Point3D( const int * p_coord)
			:	x( (T)p_coord[0]),
				y( (T)p_coord[1]),
				z( (T)p_coord[2])
		{
		}
		Point3D( const float * p_coord)
			:	x( (T)p_coord[0]),
				y( (T)p_coord[1]),
				z( (T)p_coord[2])
		{
		}
		Point3D( const Point3D & p_point)
			:	x( p_point.x),
				y( p_point.y),
				z( p_point.z)
		{
		}
		/**
		* Constructor from the difference between second and first argument
		*/
		Point3D( const Point3D & p_v1, const Point3D & p_v2)
			:	x( p_v2.x - p_v1.x),
				y( p_v2.y - p_v1.y),
				z( p_v2.z - p_v1.z)
		{
		}

		inline void operator +=( const Point3D & p_vertex)
		{
			x += p_vertex.x;
			y += p_vertex.y;
			z += p_vertex.z;
		}
		inline void operator +=( T * p_coord)
		{
			x += p_coord[0];
			y += p_coord[1];
			z += p_coord[2];
		}
		inline void operator -=( const Point3D & p_vertex)
		{
			x -= p_vertex.x;
			y -= p_vertex.y;
			z -= p_vertex.z;
		}
		inline void operator *=( const Point3D & p_vertex)
		{
			x *= p_vertex.x;
			y *= p_vertex.y;
			z *= p_vertex.z;
		}
		inline void operator *=( float p_value)
		{
			x *= p_value;
			y *= p_value;
			z *= p_value;
		}
		inline void operator /=( int p_value)
		{
			if (p_value == 0.0)
			{
				return;
			}

			x /= p_value;
			y /= p_value;
			z /= p_value;
		}
		inline void operator /=( float p_value)
		{
			if (p_value == 0)
			{
				return;
			}

			x /= p_value;
			y /= p_value;
			z /= p_value;
		}
		inline Point3D operator =( const Point3D & p_vertex)
		{
			x = p_vertex.x;
			y = p_vertex.y;
			z = p_vertex.z;
			return * this;
		}
		inline Point3D operator /( int p_value)const
		{
			if (p_value == 0)
			{
				return * this;
			}
			return Point3D( x / p_value, y / p_value, z / p_value);	
		}
		inline Point3D operator /( float p_value)const
		{
			if (p_value == 0.0)
			{
				return * this;
			}
			return Point3D( x / p_value, y / p_value, z / p_value);	
		}
		inline float & operator[]( size_t p_uiIndex) { return ptr()[p_uiIndex]; }
		/**
		* Vectorial product
		*/
		inline Point3D operator ^( const Point3D & p_vertex)const
		{
			Point3D l_result;
			float l_x = p_vertex.x;
			float l_y = p_vertex.y;
			float l_z = p_vertex.z;
			l_result.x = (y * l_z) - (l_y * z);
			l_result.y = (z * l_x) - (l_z * x);
			l_result.z = (x * l_y) - (l_x * y);
			return l_result;
		}
		inline void Reverse()
		{
			x = -x;
			y = -y;
			z = -z;
		}
		inline void Normalise()
		{
			float l_length = GetLength();	
			if (l_length == 0.0f)
			{
				return;
			}
			x /= l_length;
			y /= l_length;
			z /= l_length;
		}
		inline Point3D GetNormalised()const
		{
			float l_length = GetLength();	
			if (l_length == 0.0f)
			{
				return *this;
			}
			return Point3D( x / l_length, y / l_length, z / l_length);
		}
		inline Point3D GetNormal( Point3D & p_vertex)const
		{
			Point3D l_normal = this->operator ^( p_vertex);
			l_normal.Normalise();
			return l_normal;
		}
		inline Point3D operator +( const Point3D & p_vertex)const
		{
			return Point3D( x + p_vertex.x, y + p_vertex.y, z + p_vertex.z);
		}
		inline Point3D operator -( const Point3D & p_vertex)const
		{
			return Point3D( x - p_vertex.x, y - p_vertex.y, z - p_vertex.z);
		}
		inline Point3D operator *( const Point3D & p_vertex)const
		{
			return Point3D( x * p_vertex.x, y * p_vertex.y, z * p_vertex.z);
		}
		inline Point3D operator *( int p_value)const
		{
			return Point3D( x * p_value, y * p_value, z * p_value);
		}
		inline Point3D operator *( float p_value)const
		{
			return Point3D( x * p_value, y * p_value, z * p_value);
		}
		inline bool operator !=( const Point3D & p_vertex)const
		{
			return (x != p_vertex.x) || (y != p_vertex.y) || (z != p_vertex.z);
		}
		inline T dotProduct( const Point3D & p_vertex)const
		{
			return x * p_vertex.x + y * p_vertex.y + z * p_vertex.z;
		}
		inline T GetSquaredLength()const
		{
			return x * x + y * y + z * z;
		}
		inline float GetLength()const
		{
			return sqrt( (float)GetSquaredLength());
		}
		inline float GetCosTheta( Point3D & p_vector)const
		{
			return dotProduct( p_vector) / (GetLength() * p_vector.GetLength());
		}
		inline bool operator ==( const Point3D<T> & p_ptPoint)
		{
			return (abs( x - p_ptPoint.x) < T( 0.0001f)) && (abs( y - p_ptPoint.y) < T( 0.0001f)) && (abs( z - p_ptPoint.z) < T( 0.0001f));
		}
		inline bool operator !=( const Point3D<T> & p_ptPoint)
		{
			return (abs( x - p_ptPoint.x) >= T( 0.0001f)) || (abs( y - p_ptPoint.y) >= T( 0.0001f)) || (abs( z - p_ptPoint.z) >= T( 0.0001f));
		}
		inline T * ptr() { return &x; }
		inline const T * const_ptr()const { return &x; }
		inline operator Point2D<T>() { return Point2D<T>( x, y); }
	};
	/*!
	The representation of a 4D Point, with X, Y, Z and W which can be of a wanted type (like int, float...)
	\author Sylvain DOREMUS
	\date 14/02/2010
	*/
	template <typename T>
	class Point4D
	{
	public:
		union
		{
			float pPtr[4];
			struct
			{
				T x;
				T y;
				T z;
				T w;
			};
			struct
			{
				T r;
				T g;
				T b;
				T a;
			};
			struct
			{
				T left;
				T top;
				T right;
				T bottom;
			};
		};

	public:
		/**
		* Specified T constructor
		*/
		Point4D( T p_x=T( 0), T p_y=T( 0), T p_z=T( 0), T p_w=T( 0))
			:	x( p_x),
				y( p_y),
				z( p_z),
				w( p_w)
		{
		}
		Point4D( const int * p_coord)
			:	x( (T)p_coord[0]),
				y( (T)p_coord[1]),
				z( (T)p_coord[2]),
				w( (T)p_coord[3])
		{
		}
		Point4D( const float * p_coord)
			:	x( (T)p_coord[0]),
				y( (T)p_coord[1]),
				z( (T)p_coord[2]),
				w( (T)p_coord[3])
		{
		}
		Point4D( const Point4D & p_point)
			:	x( p_point.x),
				y( p_point.y),
				z( p_point.z),
				w( p_point.w)
		{
		}
		/**
		* Constructor from the difference between second and first argument
		*/
		Point4D( const Point4D & p_v1, const Point4D & p_v2)
			:	x( p_v2.x - p_v1.x),
				y( p_v2.y - p_v1.y),
				z( p_v2.z - p_v1.z),
				w( p_v2.w - p_v1.w)
		{
		}

		inline void operator +=( const Point3D<T> & p_vertex)
		{
			x += p_vertex.x;
			y += p_vertex.y;
			z += p_vertex.z;
		}

		inline void operator +=( const Point4D & p_vertex)
		{
			x += p_vertex.x;
			y += p_vertex.y;
			z += p_vertex.z;
			w += p_vertex.w;
		}
		inline void operator +=( T * p_coord)
		{
			x += p_coord[0];
			y += p_coord[1];
			z += p_coord[2];
			w += p_coord[3];
		}
		inline void operator -=( const Point4D & p_vertex)
		{
			x -= p_vertex.x;
			y -= p_vertex.y;
			z -= p_vertex.z;
			w -= p_vertex.w;
		}
		inline void operator *=( const Point4D & p_vertex)
		{
			x *= p_vertex.x;
			y *= p_vertex.y;
			z *= p_vertex.z;
			w *= p_vertex.w;
		}
		inline void operator *=( float p_value)
		{
			x *= p_value;
			y *= p_value;
			z *= p_value;
			w *= p_value;
		}
		inline void operator /=( int p_value)
		{
			if (p_value == 0.0)
			{
				return;
			}

			x /= p_value;
			y /= p_value;
			z /= p_value;
			w /= p_value;
		}
		inline void operator /=( float p_value)
		{
			if (p_value == 0)
			{
				return;
			}

			x /= p_value;
			y /= p_value;
			z /= p_value;
			w /= p_value;
		}
		inline Point4D operator =( const Point4D & p_vertex)
		{
			x = p_vertex.x;
			y = p_vertex.y;
			z = p_vertex.z;
			w = p_vertex.w;
			return * this;
		}
		inline Point4D operator /( int p_value)const
		{
			if (p_value == 0)
			{
				return * this;
			}

			return Point4D( x / p_value, y / p_value, z / p_value, w / p_value);	
		}
		inline Point4D operator /( float p_value)const
		{
			if (p_value == 0.0)
			{
				return * this;
			}

			return Point4D( x / p_value, y / p_value, z / p_value, w / p_value);	
		}
		inline float & operator[]( size_t p_uiIndex) { return ptr()[p_uiIndex]; }
		inline void Reverse()
		{
			x = -x;
			y = -y;
			z = -z;
			w = -w;
		}
		inline Point4D operator +( const Point4D & p_vertex)const
		{
			return Point4D( x + p_vertex.x, y + p_vertex.y, z + p_vertex.z, w + p_vertex.w);
		}
		inline Point4D operator -( const Point4D & p_vertex)const
		{
			return Point4D( x - p_vertex.x, y - p_vertex.y, z - p_vertex.z, w - p_vertex.w);
		}
		inline Point4D operator *( const Point4D & p_vertex)const
		{
			return Point4D( x * p_vertex.x, y * p_vertex.y, z * p_vertex.z, w * p_vertex.w);
		}
		inline Point4D operator *( int p_value)const
		{
			return Point4D( x * p_value, y * p_value, z * p_value, w * p_value);
		}
		inline Point4D operator *( float p_value)const
		{
			return Point4D( x * p_value, y * p_value, z * p_value, w * p_value);
		}
		inline bool operator !=( const Point4D & p_vertex)const
		{
			return (x != p_vertex.x) || (y != p_vertex.y) || (z != p_vertex.z) || (w != p_vertex.w);
		}
		inline bool operator ==( const Point4D<T> & p_ptPoint)
		{
			return (abs( x - p_ptPoint.x) < T( 0.0001f)) && (abs( y - p_ptPoint.y) < T( 0.0001f)) && (abs( z - p_ptPoint.z) < T( 0.0001f)) && (abs( w - p_ptPoint.w) < T( 0.0001f));
		}
		inline bool operator !=( const Point4D<T> & p_ptPoint)
		{
			return (abs( x - p_ptPoint.x) >= T( 0.0001f)) || (abs( y - p_ptPoint.y) >= T( 0.0001f)) || (abs( z - p_ptPoint.z) < T( 0.0001f)) || (abs( w - p_ptPoint.w) >= T( 0.0001f));
		}
		inline T * ptr() { return &x; }
		inline const T * const_ptr()const { return &x; }
		inline operator Point3D<T>() { return Point3D<T>( x, y, z); }
	};

	class Point2Bool : public Point2D<bool>
	{
	public:
		Point2Bool( bool p_bX=false, bool p_bY=false)
			:	Point2D<bool>( p_bX, p_bY)
		{
		}
		inline bool operator ==( const Point2Bool & p_ptPoint)
		{
			return (x == p_ptPoint.x) && (y == p_ptPoint.y);
		}
		inline bool operator !=( const Point2Bool & p_ptPoint)
		{
			return (x != p_ptPoint.x) || (y != p_ptPoint.y);
		}
	};

	class Point3Bool : public Point3D<bool>
	{
	public:
		Point3Bool( bool p_bX=false, bool p_bY=false, bool p_bZ=false)
			:	Point3D<bool>( p_bX, p_bY, p_bZ)
		{
		}
		inline bool operator ==( const Point3Bool & p_ptPoint)
		{
			return (x == p_ptPoint.x) && (y == p_ptPoint.y) && (z == p_ptPoint.z);
		}
		inline bool operator !=( const Point3Bool & p_ptPoint)
		{
			return (x != p_ptPoint.x) || (y != p_ptPoint.y) || (z != p_ptPoint.z);
		}
	};

	class Point4Bool : public Point4D<bool>
	{
	public:
		Point4Bool( bool p_bX=false, bool p_bY=false, bool p_bZ=false, bool p_bW=false)
			:	Point4D<bool>( p_bX, p_bY, p_bZ, p_bW)
		{
		}
		inline bool operator ==( const Point4Bool & p_ptPoint)
		{
			return (x == p_ptPoint.x) && (y == p_ptPoint.y) && (z == p_ptPoint.z) && (w == p_ptPoint.w);
		}
		inline bool operator !=( const Point4Bool & p_ptPoint)
		{
			return (x != p_ptPoint.x) || (y != p_ptPoint.y) || (z != p_ptPoint.z) || (w != p_ptPoint.w);
		}
	};
}
}

#endif