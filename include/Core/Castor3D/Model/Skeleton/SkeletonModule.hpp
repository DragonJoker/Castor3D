/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ModelSkeletonModule_H___
#define ___C3D_ModelSkeletonModule_H___

#include "Castor3D/Cache/CacheModule.hpp"
#include "Castor3D/Model/ModelModule.hpp"

#include <CastorUtils/Math/Quaternion.hpp>

namespace castor3d
{
	/**@name Model */
	//@{
	/**@name Skeleton */
	//@{

	/**
	\~english
	\brief		Skeleton node types enumeration.
	\~french
	\brief		Enumération des types de noeud de squelette.
	*/
	enum class SkeletonNodeType
	{
		//!\~english	Node objects.
		//!\~french		Objets noeud.
		eNode,
		//!\~english	Bone objects.
		//!\~french		Objets os.
		eBone,
		CU_ScopedEnumBounds( eNode )
	};
	C3D_API castor::String getName( SkeletonNodeType value );
	/**
	*\~english
	*\brief
	*	A node in the skeleton.
	*\~french
	*\brief
	*	Un noeud dans le squelette.
	*/
	class SkeletonNode;
	/**
	*\~english
	*\brief
	*	Class holding bone data.
	*\~french
	*\brief
	*	Classe contenant les données d'un bone.
	*/
	class BoneNode;
	/**
	*\~english
	*\brief
	*	The skeleton, holds each skeleton node.
	*\~french
	*\brief
	*	Le squelette, contient chaque noeud de squelette.
	*/
	class Skeleton;
	/**
	*\~english
	*\brief
	*	Holds bone data for one vertex
	*\~french
	*\brief
	*	Contient les données de bones pour un vertice
	*/
	struct VertexBoneData;
	/**
	*\~english
	*\brief
	*	Base class for external file import
	*\~french
	*\brief
	*	Classe de base pour l'import de fichiers externes
	*/
	class SkeletonImporter;
	/**
	*\~english
	*\brief
	*	The importer factory.
	*\~french
	*\brief
	*	La fabrique d'importeurs.
	*/
	class SkeletonImporterFactory;
	/**
	*\~english
	*	Helper structure to specialise a cache behaviour.
	*\remarks
	*	Specialisation for Skeleton.
	*\~french
	*	Structure permettant de spécialiser le comportement d'un cache.
	*\remarks
	*	Spécialisation pour Skeleton.
	*/
	template<>
	struct PtrCacheTraitsT< Skeleton, castor::String >
		: CUPtrCacheTraitsBaseT< Skeleton, castor::String >
	{
		using ResT = Skeleton;
		using KeyT = castor::String;
		using Base = CUPtrCacheTraitsBaseT< ResT, KeyT >;
		using ElementT = typename Base::ElementT;
		using ElementPtrT = typename Base::ElementPtrT;

		C3D_API static const castor::String Name;
	};

	using SkeletonCacheTraits = PtrCacheTraitsT< Skeleton, castor::String >;
	using SkeletonCache = castor::ResourceCacheT< Skeleton
		, castor::String
		, SkeletonCacheTraits >;

	using SkeletonRes = SkeletonCacheTraits::ElementPtrT;
	using SkeletonResPtr = SkeletonCacheTraits::ElementObsT;

	CU_DeclareSmartPtr( SkeletonImporter );

	CU_DeclareCUSmartPtr( castor3d, BoneNode, C3D_API );
	CU_DeclareCUSmartPtr( castor3d, SkeletonNode, C3D_API );
	CU_DeclareCUSmartPtr( castor3d, Skeleton, C3D_API );
	CU_DeclareCUSmartPtr( castor3d, SkeletonCache, C3D_API );
	CU_DeclareCUSmartPtr( castor3d, SkeletonImporterFactory, C3D_API );

	//! Skinned vertex data array
	CU_DeclareVector( VertexBoneData, VertexBoneData );
	//! Bone pointer array
	CU_DeclareVector( SkeletonNodeUPtr, SkeletonNodePtr );

	struct NodeTransform
	{
		castor::Point3f translate{};
		castor::Point3f scale{};
		castor::Quaternion rotate{};
	};

	//@}
	//@}
}

#endif
