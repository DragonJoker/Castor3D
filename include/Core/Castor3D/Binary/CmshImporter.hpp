/*
See LICENSE file in root folder
*/
#ifndef ___C3D_CmshImporter_H___
#define ___C3D_CmshImporter_H___

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
		 *\copydoc		castor3d::MeshImporter::doImportSkeleton
		 */
		C3D_API bool doImportSkeleton( Skeleton & skeleton )override;
		/**
		 *\copydoc		castor3d::MeshImporter::doImportMesh
		 */
		C3D_API bool doImportMesh( Mesh & mesh )override;
		/**
		 *\copydoc		castor3d::MeshImporter::doImportScene
		 */
		C3D_API bool doImportScene( castor3d::Scene & scene )override
		{
			return false;
		}

	public:
		static castor::String const MeshType;
		static castor::String const MeshAnimType;
		static castor::String const SkeletonType;
		static castor::String const SkeletonAnimType;
		static castor::String const NodeAnimType;
	};
}

#endif
