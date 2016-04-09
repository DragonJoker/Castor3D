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
#include <OwnedBy.hpp>

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
		C3D_API Importer( Engine & p_engine );
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
		C3D_API SceneSPtr ImportScene( Castor::Path const & p_pathFile, Parameters const & p_parameters );
		/**
		 *\~english
		 *\brief		Mesh import Function
		 *\param[in]	p_scene			The scene into which the Mesh is loaded.
		 *\param[in]	p_pathFile		The location of the file to import
		 *\param[in]	p_parameters	Import configuration parameters
		 *\return		The imported Mesh
		 *\~french
		 *\brief		Fonction d'import de Mesh
		 *\param[in]	p_scene			La scène dans laquelle le maillage est chargé
		 *\param[in]	p_pathFile		Le chemin vers le fichier à importer
		 *\param[in]	p_parameters	Paramètres de configuration de l'import
		 *\return		Le Mesh importé
		 */
		C3D_API MeshSPtr ImportMesh( Scene & p_scene, Castor::Path const & p_pathFile, Parameters const & p_parameters );
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
		C3D_API TextureUnitSPtr LoadTexture( Castor::Path const & p_path, Pass & p_pass, eTEXTURE_CHANNEL p_channel );

	protected:
		/**
		 *\~english
		 *\brief		Scene import Function
		 *\return		The imported Scene
		 *\~french
		 *\brief		Fonction d'import de Scene
		 *\return		La Scene importée
		 */
		C3D_API virtual SceneSPtr DoImportScene() = 0;
		/**
		 *\~english
		 *\brief		Mesh import Function.
		 *\param[in]	p_scene	The scene into which the Mesh is loaded.
		 *\return		The imported Mesh
		 *\~french
		 *\brief		Fonction d'import de maillage.
		 *\param[in]	p_scene	La scène dans laquelle le maillage est chargé
		 *\return		Le maillage importé.
		 */
		C3D_API virtual MeshSPtr DoImportMesh( Scene & p_scene ) = 0;

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
