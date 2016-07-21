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

#ifndef ___C3D_PROJECTION_H___
#define ___C3D_PROJECTION_H___

#include "Mesh/MeshGenerator.hpp"

#include <Math/Point.hpp>

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\date		26/08/2010
	\~english
	\brief		Projection mesh representation
	\remark		The projection mesh is the projection of an arc over along axis on a given distance
	\~french
	\brief		Représentation d'une projection
	\remark		Ce type de mesh est la projection d'un arc selon un axe sur une distance donnée
	*/
	class Projection
		: public MeshGenerator
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		C3D_API Projection();
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~Projection();
		/**
		 *\copydoc		Castor3D::MeshGenerator::Create
		 */
		C3D_API static MeshGeneratorSPtr Create();
		/**
		 *\~english
		 *\brief		Defines the pattern used to build the projection
		 *\param[in]	p_pPattern	The arc to project
		 *\param[in]	p_vAxis		The projection axis
		 *\param[in]	p_bClosed	Tells if the projection must be closed
		 *\~french
		 *\brief		Définit l'arc utilisé pour construire la projection
		 *\param[in]	p_pPattern	L'arc à projeter
		 *\param[in]	p_vAxis		L'axe de projection
		 *\param[in]	p_bClosed	Dit si on doit fermer la projection
		 */
		C3D_API void SetPoints( Point3rPatternSPtr p_pPattern, Castor::Point3r const & p_vAxis, bool p_bClosed );
		/**
		 *\~english
		 *\brief		Retrieves number of faces
		 *\~french
		 *\brief		Récupère le nombre de faces
		 */
		inline uint32_t	GetFaceCount()const
		{
			return m_uiNbFaces;
		}

	private:
		/**
		*\copydoc		Castor3D::MeshGenerator::DoGenerate
		*/
		C3D_API virtual void DoGenerate( Mesh & p_mesh, UIntArray const & p_faces, RealArray const & p_dimensions );

	private:
		Point3rPatternSPtr m_pPattern;
		Castor::Point3r m_vAxis;
		real m_fDepth;
		bool m_bClosed;
		uint32_t m_uiNbFaces;
		friend std::ostream & operator <<( std::ostream & o, Projection const & c );
	};
	/**
	 *\~english
	 *\brief		Stream operator
	 *\~french
	 *\brief		Operateur de flux
	 */
	inline std::ostream & operator <<( std::ostream & o, Projection const & c )
	{
		return o << "Projection( (" << c.m_vAxis[0] << "," << c.m_vAxis[1] << "," << c.m_vAxis[2] << ")," << c.m_fDepth << "," << c.m_bClosed << "," << c.m_uiNbFaces << ")";
	}
}

#endif
