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
#ifndef ___C3D_ICOSAHEDRON_H___
#define ___C3D_ICOSAHEDRON_H___

#include "MeshGenerator.hpp"

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\date		14/02/2010
	\~english
	\brief		The icosahedron representation
	\remark		An icosahedron is a geometric polygon constituted by 20 equilateral triangles.
				<br />This mesh is used to build a sphere with triangular faces.
	\~french
	\brief		Représentation d'un icosaèdre
	\remark		Un icosaèdre est un polygône constitué de 20 triangles équilatéraux
				<br />Ce mesh est utilisé pour construire des sphères à faces triangulaires
	*/
	class C3D_API Icosahedron
		:	public MeshGenerator
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		Icosahedron();
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		~Icosahedron();
		/**
		 *\copydoc		Castor3D::MeshGenerator::Create
		 */
		static MeshGeneratorSPtr Create();
		/**
		 *\copydoc		Castor3D::MeshGenerator::Generate
		 */
		virtual void Generate( Mesh & p_mesh, UIntArray const & p_faces, RealArray const & p_dimensions );
		/**
		 *\~english
		 *\brief		Retrieves number of faces
		 *\~french
		 *\brief		Récupère le nombre de faces
		 */
		inline uint32_t GetFaceCount()const
		{
			return m_nbFaces;
		}
		/**
		 *\~english
		 *\brief		Retrieves icosahedron radius
		 *\~french
		 *\brief		Récupère le rayon de l'icosaèdre
		 */
		inline real GetRadius()const
		{
			return m_radius;
		}

	private:
		real m_radius;
		uint32_t m_nbFaces;
		friend std::ostream & operator <<( std::ostream & o, Icosahedron const & c );
	};
	/**
	 *\~english
	 *\brief		Stream operator
	 *\~french
	 *\brief		Operateur de flux
	 */
	inline std::ostream & operator <<( std::ostream & o, Icosahedron const & c )
	{
		return o << "Icosahedron(" << c.m_nbFaces << "," << c.m_radius << ")";
	}
}

#endif
