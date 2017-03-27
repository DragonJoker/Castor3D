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
#ifndef ___C3D_DiamondSquareTerrain_H___
#define ___C3D_DiamondSquareTerrain_H___

#include <Mesh/MeshGenerator.hpp>

namespace diamond_square_terrain
{
	class Generator
		: public Castor3D::MeshGenerator
	{
	public:
		Generator();
		virtual ~Generator();
		/**
		*\copydoc		Castor3D::MeshGenerator::Create
		*/
		static Castor3D::MeshGeneratorSPtr Create();

	private:
		/**
		 *\copydoc		Castor3D::MeshGenerator::DoGenerate
		 */
		virtual void DoGenerate( Castor3D::Mesh & p_mesh
			, Castor3D::Parameters const & p_parameters );

	public:
		static Castor::String const Type;
		static Castor::String const Name;

	private:
	};
}

#endif
