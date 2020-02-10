/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ModelMeshModule_H___
#define ___C3D_ModelMeshModule_H___

#include "Castor3D/Model/ModelModule.hpp"

namespace castor3d
{
	/**@name Model */
	//@{
	/**@name Mesh */
	//@{

	/*!
	\~english
	\brief		The mesh representation
	\remark		A mesh is a collection of submeshes.
	\~french
	\brief		Représentation d'un maillage
	\remark		Un maillage est une collectionde sous maillages.
	*/
	class Mesh;
	/*!
	\version	0.7.0
	\~english
	\brief		The mesh factory
	\~french
	\brief		La fabrique de maillages
	*/
	class MeshFactory;
	/*!
	\version	0.7.0
	\~english
	\brief		Mesh category is used to create a mesh's vertices, it is created by MeshFactory
	\remark		Custom mesh category doesn't generate vertices.
	\~french
	\brief		La catégorie de maillage est utilisée afin de créer les sommets du maillage, c'est créé via la MeshFactory
	\remark		La categorie de type Custom ne génère pas de points
	*/
	class MeshGenerator;
	/*!
	\version	0.1.0
	\~english
	\brief		Base class for external file import
	\~french
	\brief		Classe de base pour l'import de fichiers externes
	*/
	class MeshImporter;
	/*!
	\version	0.9.0
	\~english
	\brief		The importer factory.
	\~french
	\brief		La fabrique d'importeurs.
	*/
	class MeshImporterFactory;
	/*!
	\~english
	\remark		Abstract class C3D_API for subdivisers, contains the header for the main Subdivide function
	\~french
	\remark		Contient l'interface commune aux subdiviseurs
	*/
	class MeshSubdivider;

	CU_DeclareSmartPtr( Mesh );
	CU_DeclareSmartPtr( MeshFactory );
	CU_DeclareSmartPtr( MeshGenerator );
	CU_DeclareSmartPtr( MeshImporter );
	CU_DeclareSmartPtr( MeshSubdivider );

	//! Mesh pointer array
	CU_DeclareMap( castor::String, MeshSPtr, MeshPtrStr );

	/*!
	\version	0.9.0
	\~english
	\brief		The subdivider factory.
	\~french
	\brief		La fabrique de subdiviseurs.
	*/
	using MeshSubdividerFactory = castor::Factory< MeshSubdivider, castor::String, MeshSubdividerUPtr >;

	//@}
	//@}
}

#endif
