/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ObjectCacheBase_H___
#define ___C3D_ObjectCacheBase_H___

#include "CacheModule.hpp"
#include "Castor3D/Scene/SceneModule.hpp"

#include <CastorUtils/Design/OwnedBy.hpp>
#include <CastorUtils/Design/ResourceCacheBase.hpp>

namespace castor3d
{
	template< typename ObjT, typename KeyT, typename TraitsT >
	class ObjectCacheBaseT
		: public castor::OwnedBy< Scene >
		, public castor::ResourceCacheBaseT< ObjT, KeyT, TraitsT >
	{
	protected:
		using ElementT = ObjT;
		using ElementKeyT = KeyT;
		using ElementCacheTraitsT = TraitsT;
		using ElementCacheT = castor::ResourceCacheBaseT< ElementT, ElementKeyT, ElementCacheTraitsT >;
		using ElementCacheBaseT = ElementCacheT;
		using ElementObjectCacheT = ObjectCacheBaseT< ElementT, ElementKeyT, ElementCacheTraitsT >;
		using ElementPtrT = ObjectPtrT< ElementT, ElementKeyT >;
		using ElementObsT = ObjectObsT< ElementT, ElementKeyT >;
		using ElementContT = ObjectContT< ElementT, ElementKeyT >;
		using ElementInitialiserT = ObjectInitialiserT< ElementT, ElementKeyT >;
		using ElementCleanerT = ObjectCleanerT< ElementT, ElementKeyT >;
		using ElementMergerT = ObjectMergerT< ElementT, ElementKeyT >;
		using ElementAttacherT = ObjectAttacherT< ElementT, ElementKeyT >;
		using ElementDetacherT = ObjectDetacherT< ElementT, ElementKeyT >;

	public:
		using OnChangedFunction = std::function< void() >;
		using OnChanged = castor::SignalT< OnChangedFunction >;

	protected:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine			The engine.
		 *\param[in]	scene			The scene.
		 *\param[in]	rootNode		The root node.
		 *\param[in]	rootCameraNode	The cameras root node.
		 *\param[in]	rootObjectNode	The objects root node.
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
		 *\param[in]	initialise		L'initialiseur d'objet.
		 *\param[in]	clean			Le nettoyeur d'objet.
		 *\param[in]	merge			Le fusionneur de collection d'objets.
		 *\param[in]	attach			L'attacheur d'objet (à un noeud de scène).
		 *\param[in]	detach			Le détacheur d'objet (d'un noeud de scène).
		 */
		inline ObjectCacheBaseT( Scene & scene
			, SceneNodeSPtr rootNode
			, SceneNodeSPtr rootCameraNode
			, SceneNodeSPtr rootObjectNode
			, ElementInitialiserT initialise = ElementInitialiserT{}
			, ElementCleanerT clean = ElementCleanerT{}
			, ElementMergerT merge = ElementMergerT{}
			, ElementAttacherT attach = ElementAttacherT{}
			, ElementDetacherT detach = ElementDetacherT{} )
			: castor::OwnedBy< Scene >{ scene }
			, ElementCacheBaseT{ castor3d::getLogger( scene )
				, [this, initialise]( ElementT & element )
				{
					if ( initialise )
					{
						initialise( element );
					}

					m_attach( element
						, *element.getParent()
						, m_rootNode.lock()
						, m_rootCameraNode.lock()
						, m_rootObjectNode.lock() );
					onChanged();
				}
				, [this, clean]( ElementT & element )
				{
					m_detach( element );

					if ( clean )
					{
						clean( element );
					}

					onChanged();
				} }
			, m_engine{ castor3d::getEngine( scene ) }
			, m_rootNode{ rootNode }
			, m_rootCameraNode{ rootCameraNode }
			, m_rootObjectNode{ rootObjectNode }
			, m_merge{ std::move( merge ) }
			, m_attach{ std::move( attach ) }
			, m_detach{ std::move( detach ) }
		{
		}
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		inline ~ObjectCacheBaseT()
		{
		}

	public:
		/**
		 *\~english
		 *\return		Merges this cache's elements to the one given.
		 *\param[out]	destination		The destination cache.
		 *\~french
		 *\return		Met les éléments de ce cache dans ceux de celui donné.
		 *\param[out]	destination		Le cache de destination.
		 */
		inline void mergeInto( ElementObjectCacheT & destination )
		{
			auto lock( castor::makeUniqueLock( *this ) );
			auto lockOther( castor::makeUniqueLock( destination ) );

			if ( m_merge )
			{
				for ( auto it : *this )
				{
					m_merge( *this
						, destination.m_resources
						, it.second
						, destination.m_rootCameraNode.lock()
						, destination.m_rootObjectNode.lock() );
				}
			}

			this->doClearNoLock();
			onChanged();
		}
		/**
		*\~english
		*\return		The Engine.
		*\~french
		*\return		L'Engine.
		*/
		inline Engine * getEngine()const
		{
			return &m_engine;
		}
		/**
		*\~english
		*\return		The Engine.
		*\~french
		*\return		L'Engine.
		*/
		inline castor::String const & getObjectTypeName()const
		{
			return ElementCacheTraitsT::Name;
		}

	private:
		using ElementCacheBaseT::mergeInto;

	public:
		//!\~english	The signal emitted when the content has changed.
		//!\~french		Le signal émis lorsque le contenu a changé.
		OnChanged onChanged;

	protected:
		//!\~english	The engine.
		//!\~french		Le moteur.
		Engine & m_engine;
		//!\~english	The root node.
		//!\~french		Le noeud père de tous les noeuds de la scène.
		SceneNodeWPtr m_rootNode;
		//!\~english	The root node used only for cameras.
		//!\~french		Le noeud père de tous les noeuds de caméra.
		SceneNodeWPtr m_rootCameraNode;
		//!\~english	The root node for every object other than camera.
		//!\~french		Le noeud père de tous les noeuds d'objet.
		SceneNodeWPtr m_rootObjectNode;
		//!\~english	The object attacher.
		//!\~french		L'attacheur d'objet.
		ElementMergerT m_merge;
		//!\~english	The object attacher.
		//!\~french		L'attacheur d'objet.
		ElementAttacherT m_attach;
		//!\~english	The object detacher.
		//!\~french		Le détacheur d'objet.
		ElementDetacherT m_detach;

		friend class Scene;
	};
}

#endif
