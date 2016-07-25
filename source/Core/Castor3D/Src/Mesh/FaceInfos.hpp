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
#ifndef ___C3D_FACE_INFOS_H___
#define ___C3D_FACE_INFOS_H___

#include "Castor3DPrerequisites.hpp"

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		20/07/2012
	\~english
	\brief		Holds face informations.
	\~french
	\brief		Contient des informations sur une face.
	*/
	struct stFACE_INFOS
	{
		/*!
		\author 	Sylvain DOREMUS
		\date 		20/07/2012
		\~english
		\brief		Holds vertex informations.
		\~french
		\brief		Contient les informations d'un sommet.
		\remark
		*/
		struct stVERTEX
		{
			//!\~english	Vertex index.
			//!\~french		Indice du sommet.
			int m_index{ -1 };
			//!\~english	Vertex normal.
			//!\~french		Normale du sommet.
		  std::array< real, 3 > m_fNormal{ { 0.0_r, 0.0_r, 0.0_r } };
			//!\~english	Vertex UV.
			//!\~french		UV du sommet.
			std::array< real, 2 > m_fTexCoords{ { 0.0_r, 0.0_r } };
			//!\~english	Tells the vertex has a normal.
			//!\~french		Dit que le sommet a une normale.
			bool m_bHasNormal{ false };
			//!\~english	Tells the vertex has UV.
			//!\~french		Dit que le sommet a un UV.
			bool m_bHasTexCoords{ false };
		};
		//!\~english	The 3 vertex informations.
		//!\~french		Les informations des 3 sommets.
		std::array< stVERTEX, 3 > m_vertex;
	};
}

#endif
