/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ModelSkeletonModule_H___
#define ___C3D_ModelSkeletonModule_H___

#include "Castor3D/Cache/CacheModule.hpp"
#include "Castor3D/Model/ModelModule.hpp"

#include <CastorUtils/Math/Quaternion.hpp>

namespace c3d
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
		eNode = 0,
		//!\~english	Bone objects.
		//!\~french		Objets os.
		eBone = 1,
		CU_ScopedEnumBounds( eNode, eBone )
	};
	C3D_API String getName( SkeletonNodeType value );
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
	struct PtrCacheTraitsT< Skeleton, String >
		: PtrCacheTraitsBaseT< Skeleton, String >
	{
		using ResT = Skeleton;
		using KeyT = String;
		using Base = PtrCacheTraitsBaseT< ResT, KeyT >;
		using ElementT = typename Base::ElementT;
		using ElementPtrT = typename Base::ElementPtrT;

		C3D_API static const String Name;
	};

	using SkeletonCacheTraits = PtrCacheTraitsT< Skeleton, String >;
	using SkeletonCache = ResourceCacheT< Skeleton
		, String
		, SkeletonCacheTraits >;

	using SkeletonRes = SkeletonCacheTraits::ElementPtrT;
	using SkeletonResPtr = SkeletonCacheTraits::ElementObsT;

	/** @cond !Doxygen */
	CU_DeclareSmartPtr( c3d, BoneNode, C3D_API );
	CU_DeclareSmartPtr( c3d, SkeletonNode, C3D_API );
	CU_DeclareSmartPtr( c3d, Skeleton, C3D_API );
	CU_DeclareSmartPtr( c3d, SkeletonCache, C3D_API );
	CU_DeclareSmartPtr( c3d, SkeletonImporter, C3D_API );
	CU_DeclareSmartPtr( c3d, SkeletonImporterFactory, C3D_API );

	//! Skinned vertex data array
	CU_DeclareVector( VertexBoneData, VertexBoneData );
	//! Bone pointer array
	CU_DeclareVector( SkeletonNodeUPtr, SkeletonNodePtr );
	/** @endcond */

	struct NodeTransform
	{
		Point3f translate{};
		Point3f scale{ 1.0f, 1.0f, 1.0f };
		Quaternion rotate{};
	};

	inline bool operator==( NodeTransform const & lhs, NodeTransform const & rhs )noexcept
	{
		return lhs.translate == rhs.translate
			&& lhs.scale == rhs.scale
			&& lhs.rotate == rhs.rotate;
	}

	//@}
	//@}
}

#endif
