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
#ifndef ___C3D_PNTrianglesDivider___
#define ___C3D_PNTrianglesDivider___

#include "Subdiviser.h"

namespace Castor3D
{
	/*!
	PN Triangles algorithm specialisation of Subdiviser.
	\author Sylvain DOREMUS
	\date 12/03/2010
	*/
	class PNTrianglesSubdiviser : public Subdiviser
	{
	public:
		PNTrianglesSubdiviser( Submesh * p_submesh)
			:	Subdiviser( p_submesh)
		{}
		virtual ~PNTrianglesSubdiviser();

		virtual void Subdivide( Vector3f * p_center);

	private:
		Vector3f * _computePointFrom( const Vector3f & p_a, float * p_aNormal,
									  const Vector3f & p_b, float * p_bNormal,
									  Vector3f * p_center=NULL);
		Vector3f * _computePointFrom( const Vector3f & p_a, Vector3f * p_aNormal,
									  const Vector3f & p_b, Vector3f * p_bNormal,
									  Vector3f * p_center=NULL)
		{
			return _computePointFrom( p_a, p_aNormal->ptr(), p_b, p_bNormal->ptr(), p_center);
		}
	};
}

#endif