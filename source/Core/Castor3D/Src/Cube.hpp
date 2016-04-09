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
#ifndef ___C3D_CUBE_H___
#define ___C3D_CUBE_H___

#include "MeshGenerator.hpp"

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\date		14/02/2010
	\~english
	\brief		The cube representation
	\remark		The calling of cube is abusive because it has parametrable dimensions (width, height and depth)
	\~french
	\brief		Représentation d'un cube
	\remark		La dénomination "Cube" est un abus de langage car ses 3 dimensions sont paramétrables (largeur, hauteur et profondeur)
	*/
	class Cube
		: public MeshGenerator
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		C3D_API Cube();
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~Cube();
		/**
		 *\copydoc		Castor3D::MeshGenerator::Create
		 */
		C3D_API static MeshGeneratorSPtr Create();
		/**
		 *\~english
		 *\brief		Retrieves the cube height
		 *\~french
		 *\brief		Récupère la hauteur du cube
		 */
		inline real GetHeight()const
		{
			return m_height;
		}
		/**
		 *\~english
		 *\brief		Retrieves the cube width
		 *\~french
		 *\brief		Récupère la largeur du cube
		 */
		inline real GetWidth()const
		{
			return m_width;
		}
		/**
		 *\~english
		 *\brief		Retrieves the cube depth
		 *\~french
		 *\brief		Récupère la profondeur du cube
		 */
		inline real GetDepth()const
		{
			return m_depth;
		}

	private:
		/**
		*\copydoc		Castor3D::MeshGenerator::DoGenerate
		*/
		C3D_API virtual void DoGenerate( Mesh & p_mesh, UIntArray const & p_faces, RealArray const & p_dimensions );

	private:
		real m_height;
		real m_width;
		real m_depth;
		friend std::ostream & operator <<( std::ostream & o, Cube const & c );
	};
	/**
	 *\~english
	 *\brief		Stream operator
	 *\~french
	 *\brief		Opérateur de flux
	 */
	inline std::ostream & operator <<( std::ostream & o, Cube const & c )
	{
		return o << "Cube(" << c.m_height << "," << c.m_width << "," << c.m_depth << ")";
	}
}

#endif
