/*
See LICENSE file in root folder
*/
#ifndef ___C3D_LIGHT_CACHE_H___
#define ___C3D_LIGHT_CACHE_H___

#include "Castor3D/Scene/Light/LightFactory.hpp"
#include "Castor3D/Cache/ObjectCache.hpp"
#include "Castor3D/Texture/TextureUnit.hpp"

#include <ashespp/Buffer/BufferView.hpp>

namespace castor3d
{
	using LightsRefArray = std::vector< Light * >;
	using LightsArray = std::vector< LightSPtr >;
	using LightsMap = std::array< LightsArray, size_t( LightType::eCount ) >;
	/*!
	\author 	Sylvain DOREMUS
	\date 		04/02/2016
	\version	0.8.0
	\~english
	\brief		Helper structure to specialise a scene cache behaviour.
	\remarks	Specialisation for Light.
	\~french
	\brief		Structure permettant de spécialiser le comportement d'un cache de scène.
	\remarks	Spécialisation pour Light.
	*/
	template< typename KeyType >
	struct ObjectCacheTraits< Light, KeyType >
	{
		C3D_API static const castor::String Name;
		using Producer = std::function< std::shared_ptr< Light >( KeyType const &, SceneNodeSPtr, LightType ) >;
		using Merger = std::function< void( ObjectCacheBase< Light, KeyType > const &
			, castor::Collection< Light, KeyType > &
			, std::shared_ptr< Light >
			, SceneNodeSPtr
			, SceneNodeSPtr ) >;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		29/01/2016
	\version	0.8.0
	\~english
	\brief		Light cache.
	\~french
	\brief		Cache de Light.
	*/
	template<>
	class ObjectCache< Light, castor::String >
		: public ObjectCacheBase< Light, castor::String >
	{
	public:
		using MyObjectCache = ObjectCacheBase< Light, castor::String >;
		using MyObjectCacheTraits = typename MyObjectCacheType::MyObjectCacheTraits;
		using Element = typename MyObjectCacheType::Element;
		using Key = typename MyObjectCacheType::Key;
		using Collection = typename MyObjectCacheType::Collection;
		using ElementPtr = typename MyObjectCacheType::ElementPtr;
		using Producer = typename MyObjectCacheType::Producer;
		using Initialiser = typename MyObjectCacheType::Initialiser;
		using Cleaner = typename MyObjectCacheType::Cleaner;
		using Merger = typename MyObjectCacheType::Merger;
		using Attacher = typename MyObjectCacheType::Attacher;
		using Detacher = typename MyObjectCacheType::Detacher;
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
		C3D_API ObjectCache( Engine & engine
			, Scene & scene
			, SceneNodeSPtr rootNode
			, SceneNodeSPtr rootCameraNode
			, SceneNodeSPtr rootObjectNode
			, Producer && produce
			, Initialiser && initialise = Initialiser{}
			, Cleaner && clean = Cleaner{}
			, Merger && merge = Merger{}
			, Attacher && attach = Attacher{}
			, Detacher && detach = Detacher{} );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~ObjectCache();
		/**
		 *\~english
		 *\brief		Initialises the lights texture.
		 *\~french
		 *\brief		Initialise la texture de lumières.
		 */
		C3D_API void initialise();
		/**
		 *\~english
		 *\brief		Sets all the elements to be cleaned up.
		 *\~french
		 *\brief		Met tous les éléments à nettoyer.
		 */
		C3D_API void cleanup();
		/**
		 *\~english
		 *\brief		adds an object.
		 *\param[in]	name	The object name.
		 *\param[in]	element	The object.
		 *\return		The added object, or the existing one.
		 *\~french
		 *\brief		Ajoute un objet.
		 *\param[in]	name	Le nom d'objet.
		 *\param[in]	element	L'objet.
		 *\return		L'objet ajouté, ou celui existant.
		 */
		C3D_API ElementPtr add( Key const & name
			, ElementPtr element );
		/**
		 *\~english
		 *\brief		Creates an object.
		 *\param[in]	name	The object name.
		 *\param[in]	parent	The parent scene node.
		 *\param[in]	type	The light source type.
		 *\return		The created object.
		 *\~french
		 *\brief		Crée un objet.
		 *\param[in]	name	Le nom d'objet.
		 *\param[in]	parent	Le noeud de scène parent.
		 *\param[in]	type	Le type de source lumineuse.
		 *\return		L'objet créé.
		 */
		C3D_API ElementPtr add( Key const & name
			, SceneNodeSPtr parent
			, LightType type );
		/**
		 *\~english
		 *\brief		Removes an object, given a name.
		 *\param[in]	name		The object name.
		 *\~french
		 *\brief		Retire un objet à partir d'un nom.
		 *\param[in]	name		Le nom d'objet.
		 */
		C3D_API void remove( Key const & name );
		/**
		 *\~english
		 *\brief		Updates the dirty lights.
		 *\~french
		 *\brief		Met à jour les sources lumineuses modifiées.
		 */
		C3D_API void update();
		/**
		 *\~english
		 *\brief		Updates the lights texture.
		 *\param[in]	camera	The camera used to tell if a light is applicable or not.
		 *\~french
		 *\brief		Met à jour la texture de sources lumineuses.
		 *\param[in]	camera	La caméra utilisée pour déterminer si une source lumineuse est applicable ou pas.
		 */
		C3D_API void updateLightsTexture( Camera const & camera )const;
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
		inline uint32_t getLightsCount( LightType type )const
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
		inline LightsArray getLights( LightType type )const
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
	using LightCache = ObjectCache< Light, castor::String >;
	CU_DeclareSmartPtr( LightCache );
}

#endif
