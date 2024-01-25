/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SceneNodeCache_H___
#define ___C3D_SceneNodeCache_H___

#include "Castor3D/Cache/ObjectCache.hpp"
#include "Castor3D/Scene/SceneModule.hpp"

namespace castor3d
{
	/**
	\~english
	\brief		SceneNode cache.
	\~french
	\brief		Cache de SceneNode.
	*/
	template<>
	class ObjectCacheT< SceneNode, castor::String, SceneNodeCacheTraits >
		: public ObjectCacheBaseT< SceneNode, castor::String, SceneNodeCacheTraits >
	{
	public:
		using ElementT = SceneNode;
		using ElementKeyT = castor::String;
		using ElementCacheTraitsT = SceneNodeCacheTraits;
		using ElementObjectCacheT = ObjectCacheBaseT< ElementT, ElementKeyT, ElementCacheTraitsT >;
		using ElementPtrT = typename ElementObjectCacheT::ElementPtrT;
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
		 *\param[in]	initialise		The element initialiser.
		 *\param[in]	clean			The element cleaner.
		 *\param[in]	merge			The element collection merger.
		 *\param[in]	attach			The element attacher (to a scene node).
		 *\param[in]	detach			The element detacher (from a scene node).
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	scene			La scène.
		 *\param[in]	initialise		L'initialiseur d'objet.
		 *\param[in]	clean			Le nettoyeur d'objet.
		 *\param[in]	merge			Le fusionneur de collection d'objets.
		 *\param[in]	attach			L'attacheur d'objet (à un noeud de scène).
		 *\param[in]	detach			Le détacheur d'objet (d'un noeud de scène).
		 */
		C3D_API ObjectCacheT( Scene & scene
			, ElementInitialiserT initialise
			, ElementCleanerT clean
			, ElementMergerT merge
			, ElementAttacherT attach
			, ElementDetacherT detach );
		/**
		 *\~english
		 *\brief			Logging version of tryAdd.
		 *\param[in]		name		The resource name.
		 *\param[in,out]	element		The resource, will be emptied on add (the cache now owns it).
		 *\param[in]		initialise	\p true to initialise the added element (no effect on duplicates).
		 *\return			The real element (added or duplicate original ).
		 *\~french
		 *\brief			Version journalisante de tryAdd.
		 *\param[in]		name		Le nom de la ressource.
		 *\param[in,out]	element		La ressource, vidée en cas d'ajout (le cache en prend la responsabilité).
		 *\param[in]		initialise	\p true pour initialiser l'élément ajouté (aucun effect sur les doublons).
		 *\return			L'élément réel (ajouté, ou original du doublon).
		 */
		C3D_API ElementObsT add( ElementKeyT const & name
			, ElementPtrT & element
			, bool initialise = true );
		/**
		 *\~english
		 *\brief		Logging version of tryAdd.
		 *\param[in]	name	The element name.
		 *\return		The real element (added or duplicate original ).
		 *\~french
		 *\brief		Version journalisante de tryAdd.
		 *\param[in]	name	Le nom d'élément.
		 *\return		L'élément réel (ajouté, ou original du doublon).
		 */
		C3D_API ElementObsT add( ElementKeyT const & name );

	private:
		void doReparentNode( SceneNode & node );

		using ElementObjectCacheT::tryAdd;
		using ElementObjectCacheT::tryAddNoLock;
		using ElementObjectCacheT::add;
		using ElementObjectCacheT::addNoLock;

	private:
		castor::Vector< SceneNode * > m_linearNodes;
		castor::Map< SceneNode * , OnSceneNodeReparentConnection > m_connections;
	};
}

#endif
