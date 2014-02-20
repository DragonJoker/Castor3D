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
#ifndef ___C3D_Icosahedron___
#define ___C3D_Icosahedron___

#include "Mesh.hpp"

#pragma warning( push )
#pragma warning( disable:4251 )
#pragma warning( disable:4275 )

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
	class C3D_API Icosahedron : public MeshCategory
	{
	private:
		real		m_radius;
		uint32_t	m_nbFaces;

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
		 *\~english
		 *\brief		Create function, used by Factory
		 *\return		An icosahedron
		 *\~french
		 *\brief		Fonction de création utilisée par Factory
		 *\return		Un icosaèdre
		 */
		static MeshCategorySPtr Create();
		/**
		 *\~english
		 *\brief		Generates the mesh points and faces
		 *\~french
		 *\brief		Génère les points et faces du mesh
		 */
		virtual void Generate();
		/**
		 *\~english
		 *\brief		Modifies the mesh caracteristics then rebuild it
		 *\param[in]	p_arrayFaces		The new wanted mesh faces number
		 *\param[in]	p_arrayDimensions	The new wanted mesh dimensions
		 *\~french
		 *\brief		Modifie les caractéristiques du mesh et le reconstruit
		 *\param[in]	p_arrayFaces		Tableau contenant les nombres de faces
		 *\param[in]	p_arrayDimensions	Tableau contenant les dimensions du mesh
		 */
		virtual void Initialise( UIntArray const & p_arrayFaces, RealArray const & p_arrayDimensions);
		/**
		 *\~english
		 *\brief		Retrieves number of faces
		 *\~french
		 *\brief		Récupère le nombre de faces
		 */
		inline uint32_t GetNbFaces()const { return m_nbFaces; }
		/**
		 *\~english
		 *\brief		Retrieves icosahedron radius
		 *\~french
		 *\brief		Récupère le rayon de l'icosaèdre
		 */
		inline real GetRadius()const { return m_radius; }
		/**
		 *\~english
		 *\brief		Stream operator
		 *\~french
		 *\brief		Operateur de flux
		 */
		inline friend std::ostream & operator <<( std::ostream & o, Icosahedron const & c) { return o << "Icosaedre(" << c.m_nbFaces << "," << c.m_radius << ")";}
	};
}

#pragma warning( pop )

#endif
