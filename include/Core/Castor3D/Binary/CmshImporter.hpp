/*
See LICENSE file in root folder
*/
#ifndef ___C3D__CMSH_IMPORTER_H___
#define ___C3D__CMSH_IMPORTER_H___

#include "BinaryModule.hpp"

#include "Castor3D/Model/Mesh/Importer.hpp"

namespace castor3d
{
	class CmshImporter
		: public MeshImporter
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	engine	The core engine.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	engine	Le moteur.
		 */
		C3D_API explicit CmshImporter( Engine & engine );
		/**
		 *\~english
		 *\brief		Creator function.
		 *\param[in]	engine	The core engine.
		 *\~french
		 *\brief		Fonction de création.
		 *\param[in]	engine	Le moteur.
		 */
		static MeshImporterUPtr create( Engine & engine );

	protected:
		/**
		 *\copydoc		castor3d::Importer::doImportMesh
		 */
		C3D_API bool doImportMesh( Mesh & mesh )override;

	public:
		static castor::String const Type;
	};
}

#endif
