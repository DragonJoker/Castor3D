/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ModelMeshModule_H___
#define ___C3D_ModelMeshModule_H___

#include "Castor3D/Model/ModelModule.hpp"

#include <CastorUtils/Design/Factory.hpp>

namespace castor3d
{
	/**@name Model */
	//@{
	/**@name Mesh */
	//@{

	/**
	*\~english
	*\brief
	*	The mesh representation
	*\remarks
	*	A mesh is a collection of submeshes.
	*\~french
	*\brief
	*	Représentation d'un maillage
	*\remarks
	*	Un maillage est une collectionde sous maillages.
	*/
	class Mesh;
	/**
	*\~english
	*\brief
	*	The mesh factory
	*\~french
	*\brief
	*	La fabrique de maillages
	*/
	class MeshFactory;
	/**
	*\~english
	*\brief
	*	Mesh category is used to create a mesh's vertices, it is created by MeshFactory
	*\remarks
	*	Custom mesh category doesn't generate vertices.
	*\~french
	*\brief
	*	La catégorie de maillage est utilisée afin de créer les sommets du maillage, c'est créé via la MeshFactory
	*\remarks
	*	La categorie de type Custom ne génère pas de points
	*/
	class MeshGenerator;
	/**
	*\~english
	*\brief
	*	Base class for external file import
	*\~french
	*\brief
	*	Classe de base pour l'import de fichiers externes
	*/
	class MeshImporter;
	/**
	*\~english
	*\brief
	*	The importer factory.
	*\~french
	*\brief
	*	La fabrique d'importeurs.
	*/
	class MeshImporterFactory;
	/**
	*\~english
	*\brief
	*	Abstract class C3D_API for subdivisers, contains the header for the main Subdivide function
	*\~french
	*\brief
	*	Contient l'interface commune aux subdiviseurs
	*/
	class MeshSubdivider;
	/**
	*\~english
	*\brief
	*	The subdivider factory.
	*\~french
	*\brief
	*	La fabrique de subdiviseurs.
	*/
	using MeshSubdividerFactory = castor::Factory< MeshSubdivider, castor::String, std::unique_ptr< MeshSubdivider > >;

	CU_DeclareSmartPtr( Mesh );
	CU_DeclareSmartPtr( MeshGenerator );
	CU_DeclareSmartPtr( MeshImporter );
	CU_DeclareSmartPtr( MeshSubdivider );

	CU_DeclareCUSmartPtr( castor3d, MeshFactory, C3D_API );
	CU_DeclareCUSmartPtr( castor3d, MeshImporterFactory, C3D_API );
	CU_DeclareCUSmartPtr( castor3d, MeshSubdividerFactory, C3D_API );

	//! Mesh pointer array
	CU_DeclareMap( castor::String, MeshSPtr, MeshPtrStr );

	//@}
	//@}
}

#endif
