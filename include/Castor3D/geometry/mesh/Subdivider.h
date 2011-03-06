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
#ifndef ___C3D_Subdivider___
#define ___C3D_Subdivider___

#include "../../Prerequisites.h"
#include "../../main/FrameListener.h"

namespace Castor3D
{
	//! Threaded Subdivision Frame event
	/*!
	At the end of the subdivision, buffers of a submesh may be generated, it happens in this event
	\author Sylvain DOREMUS
	\version 0.6.1.0
	\date 11/01/2011
	*/
	class SubdivisionFrameEvent : public FrameEvent
	{
	private:
		Submesh * m_pSubmesh;

	public:
		SubdivisionFrameEvent( Submesh * p_pSubmesh);
		virtual ~SubdivisionFrameEvent();

		virtual bool Apply();
	};
	//! Subdividers main class
	/*!
	Abstract class for subdivisers, contains the header for the main Subdivide function
	\author Sylvain DOREMUS
	\date 12/03/2010
	*/
	class C3D_API Subdivider
	{
	protected:
		typedef void SubdivisionEndFunction( void *, Subdivider *);
		typedef SubdivisionEndFunction * PSubdivisionEndFunction;

		Submesh * m_submesh;				//!< The submesh being subdivided
		IdPoint3rPtrArray m_points;			//!< All the vertices
		SmoothGroupPtrArray m_smoothGroups;	//!< The submesh smoothing groups
		VertexBufferPtr m_vertex;			//!< Pointer over divided submesh vertex
		Point3r m_ptDivisionCenter;
		shared_ptr<MultiThreading::Thread> m_pThread;
		bool m_bGenerateBuffers;
		PSubdivisionEndFunction m_pfnSubdivisionEnd;
		void * m_pArg;
		bool m_bThreaded;
		Castor::MultiThreading::RecursiveMutex m_mutex;

	public:
		/**
		 * Constructor
		 *@param p_submesh : [in] The submesh to subdivide
		 */
		Subdivider( Submesh * p_submesh);
		/**
		 * Destructor
		 */
		virtual ~Subdivider();
		/**
		 * Initialisation function
		 */
		virtual void Initialise();
		/**
		 * Main subdivision function, must be implemented by children classes
		 *@param p_center : [in] The point used as center to compute new points
		 */
		void Subdivide( const Point3r & p_center, bool p_bGenerateBuffers=true, bool p_bThreaded = false);
		/**
		 * Defines a function to execute when the threaded subdivision ends
		 * That function *MUST NEITHER* destroy the thread *NOR* the subdivider
		 *@param p_pfnSubdivisionEnd : [in] Pointer over the function to execute
		 *@param p_pArg : [in] Optional parameter for the function
		 */
		void SetThreadEndFunction( PSubdivisionEndFunction p_pfnSubdivisionEnd, void * p_pArg = NULL) { m_pfnSubdivisionEnd = p_pfnSubdivisionEnd;m_pArg = p_pArg; }
		/**
		 * Starts the threaded subdivision
		 */
		void StartThread();
		/**
		 * Cleans all member variables, making this divider dummy
		 */
		virtual void Cleanup();
		/**
		 * Creates and adds a vertex to my list
		 *@param x : [in] The vertex X coordinate
		 *@param y : [in] The vertex Y coordinate
		 *@param z : [in] The vertex Z coordinate
		 *@return The created vertex
		 */
		IdPoint3rPtr AddPoint( real x, real y, real z);
		/**
		 * Adds a vertex to my list
		 *@param p_v : [in] The vertex to add
		 *@return The vertex
		 */
		IdPoint3rPtr AddPoint( const Point3r & p_v);
		/**
		 * Creates and adds a vertex to my list
		 *@param p_v : [in] The vertex coordinates
		 *@return The created vertex
		 */
		IdPoint3rPtr AddPoint( real * p_v);
		/**
		 * Creates and adds a face to the wanted smoothgroup
		 *@param a : [in] The first face's vertex index
		 *@param b : [in] The second face's vertex index
		 *@param c : [in] The third face's vertex index
		 *@param p_sgIndex : [in] The wanted smoothing group index
		 *@return The created face
		 */
		virtual FacePtr AddFace( size_t a, size_t b, size_t c, size_t p_sgIndex);
		/**
		 * Tests if the given Point3r is in mine
		 *@param p_vertex : [in] The vertex to test
		 *@return The index of the vertex equal to parameter, -1 if not found
		 */
		virtual int IsInMyPoints( const Point3r & p_vertex);
		/**@name Accessors */
		//@{
		inline size_t				GetNbPoints	()const				{ return m_points.size(); }
		inline IdPoint3rPtr			GetPoint	( size_t i)const	{ CASTOR_ASSERT( i < m_points.size());return m_points[i]; }
		//@}

	protected:
		static unsigned int _subdivideThreaded( void * p_pThis);

		void _switchBuffers();
		virtual void _subdivide()=0;
		void _setTextCoords( FacePtr p_face, IdPoint3rPtr p_a, IdPoint3rPtr p_b, IdPoint3rPtr p_c, 
							 IdPoint3rPtr p_d, IdPoint3rPtr p_e, IdPoint3rPtr p_f, size_t p_sgIndex);
		void _setTextCoords( FacePtr p_face, const Vertex & p_a, const Vertex & p_b, const Vertex & p_c, 
							 const Vertex & p_d, const Vertex & p_e, const Vertex & p_f, size_t p_sgIndex);
		Point3r _computeCenterFrom( const Point3r & p_a, const Point3r & p_b, const Point3r & p_ptANormal, const Point3r & p_ptBNormal);
		Point3r _computeCenterFrom( const Point3r & p_a, const Point3r & p_b, const Point3r & p_c, const Point3r & p_ptANormal, const Point3r & p_ptBNormal, const Point3r & p_ptCNormal);
	};

	C3D_API String & operator << ( String & p_stream, const IdPoint3r & p_vertex);
	C3D_API String & operator << ( String & p_stream, const Vertex & p_vertex);
}

#endif