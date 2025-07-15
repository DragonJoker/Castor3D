/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ModelMeshModule_H___
#define ___C3D_ModelMeshModule_H___

#include "Castor3D/Limits.hpp"
#include "Castor3D/Cache/CacheModule.hpp"
#include "Castor3D/Model/ModelModule.hpp"

#include <CastorUtils/Design/Factory.hpp>
#include <CastorUtils/Design/Signal.hpp>

namespace c3d
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
	*	Class used to prepare a mesh for rendering.
	*\~french
	*\brief
	*	Classe de préparation d'un mesh pour le rendu.
	*/
	class MeshPreparer;
	/**
	*\~english
	*\brief
	*	Data for one meshlet.
	*\~french
	*\brief
	*	Données pour un meshlet.
	*/
	struct Meshlet
	{
		// Meshlets vertices indices.
		Array< uint32_t, MaxMeshletVertexCount > vertices;
		// Meshlets triangles indices.
		Array< uint8_t, MaxMeshletTriangleCount * 3u > primitives;
		// Number of vertices used in the meshlet.
		uint32_t vertexCount;
		// Number of triangles used in the meshlet.
		uint32_t triangleCount;
		// The index of the meshlet.
		uint32_t meshletIndex;
	};
	static_assert( ( sizeof( Meshlet ) % 16u ) == 0u );
	/**
	*\~english
	*\brief
	*	Cull data for one meshlet.
	*\~french
	*\brief
	*	Données de culling pour un meshlet.
	*/
	struct MeshletCullData
	{
		Point4f sphere;
		Point4f cone;
	};
	/**
	*\~english
	*\brief
	*	Draw constants for one meshlet.
	*\~french
	*\brief
	*	Données de dessin pour un meshlet.
	*/
	struct MeshletDrawConstants
	{
		uint32_t pipelineId;
		uint32_t drawId;
		uint32_t drawOffset;
		uint32_t meshletOffset;
	};
	/**
	*\~english
	*	Helper structure to specialise a cache behaviour.
	*\remarks
	*	Specialisation for Mesh.
	*\~french
	*	Structure permettant de spécialiser le comportement d'un cache.
	*\remarks
	*	Spécialisation pour Mesh.
	*/
	template<>
	struct ResourceCacheTraitsT< Mesh, String >
		: ResourceCacheTraitsBaseT< Mesh, String, ResourceCacheTraitsT< Mesh, String > >
	{
		using ResT = Mesh;
		using KeyT = String;
		using TraitsT = ResourceCacheTraitsT< ResT, KeyT >;
		using Base = ResourceCacheTraitsBaseT< ResT, KeyT, TraitsT >;
		using ElementT = typename Base::ElementT;
		using ElementPtrT = typename Base::ElementPtrT;

		C3D_API static const String Name;
	};

	using MeshCacheTraits = ResourceCacheTraitsT< Mesh, String >;
	using MeshCache = ResourceCacheT< Mesh
		, String
		, MeshCacheTraits >;

	using MeshRes = MeshCacheTraits::ElementPtrT;
	using MeshResPtr = MeshCacheTraits::ElementObsT;

	using MeshChangeFunc = std::function< void( Mesh const & ) >;
	using MeshChangeSignal = SignalT< MeshChangeFunc >;
	using MeshChangeConnection = ConnectionT< MeshChangeSignal >;

	/** @cond !Doxygen */
	CU_DeclareSmartPtr( c3d, Mesh, C3D_API );
	CU_DeclareSmartPtr( c3d, MeshCache, C3D_API );
	CU_DeclareSmartPtr( c3d, MeshFactory, C3D_API );
	CU_DeclareSmartPtr( c3d, MeshGenerator, C3D_API );
	CU_DeclareSmartPtr( c3d, MeshImporter, C3D_API );
	CU_DeclareSmartPtr( c3d, MeshImporterFactory, C3D_API );

	//! Mesh pointer array
	CU_DeclareMap( String, MeshResPtr, MeshPtrStr );
	/** @endcond */

	//@}
	//@}
}

#endif
