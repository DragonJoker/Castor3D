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
#ifndef ___C3D_PnTrianglesDivider___
#define ___C3D_PnTrianglesDivider___

#pragma warning( push )
#pragma warning( disable:4311 )
#pragma warning( disable:4312 )

#include <Submesh.hpp>
#include <Face.hpp>
#include <Vertex.hpp>

#include <Subdivider.hpp>
#include <FrameListener.hpp>

#pragma warning( pop )

#ifndef _WIN32
#	define C3D_PnTriangles_API
#else
#	ifdef PnTrianglesDivider_EXPORTS
#		define C3D_PnTriangles_API __declspec(dllexport)
#	else
#		define C3D_PnTriangles_API __declspec(dllimport)
#	endif
#endif

namespace PnTriangles
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/03/2010
	\~english
	\brief		Subdivider using PN Triangles subdivision algorithm
	*/
	class Subdivider : public Castor3D::Subdivider
	{
	public:
		Subdivider();
		virtual ~Subdivider();
		virtual void Cleanup();

	private:
		virtual void  DoSubdivide();
		Castor3D::BufferElementGroupSPtr DoComputePointFrom( Castor::Point3r const & p_ptPosA, Castor::Point3r const & p_ptPosB, Castor::Point3r const & p_ptNmlA, Castor::Point3r const & p_ptNmlB, Castor::Point3r const & p_ptCenter );
	};
}

#endif
