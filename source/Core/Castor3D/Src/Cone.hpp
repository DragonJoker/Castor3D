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
#ifndef ___C3D_CONE_H___
#define ___C3D_CONE_H___

#include "MeshGenerator.hpp"

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\date		14/02/2010
	\~english
	\brief		The cone is a basic primitive, with a parametrable number of faces
	\~french
	\brief		Le cône est une primitive basique, avec un nombre paramétrable de faces
	*/
	class Cone
		: public MeshGenerator
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		C3D_API Cone();
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~Cone();
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
		 *\brief		Retrieves cone height
		 *\~french
		 *\brief		Récupère la hauteur du cône
		 */
		inline real GetHeight()const
		{
			return m_height;
		}
		/**
		 *\~english
		 *\brief		Retrieves cone base radius
		 *\~french
		 *\brief		Récupère le rayon de la base du cône
		 */
		inline real GetRadius()const
		{
			return m_radius;
		}

	private:
		//!\~english The number of faces	\~french Nombre de faces
		uint32_t m_nbFaces;
		//!\~english  The cone height	\~french La hauteur du cône
		real m_height;
		//!\~english  The cone radius	\~french Le rayon de la base
		real m_radius;
		friend std::ostream & operator <<( std::ostream & o, Cone const & c );
	};
	/**
	 *\~english
	 *\brief		Stream operator
	 *\~french
	 *\brief		Operateur de flux
	 */
	inline std::ostream & operator <<( std::ostream & o, Cone const & c )
	{
		return o << "Cone(" << c.m_nbFaces << "," << c.m_height << "," << c.m_radius << ")";
	}
}

#endif
