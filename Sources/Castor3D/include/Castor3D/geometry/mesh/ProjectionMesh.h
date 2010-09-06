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

#ifndef ___C3D_ProjectionMesh___
#define ___C3D_ProjectionMesh___

#include "Mesh.h"

namespace Castor3D
{
	//! Projection mesh representation
	/*!
	The projection mesh is the projection of an arc over a distance
	\author Sylvain DOREMUS
	\date 26/08/2010
	*/
	class ProjectionMesh : public Mesh
	{
	private:
		Arc * m_pPattern;
		float m_fDepth;
		bool m_bClosed;
		unsigned int m_uiNbFaces;

	public:
		/**
		 * Constructor
		 */
		ProjectionMesh();
		/**
		 * Specified Constructor
		 *@param p_fAlphaX
		 */
		ProjectionMesh( Arc * p_pPattern, float p_fDepth, bool p_bClosed,
						unsigned int p_uiNbFaces=1, const String & p_strName = C3DEmptyString);
		~ProjectionMesh();
		/**
		 * Generates the mesh points
		 */
		virtual void GeneratePoints();

	public:
		inline unsigned int	GetNbFaces	()const { return m_uiNbFaces; }
	};
}

#endif