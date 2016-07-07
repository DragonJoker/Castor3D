/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___C3D_LIGHT_CACHE_H___
#define ___C3D_LIGHT_CACHE_H___

#include "Scene/Light/LightFactory.hpp"
#include "Cache/ObjectCache.hpp"

namespace Castor3D
{
	using LightsArray = std::vector< LightSPtr >;
	using LightsMap = std::map< eLIGHT_TYPE, LightsArray >;
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
		using Producer = std::function< std::shared_ptr< Light >( KeyType const &, SceneNodeSPtr, eLIGHT_TYPE ) >;
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
		 *\brief		Binds the scene lights.
		 *\param[in]	p_program		The shader program.
		 *\param[in]	p_sceneBuffer	The constants buffer.
		 *\~french
		 *\brief		Attache les sources lumineuses
		 *\param[in]	p_program		Le programme shader.
		 *\param[in]	p_sceneBuffer	Le tampon de constantes.
		 */
		C3D_API void BindLights( ShaderProgram & p_program, FrameVariableBuffer & p_sceneBuffer );
		/**
		 *\~english
		 *\brief		Unbinds the scene lights.
		 *\param[in]	p_program		The shader program.
		 *\param[in]	p_sceneBuffer	The constants buffer.
		 *\~french
		 *\brief		Détache les sources lumineuses
		 *\param[in]	p_program		Le programme shader.
		 *\param[in]	p_sceneBuffer	Le tampon de constantes.
		 */
		C3D_API void UnbindLights( ShaderProgram & p_program, FrameVariableBuffer & p_sceneBuffer );

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
