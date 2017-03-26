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
#ifndef ___C3D_ICOSAHEDRON_H___
#define ___C3D_ICOSAHEDRON_H___

#include "Mesh/MeshGenerator.hpp"

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
	class Icosahedron
		: public MeshGenerator
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		C3D_API Icosahedron();
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~Icosahedron();
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
		 *\brief		Retrieves icosahedron radius
		 *\~french
		 *\brief		Récupère le rayon de l'icosaèdre
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
