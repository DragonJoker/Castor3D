/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)

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
		C3D_API virtual SceneSPtr DoImportScene();
		/**
		 *\copydoc		Castor::Importer::DoImportMesh
		 */
		C3D_API virtual MeshSPtr DoImportMesh( Scene & p_scene );

	public:
		static Castor::String const Type;
	};
}

#endif
