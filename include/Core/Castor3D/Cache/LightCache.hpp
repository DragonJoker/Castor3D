/*
See LICENSE file in root folder
*/
#ifndef ___C3D_LightCache_H___
#define ___C3D_LightCache_H___

#include "Castor3D/Cache/ObjectCache.hpp"
#include "Castor3D/Scene/Light/LightModule.hpp"

#include <ashespp/Buffer/Buffer.hpp>
#include <ashespp/Buffer/BufferView.hpp>

namespace castor3d
{
	using LightCacheTraits = ObjectCacheTraitsT< Light, castor::String >;
	/**
	\~english
	\brief		Light cache.
	\~french
	\brief		Cache de Light.
	*/
	template<>
	class ObjectCacheT< Light, castor::String, LightCacheTraits >
		: public ObjectCacheBaseT< Light, castor::String, LightCacheTraits >
	{
	public:
		using ElementT = Light;
		using ElementKeyT = castor::String;
		using ElementCacheTraitsT = LightCacheTraits;
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
		/**
		 *\~english
		 *\brief		Sets all the elements to be cleaned up.
		 *\~french
		 *\brief		Met tous les éléments à nettoyer.
		 */
		C3D_API void cleanup();
		/**
		 *\~english
		 *\brief			Updates the render pass, CPU wise.
		 *\param[in, out]	updater	The update data.
		 *\~french
		 *\brief			Met à jour la passe de rendu, au niveau CPU.
		 *\param[in, out]	updater	Les données d'update.
		 */
		C3D_API void update( CpuUpdater & updater );
		/**
		 *\~english
		 *\brief			Updates the render pass, GPU wise.
		 *\param[in, out]	updater	The update data.
		 *\~french
		 *\brief			Met à jour la passe de rendu, au niveau GPU.
		 *\param[in, out]	updater	Les données d'update.
		 */
		C3D_API void update( GpuUpdater & updater );
		C3D_API ashes::WriteDescriptorSet getDescriptorWrite( uint32_t binding )const;
		/**
		 *\~english
		 *\return		The texture buffer.
		 *\~french
		 *\return		Le tampon de la texture.
		 */
		C3D_API ashes::Buffer< castor::Point4f > const & getBuffer()const
		{
			return *m_textureBuffer;
		}
		/**
		 *\~english
		 *\return		The texture view.
		 *\~french
		 *\return		La vue de la texture.
		 */
		C3D_API ashes::BufferView const & getView()const
		{
			return *m_textureView;
		}
		/**
		 *\~english
		 *\brief		Retrieves the count of the lights of given type.
		 *\param[in]	type	The light type.
		 *\return		The count.
		 *\~french
		 *\brief		Récupère le nombre de lumières du type donné.
		 *\param[in]	type	Le type de lumière.
		 *\return		Le compte.
		 */
		uint32_t getLightsCount( LightType type )const
		{
			return uint32_t( getLights( type ).size() );
		}
		/**
		 *\~english
		 *\brief		Retrieves the lights of given type.
		 *\param[in]	type	The light type.
		 *\return		The lights.
		 *\~french
		 *\brief		Récupère les lumières du type donné.
		 *\param[in]	type	Le type de lumière.
		 *\return		Les lumières.
		 */
		LightsArray getLights( LightType type )const
		{
			return m_typeSortedLights[size_t( type )];
		}

	private:
		void onLightChanged( Light & light );
		bool doCheckUniqueDirectionalLight( LightType toAdd );

	private:
		LightsMap m_typeSortedLights;
		mutable castor::Point4fArray m_lightsBuffer;
		ashes::BufferPtr< castor::Point4f > m_textureBuffer;
		ashes::BufferViewPtr m_textureView;
		LightsRefArray m_dirtyLights;
		std::map< Light *, OnLightChangedConnection > m_connections;
	};
}

#endif
