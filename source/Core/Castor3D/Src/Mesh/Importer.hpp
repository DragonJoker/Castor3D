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
#ifndef ___C3D_IMPORTER_H___
#define ___C3D_IMPORTER_H___

#include "Castor3DPrerequisites.hpp"
#include "Miscellaneous/Parameter.hpp"

#include <Data/Path.hpp>
#include <Design/OwnedBy.hpp>

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
	class Importer
		: public Castor::OwnedBy< Engine >
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
		C3D_API explicit Importer( Engine & p_engine );
		/**
		 *\~english
		 *\brief		Scene import Function.
		 *\param[out]	p_scene			Receives the imported data.
		 *\param[in]	p_pathFile		The location of the file to import.
		 *\param[in]	p_parameters	Import configuration parameters.
		 *\return		\p false if any problem occured.
		 *\~french
		 *\brief		Fonction d'import de Scene.
		 *\param[out]	p_scene			Reçoit les données importées.
		 *\param[in]	p_pathFile		Le chemin vers le fichier à importer.
		 *\param[in]	p_parameters	Paramètres de configuration de l'import.
		 *\return		\p false si un problème quelconque est survenu.
		 */
		C3D_API bool ImportScene( Scene & p_scene, Castor::Path const & p_pathFile, Parameters const & p_parameters );
		/**
		 *\~english
		 *\brief		Mesh import Function.
		 *\param[out]	p_mesh			Receives the imported data.
		 *\param[in]	p_pathFile		The location of the file to import.
		 *\param[in]	p_parameters	Import configuration parameters.
		 *\param[in]	p_initialise	Tells if the imported mesh must be initialised.
		 *\return		\p false if any problem occured.
		 *\~french
		 *\brief		Fonction d'import de Mesh.
		 *\param[out]	p_mesh			Reçoit les données importées.
		 *\param[in]	p_pathFile		Le chemin vers le fichier à importer.
		 *\param[in]	p_parameters	Paramètres de configuration de l'import.
		 *\param[in]	p_initialise	Dit si le mesh importé doit être initialisé.
		 *\return		\p false si un problème quelconque est survenu.
		 */
		C3D_API bool ImportMesh( Mesh & p_mesh, Castor::Path const & p_pathFile, Parameters const & p_parameters, bool p_initialise );
		/**
		 *\~english
		 *\brief		Loads a texture and adds it to the given pass.
		 *\param[in]	p_path		The image file path (can be relative or absolute).
		 *\param[in]	p_pass		The pass.
		 *\param[in]	p_channel	The channel affected.
		 *\return		\p false if any error occured.
		 *\~french
		 *\brief		Charge une texture et l'ajoute à la passe donnée.
		 *\param[in]	p_path		Le chemin vers l'image (peut être relatif ou absolu).
		 *\param[in]	p_pass		La passe.
		 *\param[in]	p_channel	Le canal affecté à l'image.
		 *\return		\p false en cas d'erreur.
		 */
		C3D_API TextureUnitSPtr LoadTexture( Castor::Path const & p_path, Pass & p_pass, TextureChannel p_channel );

	protected:
		/**
		 *\~english
		 *\brief		Scene import Function.
		 *\param[out]	p_scene	Receives the imported data.
		 *\return		\p false if any problem occured.
		 *\~french
		 *\brief		Fonction d'import de Scene.
		 *\param[out]	p_mesh	Reçoit les données importées.
		 *\return		\p false si un problème quelconque est survenu.
		 */
		C3D_API virtual bool DoImportScene( Scene & p_scene ) = 0;
		/**
		 *\~english
		 *\brief		Mesh import Function.
		 *\param[out]	p_mesh	Receives the imported data.
		 *\return		\p false if any problem occured.
		 *\~french
		 *\brief		Fonction d'import de maillage.
		 *\param[out]	p_mesh	Reçoit les données importées.
		 *\return		\p false si un problème quelconque est survenu.
		 */
		C3D_API virtual bool DoImportMesh( Mesh & p_mesh ) = 0;

	protected:
		//!\~english The file name	\~french Le nom du fichier
		Castor::Path m_fileName;
		//!\~english The file full path	\~french Le chemin complet du fichier
		Castor::Path m_filePath;
		//!\~english The loaded scene nodes	\~french Les noeuds chargés
		SceneNodePtrArray m_nodes;
		//!\~english The loaded geometries	\~french Les géométries chargées
		GeometryPtrStrMap m_geometries;
		//!\~english Import configuration parameters	\~french Paramètres de configuration de l'import
		Parameters m_parameters;
	};
}

#endif
