/* See LICENSE file in root folder */
#ifndef ___CSE_CscnSceneExporter_HPP___
#define ___CSE_CscnSceneExporter_HPP___

#include "SceneExporter.hpp"

namespace castor3d::exporter
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
		CSE_API CscnSceneExporter( ExportOptions options );
		CSE_API bool exportScene( castor3d::Scene const & scene
			, castor::Path const & fileName )override;
	};
}

#endif
