/*
See LICENSE file in root folder
*/
#ifndef ___C3D__CMSH_IMPORTER_H___
#define ___C3D__CMSH_IMPORTER_H___

#include "Importer.hpp"

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.9.0
	\date		24/05/2016
	\~english
	\brief		Used to import meshes from cmsh files.
	\~french
	\brief		Utilisé pour importer les maillages depuis des fichiers cmsh.
	*/
	class CmshImporter
		: public Importer
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
		static ImporterUPtr create( Engine & engine );

	protected:
		/**
		 *\copydoc		castor3d::Importer::doImportScene
		 */
		C3D_API bool doImportScene( Scene & scene )override;
		/**
		 *\copydoc		castor3d::Importer::doImportMesh
		 */
		C3D_API bool doImportMesh( Mesh & mesh )override;

	public:
		static castor::String const Type;
	};
}

#endif
