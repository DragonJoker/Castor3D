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
#ifndef ___C3D_Vertex___
#define ___C3D_Vertex___

#include "Prerequisites.hpp"
#include "BufferElement.hpp"

namespace Castor
{	namespace Resources
{
	//! Point2f loader
	/*!
	Holds the two functions needed for a resource loader : load and save
	\author Sylvain DOREMUS
	\version 0.6.1.0
	\date 03/01/2011
	*/
	template <> class Loader<Point2f>
	{
	public:
		/**
		 * Reads a resource from a text file
		 *@param p_object : [in/out] The object to read
		 *@param p_file : [in/out] The file where to read the object
		 */
		static bool Read( Point2f & p_object, Utils::File & p_file);
		/**
		 * Writes a resource to a text file
		 *@param p_object : [in] The object to write
		 *@param p_file : [in/out] The file where to write the object
		 */
		static bool Write( const Point2f & p_object, Utils::File & p_file);
	};
	//! Point3f loader
	/*!
	Holds the two functions needed for a resource loader : load and save
	\author Sylvain DOREMUS
	\version 0.6.1.0
	\date 03/01/2011
	*/
	template <> class Loader<Point3f>
	{
	public:
		/**
		 * Reads a resource from a text file
		 *@param p_object : [in/out] The object to read
		 *@param p_file : [in/out] The file where to read the object
		 */
		static bool Read( Point3f & p_object, Utils::File & p_file);
		/**
		 * Writes a resource to a text file
		 *@param p_object : [in] The object to write
		 *@param p_file : [in/out] The file where to write the object
		 */
		static bool Write( const Point3f & p_object, Utils::File & p_file);
	};
	//! Point4f loader
	/*!
	Holds the two functions needed for a resource loader : load and save
	\author Sylvain DOREMUS
	\version 0.6.1.0
	\date 03/01/2011
	*/
	template <> class Loader<Point4f>
	{
	public:
		/**
		 * Reads a resource from a text file
		 *@param p_object : [in/out] The object to read
		 *@param p_file : [in/out] The file where to read the object
		 */
		static bool Read( Point4f & p_object, Utils::File & p_file);
		/**
		 * Writes a resource to a text file
		 *@param p_object : [in] The object to write
		 *@param p_file : [in/out] The file where to write the object
		 */
		static bool Write( const Point4f & p_object, Utils::File & p_file);
	};
	//! Point2d loader
	/*!
	Holds the two functions needed for a resource loader : load and save
	\author Sylvain DOREMUS
	\version 0.6.1.0
	\date 03/01/2011
	*/
	template <> class Loader<Point2d>
	{
	public:
		/**
		 * Reads a resource from a text file
		 *@param p_object : [in/out] The object to read
		 *@param p_file : [in/out] The file where to read the object
		 */
		static bool Read( Point2d & p_object, Utils::File & p_file);
		/**
		 * Writes a resource to a text file
		 *@param p_object : [in] The object to write
		 *@param p_file : [in/out] The file where to write the object
		 */
		static bool Write( const Point2d & p_object, Utils::File & p_file);
	};
	//! Point3d loader
	/*!
	Holds the two functions needed for a resource loader : load and save
	\author Sylvain DOREMUS
	\version 0.6.1.0
	\date 03/01/2011
	*/
	template <> class Loader<Point3d>
	{
	public:
		/**
		 * Reads a resource from a text file
		 *@param p_object : [in/out] The object to read
		 *@param p_file : [in/out] The file where to read the object
		 */
		static bool Read( Point3d & p_object, Utils::File & p_file);
		/**
		 * Writes a resource to a text file
		 *@param p_object : [in] The object to write
		 *@param p_file : [in/out] The file where to write the object
		 */
		static bool Write( const Point3d & p_object, Utils::File & p_file);
	};
	//! Point4d loader
	/*!
	Holds the two functions needed for a resource loader : load and save
	\author Sylvain DOREMUS
	\version 0.6.1.0
	\date 03/01/2011
	*/
	template <> class Loader<Point4d>
	{
	public:
		/**
		 * Reads a resource from a text file
		 *@param p_object : [in/out] The object to read
		 *@param p_file : [in/out] The file where to read the object
		 */
		static bool Read( Point4d & p_object, Utils::File & p_file);
		/**
		 * Writes a resource to a text file
		 *@param p_object : [in] The object to write
		 *@param p_file : [in/out] The file where to write the object
		 */
		static bool Write( const Point4d & p_object, Utils::File & p_file);
	};
	//! Vertex loader
	/*!
	Holds the two functions needed for a resource loader : load and save
	\author Sylvain DOREMUS
	\version 0.6.1.0
	\date 03/01/2011
	*/
	template <> class Loader<Castor3D::Vertex>
	{
	public:
		/**
		 * Reads a resource from a text file
		 *@param p_object : [in/out] The object to read
		 *@param p_file : [in/out] The file where to read the object
		 */
		static bool Read( Castor3D::Vertex & p_object, Utils::File & p_file);
		/**
		 * Writes a resource to a text file
		 *@param p_object : [in] The object to write
		 *@param p_file : [in/out] The file where to write the object
		 */
		static bool Write( const Castor3D::Vertex & p_object, Utils::File & p_file);
	};
}
}

