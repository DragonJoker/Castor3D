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
#ifndef ___PLY_IMPORTER_H___
#define ___PLY_IMPORTER_H___

#pragma warning( disable:4251 )
#pragma warning( disable:4275 )

#pragma warning( push )
#pragma warning( disable:4311 )
#pragma warning( disable:4312 )

#include <Mesh/Importer.hpp>
#include <Mesh/Buffer/Buffer.hpp>

#pragma warning( pop )

namespace C3dPly
{
	//! PLY file importer
	/*!
	Imports data from PLY files
	\author Sylvain DOREMUS
	\date 25/08/2010
	*/
	class PlyImporter
		: public Castor3D::Importer
	{
	public:
		/**
		 * Constructor
		 */
		PlyImporter( Castor3D::Engine & engine );

		static Castor3D::ImporterUPtr Create( Castor3D::Engine & engine );

	private:
		/**
		 *\copydoc		Castor3D::Importer::DoImportScene
		 */
		bool DoImportScene( Castor3D::Scene & p_scene )override;
		/**
		 *\copydoc		Castor3D::Importer::DoImportMesh
		 */
		bool DoImportMesh( Castor3D::Mesh & p_mesh )override;
	};
}

#endif
