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
#ifndef ___C3D_PLANE_H___
#define ___C3D_PLANE_H___

#include "Mesh/MeshGenerator.hpp"

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
		/**
		*\copydoc		Castor3D::MeshGenerator::DoGenerate
		*/
		C3D_API virtual void DoGenerate( Mesh & p_mesh, UIntArray const & p_faces, RealArray const & p_dimensions );

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
