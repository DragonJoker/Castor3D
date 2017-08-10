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
#ifndef ___CV_SCENE_EXPORTER_H___
#define ___CV_SCENE_EXPORTER_H___

#include "GuiCommonPrerequisites.hpp"

#include <Data/Path.hpp>

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
		 *\param[in]	p_scene		La scène à exporter.
		 *\param[in]	p_fileName	Le nom du fichier exportà.
		 */
		virtual void ExportScene( castor3d::Scene const & p_scene, castor::Path const & p_fileName ) = 0;
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
		virtual void ExportScene( castor3d::Scene const & p_scene, castor::Path const & p_fileName );

	private:
		void doExportMaterials( castor3d::Scene const & p_scene, castor::Path const & p_path )const;
		void doExportMeshes( castor3d::Scene const & p_scene, castor::Path const & p_mtlpath, castor::Path const & p_path )const;
		castor::String doExportMaterial( castor::Path const & p_pathMtlFolder, castor3d::Material const & p_material )const;
		castor::String doExportTexture( castor::Path const & p_pathMtlFolder, castor::String p_section, castor3d::TextureUnitSPtr p_unit )const;
		castor::String doExportMesh( castor3d::Mesh const & p_mesh, uint32_t & p_offset, uint32_t & p_count )const;
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
		virtual void ExportScene( castor3d::Scene const & p_scene, castor::Path const & p_fileName );
	};
}

#endif
