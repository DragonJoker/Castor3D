/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

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
#ifndef ___ASE_IMPORTER_H___
#define ___ASE_IMPORTER_H___

#include "AseImporterPrerequisites.hpp"

namespace Ase
{
	/*!
	\author		Sylvain DOREMUS
	\date		25/08/2010
	\~english
	\brief		ASE file importer
	\~french
	\brief		Importeur de fichiers ASE
	*/
	class AseImporter : public Castor3D::Importer
	{
	private:
		AseFileParser * m_pFileParser;

	public:
		AseImporter( Castor3D::Engine & p_pEngine );
		~AseImporter();

	private:
		virtual Castor3D::SceneSPtr DoImportScene();
		virtual Castor3D::MeshSPtr DoImportMesh();
	};
}

#endif
