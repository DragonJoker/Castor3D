/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef ___C3D_CYLINDER_H___
#define ___C3D_CYLINDER_H___

#include "Mesh/MeshGenerator.hpp"

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\date		14/02/2010
	\~english
	\brief		The cylinder representation
	\remark		The cylinder is a basic primitive, with a parametrable number of faces
	\~french
	\brief		Représentation d'un cylindre
	\remark		Un cylindre est une primitive basique avec un nombre de faces paramétrable
	*/
	class Cylinder
		: public MeshGenerator
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		C3D_API Cylinder();
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~Cylinder();
		/**
		 *\copydoc		Castor3D::MeshGenerator::Create
		 */
		C3D_API static MeshGeneratorSPtr Create();
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
		 *\brief		Retrieves cylinder's height
		 *\~french
		 *\brief		Récupère la hauteur du cylindre
		 */
		inline real GetHeight()const
		{
			return m_height;
		}
		/**
		 *\~english
		 *\brief		Retrieves cylinder's radius
		 *\~french
		 *\brief		Récupère le rayon de la base du cylindre
		 */
		inline real GetRadius()const
		{
			return m_radius;
		}

	private:
		/**
		*\copydoc		Castor3D::MeshGenerator::DoGenerate
		*/
		C3D_API virtual void DoGenerate( Mesh & p_mesh, Parameters const & p_parameters );

	private:
		real m_height;
		real m_radius;
		uint32_t m_nbFaces;
		friend std::ostream & operator <<( std::ostream & o, Cylinder const & c );
	};
	/**
	 *\~english
	 *\brief		Stream operator
	 *\~french
	 *\brief		Opérateur de flux
	 */
	inline std::ostream & operator <<( std::ostream & o, Cylinder const & c )
	{
		return o << "Cylinder(" << c.m_nbFaces << "," << c.m_height << "," << c.m_radius << ")";
	}
}

#endif
