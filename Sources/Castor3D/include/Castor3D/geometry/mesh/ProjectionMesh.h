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
		Point3rPatternPtr m_pPattern;
		Point3r m_vAxis;
		real m_fDepth;
		bool m_bClosed;
		unsigned int m_uiNbFaces;

	public:
		/**
		 * Constructor
		 */
		ProjectionMesh();
		/**
		 * Specified Constructor
		 *@param p_pPattern : [in] The arc to project
		 *@param p_vAxis : [in] The axis along which th projection is done
		 *@param p_bClosed : [in] Boolean telling if the projection must be closed (id est the pattern is closed)
		 *@param p_fDepth : [in] The projection depth
		 *@param p_uiNbFaces : [in] Number of subdivisions along the axis
		 *@param p_strName : [in] The mesh name
		 */
		ProjectionMesh( Point3rPatternPtr p_pPattern, const Point3r & p_vAxis, bool p_bClosed, real p_fDepth,
						unsigned int p_uiNbFaces=1, const String & p_strName = C3DEmptyString);
		/**
		 * Destructor
		 */
		~ProjectionMesh();
		/**
		 * Generates the mesh points
		 */
		virtual void GeneratePoints();

	public:
		/**@name Accessors */
		//@{
		inline unsigned int	GetNbFaces	()const { return m_uiNbFaces; }
		//@}
	};
}

#endif