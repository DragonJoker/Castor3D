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
#ifndef ___C3D_PNTrianglesDivider___
#define ___C3D_PNTrianglesDivider___

#include "Subdivider.h"
#include "../basic/Vertex.h"

namespace Castor3D
{
	//! PN Triangles algorithm specialisation of Subdivider.
	/*!
	This class implements the PN Triangles subdivision algorithm
	\author Sylvain DOREMUS
	\date 12/03/2010
	*/
	class C3D_API PNTrianglesDivider : public Subdivider, public MemoryTraced<PNTrianglesDivider>
	{
	public:
		PNTrianglesDivider( Submesh * p_submesh);
		virtual ~PNTrianglesDivider();

		virtual void Cleanup();

	private:
		virtual void _subdivide();
		void _computePointFrom( Vertex & p_result, const Vertex & p_a, const Vertex & p_b, const Point3r * p_center=NULL);
	};
}

#endif