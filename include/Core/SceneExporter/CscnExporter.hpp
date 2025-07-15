/* See LICENSE file in root folder */
#ifndef ___CSE_CscnSceneExporter_HPP___
#define ___CSE_CscnSceneExporter_HPP___

#include "SceneExporter.hpp"

namespace c3d::exporter
{
	/**
	\~english
	\brief		Scene exporter, to Castor3D scene files.
	\~french
	\brief		Classe d'exporteur de scène, vers fichiers de scène Castor3D.
	*/
	class CscnSceneExporter
		: public SceneExporter
	{
	public:
		CSE_API explicit CscnSceneExporter( ExportOptions options );
		CSE_API bool exportScene( Scene const & scene
			, Path const & fileName )override;
		CSE_API bool exportMesh( Scene const & scene
			, Mesh const & mesh
			, Path const & outputFolder
			, String const & outputName )override;

	private:
		bool carryOn( bool result )const noexcept;
	};
}

#endif
