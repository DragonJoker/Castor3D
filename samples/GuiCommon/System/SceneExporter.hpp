/*
See LICENSE file in root folder
*/
#ifndef ___CV_SCENE_EXPORTER_H___
#define ___CV_SCENE_EXPORTER_H___

#include "GuiCommon/GuiCommonPrerequisites.hpp"

#include <CastorUtils/Data/Path.hpp>

namespace GuiCommon
{
	/**
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
		 *\param[in]	p_scene		La scène à exporter.
		 *\param[in]	p_fileName	Le nom du fichier exportà.
		 */
		virtual void exportScene( castor3d::Scene const & p_scene, castor::Path const & p_fileName ) = 0;
	};
	/**
	\author 	Sylvain DOREMUS
	\date 		16/09/2015
	\version	0.8.0
	\~english
	\brief		Scene exporter, to Wavefront OBJ files.
	\~french
	\brief		Classe d'exporteur de scène, vers fichiers Wavefront OBJ.
	*/
	class ObjSceneExporter
		: public SceneExporter
	{
	public:
		void exportScene( castor3d::Scene const & p_scene, castor::Path const & p_fileName )override;

	private:
		void doExportMaterials( castor3d::Scene const & p_scene, castor::Path const & p_path )const;
		void doExportMeshes( castor3d::Scene const & p_scene, castor::Path const & p_mtlpath, castor::Path const & p_path )const;
		castor::String doExportMaterial( castor::Path const & p_pathMtlFolder, castor3d::Material const & p_material )const;
		castor::String doExportTexture( castor::Path const & p_pathMtlFolder, castor::String p_section, castor3d::TextureUnitSPtr p_unit )const;
		castor::String doExportMesh( castor3d::Mesh const & p_mesh, uint32_t & p_offset, uint32_t & p_count )const;
	};
	/**
	\author 	Sylvain DOREMUS
	\date 		16/09/2015
	\version	0.8.0
	\~english
	\brief		Scene exporter, to Castor3D scene files.
	\~french
	\brief		Classe d'exporteur de scène, vers fichiers de scène Castor3D.
	*/
	class CscnSceneExporter
		: public SceneExporter
	{
	public:
		void exportScene( castor3d::Scene const & p_scene, castor::Path const & p_fileName )override;
	};
}

#endif
