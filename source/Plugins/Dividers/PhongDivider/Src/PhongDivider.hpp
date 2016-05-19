/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)

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
#ifndef ___C3D_PHONG_DIVIDER___
#define ___C3D_PHONG_DIVIDER___

#pragma warning( push )
#pragma warning( disable:4311 )
#pragma warning( disable:4312 )

#include <Event/Frame/FrameListener.hpp>
#include <Mesh/Submesh.hpp>
#include <Mesh/Face.hpp>
#include <Mesh/Vertex.hpp>
#include <Mesh/Subdivider.hpp>

#include <PlaneEquation.hpp>

#pragma warning( pop )

#ifndef _WIN32
#	define C3D_Phong_API
#else
#	ifdef PhongDivider_EXPORTS
#		define C3D_Phong_API __declspec(dllexport)
#	else
#		define C3D_Phong_API __declspec(dllimport)
#	endif
#endif

namespace Phong
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/03/2010
	\~english
	\brief		The PN Triangle bezier patch
	*/
	struct Patch
	{
		Patch( Castor::PlaneEquation< double > const & p_p1, Castor::PlaneEquation< double > const & p_p2, Castor::PlaneEquation< double > const & p_p3 );

		Castor::PlaneEquation< double > const & pi;
		Castor::PlaneEquation< double > const & pj;
		Castor::PlaneEquation< double > const & pk;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/03/2010
	\~english
	\brief		Subdivider using PN Triangles subdivision algorithm
	*/
	class Subdivider
		: public Castor3D::Subdivider
	{
	public:
		Subdivider();
		virtual ~Subdivider();
		virtual void Cleanup();
		virtual void Subdivide( Castor3D::SubmeshSPtr p_pSubmesh, int p_occurences, bool p_generateBuffers = true, bool p_threaded = false );

	private:
		virtual void DoSubdivide();
		void DoComputeFaces( double u0, double v0, double u2, double v2, int p_occurences, Patch const & p_patch );
		Castor3D::BufferElementGroupSPtr DoComputePoint( double u, double v, Patch const & p_patch );

	private:
		int m_occurences;
	};
}

#endif
