/*
See LICENSE file in root folder
*/
#ifndef ___PLY_IMPORTER_H___
#define ___PLY_IMPORTER_H___

#pragma warning( disable:4251 )
#pragma warning( disable:4275 )

#pragma warning( push )
#pragma warning( disable:4311 )
#pragma warning( disable:4312 )

#include <Castor3D/Model/Mesh/Importer.hpp>

#pragma warning( pop )

namespace C3dPly
{
	//! PLY file importer
	/**
	Imports data from PLY files
	\author Sylvain DOREMUS
	\date 25/08/2010
	*/
	class PlyImporter
		: public castor3d::MeshImporter
	{
	public:
		/**
		 * Constructor
		 */
		explicit PlyImporter( castor3d::Engine & engine );

		static castor3d::MeshImporterUPtr create( castor3d::Engine & engine );

	private:
		/**
		 *\copydoc		castor3d::MeshImporter::doImportMesh
		 */
		bool doImportMesh( castor3d::Mesh & p_mesh )override;
	};
}

#endif