namespace Castor3D
{
	//! A simple Point with an index
	template <typename T, size_t Count>
	class IdPoint : public Serialisable, public Point<T, Count>, public MemoryTraced< IdPoint<T, Count> >
	{
	public:
		typedef T									value_type;
		typedef Policy<value_type>					policy;

	private:
		typedef value_type &						reference;
		typedef value_type *						pointer;
		typedef value_type const &					const_reference;
		typedef value_type const *					const_pointer;
		typedef Point<value_type, Count>			point;
		typedef Point<value_type, Count> &			point_reference;
		typedef Point<value_type, Count> *			point_pointer;
		typedef const Point<value_type, Count> &	const_point_reference;
		typedef const Point<value_type, Count> *	const_point_pointer;

	private:
		size_t m_uiIndex;

	public:
		IdPoint( size_t p_uiIndex = 0);
		IdPoint( const IdPoint<T, Count> & p_ptPoint, bool p_bLinked = false);
		template <typename _Ty> IdPoint( const _Ty * p_pCoords, size_t p_uiIndex = 0, bool p_bLinked = false);
		IdPoint( const_point_reference p_ptPoint, size_t p_uiIndex = 0, bool p_bLinked = false);
		template <typename _Ty> IdPoint( const _Ty & p_a, size_t p_uiIndex = 0);
		template <typename _Ty> IdPoint( const _Ty & p_a, const _Ty & p_b, size_t p_uiIndex = 0);
		template <typename _Ty> IdPoint( const _Ty & p_a, const _Ty & p_b, const _Ty & p_c, size_t p_uiIndex = 0);
		template <typename _Ty> IdPoint( const _Ty & p_a, const _Ty & p_b, const _Ty & p_c, const _Ty & p_d, size_t p_uiIndex = 0);
		template <typename _Ty, size_t _Count> IdPoint( const Point<_Ty, _Count> & p_ptPoint, size_t p_uiIndex = 0, bool p_bLinked = false);
		virtual ~IdPoint();
	public:
		/**@name Operators */
		//@{
		inline IdPoint<T, Count> &	operator =( const IdPoint<T, Count> & p_toCopy);
		//@}

		/**@name Accessors */
		//@{
		inline size_t	GetIndex	()const { return m_uiIndex; }
		inline void	SetIndex	( const size_t & val)	{ m_uiIndex = val; }
		//@}

		DECLARE_SERIALISE_MAP()
	};
	
	//! Representation of a vertex
	/*!
	Specialisation of BufferElement, containing vertex's position, normal, tangent and texture coordinates, an index for smoothing groups
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	*/
	class C3D_API Vertex : public Serialisable, public BufferElement, public MemoryTraced<Vertex>
	{
	public:
		static size_t Size;					//!< Number of components of a Vertex (total of coordinates for coords, normal, tangent, texture coords).
		static unsigned long long Count;	//!< Total number of created vertex
		static BufferElementDeclaration VertexDeclarationElements[];
		static BufferDeclaration VertexDeclaration;

	private:
		size_t m_uiIndex;
		bool m_bManual;

