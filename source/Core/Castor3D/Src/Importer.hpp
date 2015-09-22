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
#ifndef ___C3D_IMPORTER_H___
#define ___C3D_IMPORTER_H___

#include "Castor3DPrerequisites.hpp"
#include "Parameter.hpp"

#include <Path.hpp>

#pragma warning( push )
#pragma warning( disable:4251 )
#pragma warning( disable:4275 )

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.1
	\date		25/08/2010
	\~english
	\brief		Base class for external file import
	\~french
	\brief		Classe de base pour l'import de fichiers externes
	*/
	class C3D_API Importer
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_engine		The core engine
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_engine		Le moteur
		 */
		Importer( Engine * p_engine );
		/**
		 *\~english
		 *\brief		Scene import Function
		 *\param[in]	p_pathFile		The location of the file to import
		 *\param[in]	p_parameters	Import configuration parameters
		 *\return		The imported Scene
		 *\~french
		 *\brief		Fonction d'import de Scene
		 *\param[in]	p_pathFile		Le chemin vers le fichier à importer
		 *\param[in]	p_parameters	Paramètres de configuration de l'import
		 *\return		La Scene importée
		 */
		SceneSPtr ImportScene( Castor::Path const & p_pathFile, Parameters const & p_parameters );
		/**
		 *\~english
		 *\brief		Mesh import Function
		 *\param[in]	p_pathFile		The location of the file to import
		 *\param[in]	p_parameters	Import configuration parameters
		 *\return		The imported Mesh
		 *\~french
		 *\brief		Fonction d'import de Mesh
		 *\param[in]	p_pathFile		Le chemin vers le fichier à importer
		 *\param[in]	p_parameters	Paramètres de configuration de l'import
		 *\return		Le Mesh importé
		 */
		MeshSPtr ImportMesh( Castor::Path const & p_pathFile, Parameters const & p_parameters );
		/**
		 *\~english
		 *\brief		Retrieves the core engine
		 *\return		The engine
		 *\~french
		 *\brief		Récupère le moteur
		 *\return		Le moteur
		 */
		inline Engine * GetEngine()const
		{
			return m_engine;
		}

	protected:
		/**
		 *\~english
		 *\brief		Scene import Function
		 *\return		The imported Scene
		 *\~french
		 *\brief		Fonction d'import de Scene
		 *\return		La Scene importée
		 */
		virtual SceneSPtr DoImportScene() = 0;
		/**
		 *\~english
		 *\brief		Mesh import Function
		 *\return		The imported Mesh
		 *\~french
		 *\brief		Fonction d'import de Mesh
		 *\return		Le Mesh importé
		 */
		virtual MeshSPtr DoImportMesh() = 0;

	protected:
		//!\~english The file name	\~french Le nom du fichier
		Castor::Path m_fileName;
		//!\~english The file full path	\~french Le chemin complet du fichier
		Castor::Path m_filePath;
		//!\~english The loaded scene nodes	\~french Les noeuds chargés
		SceneNodePtrArray m_nodes;
		//!\~english The core engine	\~french Le moteur
		Engine * m_engine;
		//!\~english The loaded geometries	\~french Les géométries chargées
		GeometryPtrStrMap m_geometries;
		//!\~english Import configuration parameters	\~french Paramètres de configuration de l'import
		Parameters m_parameters;
	};
}

#pragma warning( pop )

#endif
