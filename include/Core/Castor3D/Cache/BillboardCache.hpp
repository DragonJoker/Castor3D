/*
See LICENSE file in root folder
*/
#ifndef ___C3D_BillboardCache_H___
#define ___C3D_BillboardCache_H___

#include "Castor3D/Cache/ObjectCacheBase.hpp"
#include "Castor3D/Scene/SceneModule.hpp"
#include "Castor3D/Shader/ShaderBuffers/ShaderBuffersModule.hpp"

#include "Castor3D/Buffer/UniformBufferOffset.hpp"
#include "Castor3D/Shader/Ubos/UbosModule.hpp"

namespace castor3d
{
	C3D_API size_t hash( BillboardBase const & billboard
		, Pass const & pass );
	using BillboardCacheTraits = ObjectCacheTraitsT< BillboardList, castor::String >;
	/**
	\~english
	\brief		BillboardList cache.
	\~french
	\brief		Cache de BillboardList.
	*/
	template<>
	class ObjectCacheT< BillboardList, castor::String, BillboardCacheTraits > final
		: public ObjectCacheBaseT< BillboardList, castor::String, BillboardCacheTraits >
	{
	public:
		using ElementT = BillboardList;
		using ElementKeyT = castor::String;
		using ElementCacheTraitsT = BillboardCacheTraits;
		using ElementObjectCacheT = ObjectCacheBaseT< ElementT, ElementKeyT, ElementCacheTraitsT >;
		using ElementPtrT = typename ElementObjectCacheT::ElementPtrT;
		using ElementObsT = typename ElementObjectCacheT::ElementObsT;
		using ElementContT = typename ElementObjectCacheT::ElementContT;
		using ElementInitialiserT = typename ElementObjectCacheT::ElementInitialiserT;
		using ElementCleanerT = typename ElementObjectCacheT::ElementCleanerT;
		using ElementMergerT = typename ElementObjectCacheT::ElementMergerT;
		using ElementAttacherT = typename ElementObjectCacheT::ElementAttacherT;
		using ElementDetacherT = typename ElementObjectCacheT::ElementDetacherT;
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	scene			The scene.
		 *\param[in]	rootNode		The root node.
		 *\param[in]	rootCameraNode	The cameras root node.
		 *\param[in]	rootObjectNode	The objects root node.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	scene			La scène.
		 *\param[in]	rootNode		Le noeud racine.
		 *\param[in]	rootCameraNode	Le noeud racine des caméras.
		 *\param[in]	rootObjectNode	Le noeud racine des objets.
		 */
		C3D_API ObjectCacheT( Scene & scene
			, SceneNodeSPtr rootNode
			, SceneNodeSPtr rootCameraNode
			, SceneNodeSPtr rootObjectNode );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~ObjectCacheT() = default;
	};
}

#endif
