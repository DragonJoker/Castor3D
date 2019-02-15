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

#include <Castor3D/Mesh/Importer.hpp>

#pragma warning( pop )

namespace C3dPly
{
	//! PLY file importer
	/*!
	Imports data from PLY files
	\author Sylvain DOREMUS
	\date 25/08/2010
	*/
	class PlyImporter
		: public castor3d::Importer
	{
	public:
		/**
		 * Constructor
		 */
		explicit PlyImporter( castor3d::Engine & engine );

		static castor3d::ImporterUPtr create( castor3d::Engine & engine );

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
