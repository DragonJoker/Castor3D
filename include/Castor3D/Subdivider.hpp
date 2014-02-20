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
#ifndef ___C3D_Subdivider___
#define ___C3D_Subdivider___

#include "Prerequisites.hpp"
#include "FrameListener.hpp"

#pragma warning( push )
#pragma warning( disable:4251 )
#pragma warning( disable:4275 )

namespace Castor3D
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.6.1.0
	\date		11/01/2011
	\~english
	\brief		Threaded Subdivision Frame event
	\remark		At the end of the subdivision, buffers of a submesh may be generated, it happens in this event
	\~french
	\brief		Threaded Subdivision Frame event
	\remark		A la fin d'une subdivision de submesh, les tampons peuvent être regénérés, cette action est effectuée dans cet évènement
	*/
	class C3D_API SubdivisionFrameEvent : public FrameEvent
	{
	private:
		SubmeshSPtr m_pSubmesh;

	public:
		SubdivisionFrameEvent( SubmeshSPtr p_pSubmesh);
		virtual ~SubdivisionFrameEvent();

		virtual bool Apply();
	};
	/*!
	\author		Sylvain DOREMUS
	\date		12/03/2010
	\~english
	\brief		Subdividers main class C3D_API
	\remark		Abstract class C3D_API for subdivisers, contains the header for the main Subdivide function
	\~french
	\brief		Classe de base (abstraite) pour les subdiviseurs
	\remark		Contient l'interface commune aux subdiviseurs
	*/
	class C3D_API Subdivider
	{
	protected:
		typedef void SubdivisionEndFunction( void *, Subdivider * );
		typedef SubdivisionEndFunction * PSubdivisionEndFunction;

		SubmeshSPtr						m_submesh;			//!< The submesh being subdivided
		FacePtrArray					m_arrayFaces;
		Castor::Point3r					m_ptDivisionCenter;
		std::shared_ptr< std::thread >	m_pThread;
		bool							m_bGenerateBuffers;
		PSubdivisionEndFunction			m_pfnSubdivisionEnd;
		void *							m_pArg;
		bool							m_bThreaded;
		std::recursive_mutex			m_mutex;

	public:
		/**
		 * Constructor
		 *\param[in]	p_submesh	The submesh to subdivide
		 */
		Subdivider();
		/**
		 * Destructor
		 */
		virtual ~Subdivider();
		/**
		 * Main subdivision function, must be implemented by children classes
		 *\param[in]	p_pCenter	The point used as center to compute new points
		 */
		void Subdivide(  SubmeshSPtr p_pSubmesh, Castor::Point3r * p_pCenter, bool p_bGenerateBuffers=true, bool p_bThreaded = false );
		/**
		 * Defines a function to execute when the threaded subdivision ends
		 * That function *MUST NEITHER* destroy the thread *NOR* the subdivider
		 *\param[in]	p_pfnSubdivisionEnd	Pointer over the function to execute
		 *\param[in]	p_pArg	Optional parameter for the function
		 */
		void SetThreadEndFunction( PSubdivisionEndFunction p_pfnSubdivisionEnd, void * p_pArg = nullptr) { m_pfnSubdivisionEnd = p_pfnSubdivisionEnd;m_pArg = p_pArg; }
		/**
		 * Cleans all member variables, making this divider dummy
		 */
		virtual void Cleanup();
		/**
		 * Creates and adds a vertex to my list
		 *\param[in]	x	The vertex X coordinate
		 *\param[in]	y	The vertex Y coordinate
		 *\param[in]	z	The vertex Z coordinate
		 *\return	The created vertex
		 */
		BufferElementGroupSPtr AddPoint( real x, real y, real z);
		/**
		 * Adds a vertex to my list
		 *\param[in]	p_v	The vertex to add
		 *\return	The vertex
		 */
		BufferElementGroupSPtr AddPoint( Castor::Point3r const & p_v);
		/**
		 * Creates and adds a vertex to my list
		 *\param[in]	p_v	The vertex coordinates
		 *\return	The created vertex
		 */
		BufferElementGroupSPtr AddPoint( real * p_v);
		/**
		 * Creates and adds a face to the wanted smoothgroup
		 *\param[in]	a	The first face's vertex index
		 *\param[in]	b	The second face's vertex index
		 *\param[in]	c	The third face's vertex index
		 *\param[in]	p_sgIndex	The wanted smoothing group index
		 *\return	The created face
		 */
		virtual FaceSPtr AddFace( uint32_t a, uint32_t b, uint32_t c );
		/**
		 * Tests if the given Point3r is in mine
		 *\param[in]	p_vertex	The vertex to test
		 *\return	The index of the vertex equal to parameter, -1 if not found
		 */
		virtual int IsInMyPoints( Castor::Point3r const & p_vertex);
		uint32_t GetNbPoints()const;
		BufferElementGroupSPtr GetPoint( uint32_t i )const;

	protected:
		static uint32_t DoSubdivideThreaded( Subdivider * p_pThis );
		/**
		 * Initialisation function
		 */
		virtual void DoInitialise();
		void DoSwapBuffers();
		virtual void DoSubdivide()=0;
		void DoSetTextCoords( FaceSPtr p_face, BufferElementGroup const & p_a, BufferElementGroup const & p_b, BufferElementGroup const & p_c, BufferElementGroup & p_d, BufferElementGroup & p_e, BufferElementGroup & p_f );
		void DoComputeCenterFrom( Castor::Point3r const & p_a, Castor::Point3r const & p_b, Castor::Point3r const & p_ptANormal, Castor::Point3r const & p_ptBNormal, Castor::Point3r & p_ptResult);
		void DoComputeCenterFrom( Castor::Point3r const & p_a, Castor::Point3r const & p_b, Castor::Point3r const & p_c, Castor::Point3r const & p_ptANormal, Castor::Point3r const & p_ptBNormal, Castor::Point3r const & p_ptCNormal, Castor::Point3r & p_ptResult);
	};
}

Castor::String & operator << ( Castor::String & p_stream, Castor3D::BufferElementGroup const & p_vertex);

#pragma warning( pop )

#endif
