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
#ifndef ___C3D_VERTEX_GROUP_H___
#define ___C3D_VERTEX_GROUP_H___

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		23/07/2012
	\~english
	\brief		Holds the all components of a vertex.
	\~french
	\brief		Contient toutes les composantes d'un sommet.
	*/
	template< typename T >
	struct InterleavedVertexT
	{
		//!\~english	The vertex coordinates.
		//!\~french		La position du sommet.
		std::array< T, 3 > m_pos;
		//!\~english	The vertex normal.
		//!\~french		La normale du sommet.
		std::array< T, 3 > m_nml;
		//!\~english	The vertex tangent.
		//!\~french		La tangente du sommet.
		std::array< T, 3 > m_tan;
		//!\~english	The vertex bitangent.
		//!\~french		La bitangente du sommet.
		std::array< T, 3 > m_bin;
		//!\~english	The vertex texture coordinates.
		//!\~french		La coordonnées de texture du sommet.
		std::array< T, 3 > m_tex;
	};

	using InterleavedVertex = InterleavedVertexT< real >;
	using InterleavedVertexArray = std::vector< InterleavedVertex >;
}

#endif
