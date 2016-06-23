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
#ifndef ___CV_SCENE_EXPORTER_H___
#define ___CV_SCENE_EXPORTER_H___

#include "GuiCommonPrerequisites.hpp"

#include <Path.hpp>

namespace GuiCommon
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		16/09/2015
	\version	0.8.0
	\~english
	\brief		Scene exporter base class.
	\~french
	\brief		Classe de base pour les exporteurs de scène.
	*/
	class SceneExporter
	{
	public:
		/**
		 *\~english
		 *\brief		Export function.
		 *\param[in]	p_scene		The scene to export.
		 *\param[in]	p_fileName	The exported file name.
		 *\~french
		 *\brief		Fonction d'export
		 *\param[in]	p_scene		La scène � exporter.
		 *\param[in]	p_fileName	Le nom du fichier export�.
		 */
		virtual void ExportScene( Castor3D::Scene const & p_scene, Castor::Path const & p_fileName ) = 0;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		16/09/2015
	\version	0.8.0
	\~english
	\brief		Scene exporter, to Wavefront OBJ files.
	\~french
	\brief		Classe d'exporteur de scène, vers fichiers Wavefront OBJ.
	*/
	class ObjSceneExporter
	{
	public:
		virtual void ExportScene( Castor3D::Scene const & p_scene, Castor::Path const & p_fileName );

	private:
		void DoExportMaterials( Castor3D::Scene const & p_scene, Castor::Path const & p_path )const;
		void DoExportMeshes( Castor3D::Scene const & p_scene, Castor::Path const & p_mtlpath, Castor::Path const & p_path )const;
		Castor::String DoExportMaterial( Castor::Path const & p_pathMtlFolder, Castor3D::Material const & p_material )const;
		Castor::String DoExportTexture( Castor::Path const & p_pathMtlFolder, Castor::String p_section, Castor3D::TextureUnitSPtr p_unit )const;
		Castor::String DoExportMesh( Castor3D::Mesh const & p_mesh, uint32_t & p_offset, uint32_t & p_count )const;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		16/09/2015
	\version	0.8.0
	\~english
	\brief		Scene exporter, to Castor3D scene files.
	\~french
	\brief		Classe d'exporteur de scène, vers fichiers de scène Castor3D.
	*/
	class CscnSceneExporter
	{
	public:
		virtual void ExportScene( Castor3D::Scene const & p_scene, Castor::Path const & p_fileName );
	};
}

#endif
