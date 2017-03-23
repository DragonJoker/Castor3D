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
#ifndef ___C3D_TORUS_H___
#define ___C3D_TORUS_H___

#include "Mesh/MeshGenerator.hpp"

namespace Castor3D
{
	/*!
	\author		Sylvain DOREMUS
	\date		14/02/2010
	\~english
	\brief		The torus representation
	\remark		A torus is an ellipse performing an horizontal ellipse.
				<br />The original ellipse will be called internal and has its own radius and number of subsections
	\~french
	\brief		Représentation d'un torre
	\remark		Un torre est une ellipse qui parcourt une ellipse
				<br />L'ellipse originale sera appelée interne et possède son propre rayon et nombre de sections
	*/
	class Torus
		: public MeshGenerator
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		C3D_API Torus();
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~Torus();
		/**
		 *\copydoc		Castor3D::MeshGenerator::Create
		 */
		C3D_API static MeshGeneratorSPtr Create();
		/**
		 *\~english
		 *\brief		Retrieves the internal ellipse faces number
		 *\~french
		 *\brief		Récupère le nombre de face de l'ellipse interne
		 */
		inline uint32_t	GetInternalNbFaces()const
		{
			return m_uiInternalNbFaces;
		}
		/**
		 *\~english
		 *\brief		Retrieves the external ellipse faces number
		 *\~french
		 *\brief		Récupère le nombre de face de l'ellipse externe
		 */
		inline uint32_t	GetExternalNbFaces()const
		{
			return m_uiExternalNbFaces;
		}
		/**
		 *\~english
		 *\brief		Retrieves the internal ellipse radius
		 *\~french
		 *\brief		Récupère le rayon de l'ellipse interne
		 */
		inline real GetInternalRadius()const
		{
			return m_rInternalRadius;
		}
		/**
		 *\~english
		 *\brief		Retrieves the external ellipse radius
		 *\~french
		 *\brief		Récupère le rayon de l'ellipse externe
		 */
		inline real GetExternalRadius()const
		{
			return m_rExternalRadius;
		}

	private:
		/**
		*\copydoc		Castor3D::MeshGenerator::DoGenerate
		*/
		C3D_API virtual void DoGenerate( Mesh & p_mesh, UIntArray const & p_faces, RealArray const & p_dimensions );

	private:
		real m_rInternalRadius;
		real m_rExternalRadius;
		uint32_t m_uiInternalNbFaces;
		uint32_t m_uiExternalNbFaces;
		friend std::ostream & operator <<( std::ostream & o, Torus const & c );
	};
	/**
	 *\~english
	 *\brief		Stream operator
	 *\~french
	 *\brief		Operateur de flux
	 */
	inline std::ostream & operator <<( std::ostream & o, Torus const & c )
	{
		return o << "Torus(" << c.m_uiInternalNbFaces << "," << c.m_uiExternalNbFaces << "," << c.m_rInternalRadius << "," << c.m_rExternalRadius << ")";
	}
}

#endif