	public:
		/**
		* Specified constructor
		*/
		Vertex( real x = 0, real y = 0, real z = 0, size_t p_uiIndex = 0);
		/**
		* Copy constructor
		*/
		explicit Vertex( const Vertex & p_vertex);
		/**
		* Constructor from point
		*/
		Vertex( Point3r const & p_point);
		/**
		* Constructor from point
		*/
		Vertex( const IdPoint3r & p_idPoint);
		/**
		* Constructor from a real array
		*/
		Vertex( real * p_pBuffer);
		/**
		* Constructor from the difference between second and first argument
		*/
		Vertex( const Vertex & p_v1, const Vertex & p_v2);
		/**
		* Destructor
		*/
		virtual ~Vertex();
		/**
		 * Links the vertex coords to the ones in parameter.
		 * The vertex no longer owns it's coords
		 *@param p_pBuffer : [in] The coordinates buffer
		 */
		void LinkCoords( real * p_pBuffer);
		/**
		 * Unlinks the vertex coords.
		 * The vertex now owns it's coords
		 */
		void UnlinkCoords();

		/**@name Assignment */
		//@{
		void SetNormal	( Point3r const & val, bool p_bManual = false);
		void SetNormal	( real x, real y, real z, bool p_bManual = false);
		void SetNormal	( real const * p_pCoords, bool p_bManual = false);

		void SetTexCoord	( const Point2r & val, bool p_bManual = false);
		void SetTexCoord	( real x, real y, bool p_bManual = false);
		void SetTexCoord	( real const * p_pCoords, bool p_bManual = false);

		void SetTangent	( Point3r const & val, bool p_bManual = false);
		void SetTangent	( real x, real y, real z, bool p_bManual = false);
		void SetTangent	( real const * p_pCoords, bool p_bManual = false);
		//@}

		/**@name Operators */
		//@{
		inline real &			operator []( size_t p_uiIndex)		{ return GetElement<Point3r>( 0)[p_uiIndex]; }
		inline const real &		operator []( size_t p_uiIndex)const	{ return GetElement<Point3r>( 0)[p_uiIndex]; }
		virtual Vertex & 		operator =( const Vertex & p_vertex);
		virtual Vertex & 		operator =( const IdPoint3r & p_idPoint);
		//@}

		/**@name Accessors */
		//@{
		inline Point3r &		GetCoords	()		{ return GetElement<Point3r>( 0); }
		inline Point3r &		GetNormal	()		{ return GetElement<Point3r>( 1); }
		inline Point3r &		GetTangent	()		{ return GetElement<Point3r>( 2); }
		inline Point2r &		GetTexCoord	()		{ return GetElement<Point2r>( 3); }
		inline Point3r const &	GetCoords	()const	{ return GetElement<Point3r>( 0); }
		inline Point3r const &	GetNormal	()const	{ return GetElement<Point3r>( 1); }
		inline Point3r const &	GetTangent	()const	{ return GetElement<Point3r>( 2); }
		inline const Point2r &	GetTexCoord	()const	{ return GetElement<Point2r>( 3); }
		inline real const *		const_ptr	()const	{ return (real const *)( BufferElement::const_ptr()); }
		inline real *			ptr			()		{ return (real *)( BufferElement::ptr()); }
		inline size_t			GetIndex	()const { return m_uiIndex; }
		inline bool				IsManual	()const	{ return m_bManual; }
		inline void SetCoords	( Point3r const & val)						{ GetElement<Point3r>( 0).clopy( val); }
		inline void SetCoords	( real x, real y, real z)					{ GetElement<Point3r>( 0)[0] = x;GetElement<Point3r>( 0)[1] = y;GetElement<Point3r>( 0)[2] = z; }
		inline void SetCoords	( real const * p_pCoords)					{ GetElement<Point3r>( 0)[0] = GetElement<Point3r>( 0)[0];GetElement<Point3r>( 0)[1] = p_pCoords[1];GetElement<Point3r>( 0)[2] = p_pCoords[2]; }
		inline void SetIndex	( const size_t & val)						{ m_uiIndex = val; }
		//@}

	private:
		void _link();
		void _computeTangent();
		DECLARE_SERIALISE_MAP()
	};
#	include "Vertex.inl"
}

#endif
