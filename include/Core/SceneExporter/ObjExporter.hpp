/* See LICENSE file in root folder */
#ifndef ___CSE_ObjSceneExporter_HPP___
#define ___CSE_ObjSceneExporter_HPP___

#include "SceneExporter.hpp"

#include <Castor3D/Material/MaterialModule.hpp>
#include <Castor3D/Material/Texture/TextureModule.hpp>

namespace castor3d::exporter
{
	/**
	\~english
	\brief		Scene exporter, to Wavefront OBJ files.
	\~french
	\brief		Classe d'exporteur de scène, vers fichiers Wavefront OBJ.
	*/
	class ObjSceneExporter
		: public SceneExporter
	{
	public:
		CSE_API ObjSceneExporter( ExportOptions options );
		CSE_API bool exportScene( castor3d::Scene const & scene
			, castor::Path const & fileName )override;

	private:
		void doExportMaterials( castor3d::Scene const & scene
			, castor::Path const & path )const;
		void doExportMeshes( castor3d::Scene const & scene
			, castor::Path const & mtlpath
			, castor::Path const & path )const;
		castor::String doExportMaterial( castor::Path const & pathMtlFolder
			, castor3d::Material const & material )const;
		castor::String doExportTexture( castor::Path const & pathMtlFolder
			, castor::String section
			, castor3d::TextureUnitSPtr unit )const;
		castor::String doExportMesh( castor3d::Mesh const & mesh
			, uint32_t & offset
			, uint32_t & count )const;
	};
}

#endif
