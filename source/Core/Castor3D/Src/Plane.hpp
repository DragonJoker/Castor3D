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
#ifndef ___C3D_PLANE_H___
#define ___C3D_PLANE_H___

#include "MeshGenerator.hpp"

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\date		14/02/2010
	\~english
	\brief		The plane representation
	\remark		A plane can be subdivided in width and in height.
	\~french
	\brief		Représentation d'un plan
	\remark		Un plan peut être subdivisé en hauteur et en largeur
	*/
	class Plane
		: public MeshGenerator
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		C3D_API Plane();
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~Plane();
		/**
		 *\copydoc		Castor3D::MeshGenerator::Create
		 */
		C3D_API static MeshGeneratorSPtr Create();
		/**
		 *\copydoc		Castor3D::MeshGenerator::Generate
		 */
		C3D_API virtual void Generate( Mesh & p_mesh, UIntArray const & p_faces, RealArray const & p_dimensions );
		/**
		 *\~english
		 *\brief		Retrieves the plane height
		 *\~french
		 *\brief		Récupère la hauteur du plan
		 */
		inline real GetHeight()const
		{
			return m_depth;
		}
		/**
		 *\~english
		 *\brief		Retrieves the plane width
		 *\~french
		 *\brief		Récupère la largeur du plan
		 */
		inline real GetWidth()const
		{
			return m_width;
		}
		/**
		 *\~english
		 *\brief		Retrieves the plane width subdivisions
		 *\~french
		 *\brief		Récupère le nombre de subdivisions en largeur du plan
		 */
		inline uint32_t	GetSubDivisionsX()const
		{
			return m_subDivisionsW;
		}
		/**
		 *\~english
		 *\brief		Retrieves the plane height subdivisions
		 *\~french
		 *\brief		Récupère le nombre de subdivisions en hauteur du plan
		 */
		inline uint32_t	GetSubDivisionsY()const
		{
			return m_subDivisionsD;
		}

	private:
		real m_depth;
		real m_width;
		uint32_t m_subDivisionsW;
		uint32_t m_subDivisionsD;
		friend std::ostream & operator <<( std::ostream & o, Plane const & c );
	};
	/**
	 *\~english
	 *\brief		Stream operator
	 *\~french
	 *\brief		Operateur de flux
	 */
	inline std::ostream & operator <<( std::ostream & o, Plane const & c )
	{
		return o << "Plane(" << c.m_depth << "," << c.m_width << "," << c.m_subDivisionsW << "," << c.m_subDivisionsD << ")";
	}
}

#endif
