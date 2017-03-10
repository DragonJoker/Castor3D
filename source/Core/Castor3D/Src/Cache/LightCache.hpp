/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef ___C3D_LIGHT_CACHE_H___
#define ___C3D_LIGHT_CACHE_H___

#include "Scene/Light/LightFactory.hpp"
#include "Cache/ObjectCache.hpp"

namespace Castor3D
{
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
		C3D_API static const Castor::String Name;
		using Producer = std::function< std::shared_ptr< Light >( KeyType const &, SceneNodeSPtr, LightType ) >;
		using Merger = std::function< void( ObjectCacheBase< Light, KeyType > const &
											, Castor::Collection< Light, KeyType > &
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
	class ObjectCache< Light, Castor::String >
		: public ObjectCacheBase< Light, Castor::String >
	{
	public:
		using MyObjectCache = ObjectCacheBase< Light, Castor::String >;
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
		 *\param[in]	p_rootNode			The root node.
		 *\param[in]	p_rootCameraNode	The cameras root node.
		 *\param[in]	p_rootObjectNode	The objects root node.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_rootNode			Le noeud racine.
		 *\param[in]	p_rootCameraNode	Le noeud racine des caméras.
		 *\param[in]	p_rootObjectNode	Le noeud racine des objets.
		 */
		C3D_API ObjectCache( Engine & p_engine
							 , Scene & p_scene
							 , SceneNodeSPtr p_rootNode
							 , SceneNodeSPtr p_rootCameraNode
							 , SceneNodeSPtr p_rootObjectNode
							 , Producer && p_produce
							 , Initialiser && p_initialise = Initialiser{}
							 , Cleaner && p_clean = Cleaner{}
							 , Merger && p_merge = Merger{}
							 , Attacher && p_attach = Attacher{}
							 , Detacher && p_detach = Detacher{} );
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
		C3D_API void Initialise();
		/**
		 *\~english
		 *\brief		Sets all the elements to be cleaned up.
		 *\~french
		 *\brief		Met tous les éléments à nettoyer.
		 */
		C3D_API void Cleanup();
		/**
		 *\~english
		 *\brief		Creates an object.
		 *\param[in]	p_name		The object name.
		 *\param[in]	p_parent	The parent scene node.
		 *\param[in]	p_params	The other constructor parameters.
		 *\return		The created object.
		 *\~french
		 *\brief		Crée un objet.
		 *\param[in]	p_name		Le nom d'objet.
		 *\param[in]	p_parent	Le noeud de scène parent.
		 *\param[in]	p_params	Les autres paramètres de construction.
		 *\return		L'objet créé.
		 */
		inline ElementPtr Add( Key const & p_name, SceneNodeSPtr p_parent, LightType p_type )
		{
			return MyObjectCache::Add( p_name, p_parent, p_type );
		}
		/**
		 *\~english
		 *\brief		Updates the lights texture.
		 *\~french
		 *\brief		Met à jour la texture de sources lumineuses.
		 */
		C3D_API void UpdateLights()const;
		/**
		 *\~english
		 *\brief		Binds the lights texture.
		 *\~french
		 *\brief		Active la texture de sources lumineuses.
		 */
		C3D_API void BindLights()const;
		/**
		 *\~english
		 *\brief		Unbinds the lights texture.
		 *\~french
		 *\brief		Désactive la texture de sources lumineuses.
		 */
		C3D_API void UnbindLights()const;
		/**
		 *\~english
		 *\brief		Retrieves the count of the lights of given type.
		 *\param[in]	p_type	The light type.
		 *\return		The count.
		 *\~french
		 *\brief		Récupère le nombre de lumières du type donné.
		 *\param[in]	p_type	Le type de lumière.
		 *\return		Le compte.
		 */
		inline uint32_t GetLightsCount( LightType p_type )const
		{
			return uint32_t( GetLights( p_type ).size() );
		}
		/**
		 *\~english
		 *\brief		Retrieves the lights of given type.
		 *\param[in]	p_type	The light type.
		 *\return		The lights.
		 *\~french
		 *\brief		Récupère les lumières du type donné.
		 *\param[in]	p_type	Le type de lumière.
		 *\return		Les lumières.
		 */
		inline LightsArray GetLights( LightType p_type )const
		{
			return m_typeSortedLights[size_t( p_type )];
		}

	private:
		//!\~english The lights sorted byt light type	\~french Les lumières, triées par type de lumière.
		LightsMap m_typeSortedLights;
		//!\~english The lights texture	\~french La texture contenant les lumières
		TextureUnitSPtr m_lightsTexture;
	};
	using LightCache = ObjectCache< Light, Castor::String >;
	DECLARE_SMART_PTR( LightCache );
}

#endif
