/* See LICENSE file in root folder */
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
	class AseImporter
		: public castor3d::Importer
	{
	public:
		AseImporter( castor3d::Engine & p_engine );
		~AseImporter();

		static castor3d::ImporterUPtr create( castor3d::Engine & p_engine );

	private:
		/**
		 *\copydoc		castor3d::Importer::doImportScene
		 */
		bool doImportScene( castor3d::Scene & p_scene )override;
		/**
		 *\copydoc		castor3d::Importer::doImportMesh
		 */
		bool doImportMesh( castor3d::Mesh & p_mesh )override;
	};
}

#endif
