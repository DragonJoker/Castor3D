/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ObjectCache_H___
#define ___C3D_ObjectCache_H___

#include "Castor3D/Cache/ObjectCacheBase.hpp"

namespace castor3d
{
	template< typename ObjT, typename KeyT, typename TraitsT >
	class ObjectCacheT final
		: public ObjectCacheBaseT< ObjT, KeyT, TraitsT >
	{
	protected:
		using ElementT = ObjT;
		using ElementKeyT = KeyT;
		using ElementObjectCacheT = ObjectCacheBaseT< ElementT, ElementKeyT, TraitsT >;
		using ElementCacheTraitsT = typename ElementObjectCacheT::ElementCacheTraitsT;
		using ElementPtrT = typename ElementObjectCacheT::ElementPtrT;
		using ElementContT = typename ElementObjectCacheT::ElementContT;
		using ElementProducerT = typename ElementObjectCacheT::ElementProducerT;
		using ElementInitialiserT = typename ElementObjectCacheT::ElementInitialiserT;
		using ElementCleanerT = typename ElementObjectCacheT::ElementCleanerT;
		using ElementMergerT = typename ElementObjectCacheT::ElementMergerT;
		using ElementAttacherT = typename ElementObjectCacheT::ElementAttacherT;
		using ElementDetacherT = typename ElementObjectCacheT::ElementDetacherT;

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine			The engine.
		 *\param[in]	scene			The scene.
		 *\param[in]	rootNode		The root node.
		 *\param[in]	rootCameraNode	The cameras root node.
		 *\param[in]	rootObjectNode	The objects root node.
		 *\param[in]	produce			The element producer.
		 *\param[in]	initialise		The element initialiser.
		 *\param[in]	clean			The element cleaner.
		 *\param[in]	merge			The element collection merger.
		 *\param[in]	attach			The element attacher (to a scene node).
		 *\param[in]	detach			The element detacher (from a scene node).
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine			Le moteur.
		 *\param[in]	scene			La scène.
		 *\param[in]	rootNode		Le noeud racine.
		 *\param[in]	rootCameraNode	Le noeud racine des caméras.
		 *\param[in]	rootObjectNode	Le noeud racine des objets.
		 *\param[in]	produce			Le créateur d'objet.
		 *\param[in]	initialise		L'initialiseur d'objet.
		 *\param[in]	clean			Le nettoyeur d'objet.
		 *\param[in]	merge			Le fusionneur de collection d'objets.
		 *\param[in]	attach			L'attacheur d'objet (à un noeud de scène).
		 *\param[in]	detach			Le détacheur d'objet (d'un noeud de scène).
		 */
		inline ObjectCacheT( Scene & scene
			, SceneNodeSPtr rootNode
			, SceneNodeSPtr rootCameraNode
			, SceneNodeSPtr rootObjectNode
			, ElementProducerT produce
			, ElementInitialiserT initialise = ElementInitialiserT{}
			, ElementCleanerT clean = ElementCleanerT{}
			, ElementMergerT merge = ElementMergerT{}
			, ElementAttacherT attach = ElementAttacherT{}
			, ElementDetacherT detach = ElementDetacherT{} )
			: ElementObjectCacheT{ scene
				, rootNode
				, rootCameraNode
				, rootObjectNode
				, std::move( produce )
				, std::move( initialise )
				, std::move( clean )
				, std::move( merge )
				, std::move( attach )
				, std::move( detach ) }
		{
		}
	};
	/**
	 *\~english
	 *\brief		Creates an object cache.
	 *\param[in]	parameters	The cache ctor parameters.
	 *\~french
	 *\brief		Crée un cache d'objets.
	 *\param[in]	parameters	Les paramètres de construction du cache.
	 */
	template< typename ObjT, typename KeyT, typename ... ParametersT >
	ObjectCachePtrT< ObjT, KeyT > makeObjectCache( ParametersT && ... parameters )
	{
		return std::make_shared< ObjectCacheT< ObjT, KeyT > >( std::forward< ParametersT >( parameters )... );
	}
}

#endif
