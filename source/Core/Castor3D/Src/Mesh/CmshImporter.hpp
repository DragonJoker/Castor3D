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
#ifndef ___C3D__CMSH_IMPORTER_H___
#define ___C3D__CMSH_IMPORTER_H___

#include "Importer.hpp"

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.9.0
	\date		24/05/2016
	\~english
	\brief		Used to import meshes from cmsh files.
	\~french
	\brief		Utilisé pour importer les maillages depuis des fichiers cmsh.
	*/
	class CmshImporter
		: public Importer
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_engine	The core engine.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_engine	Le moteur.
		 */
		C3D_API explicit CmshImporter( Engine & p_engine );
		/**
		 *\~english
		 *\brief		Creator function.
		 *\param[in]	p_engine	The core engine.
		 *\~french
		 *\brief		Fonction de création.
		 *\param[in]	p_engine	Le moteur.
		 */
		static ImporterUPtr Create( Engine & p_engine );

	protected:
		/**
		 *\copydoc		Castor::Importer::DoImportScene
		 */
		C3D_API bool DoImportScene( Scene & p_scene )override;
		/**
		 *\copydoc		Castor::Importer::DoImportMesh
		 */
		C3D_API bool DoImportMesh( Mesh & p_mesh )override;

	public:
		static Castor::String const Type;
	};
}

#endif
