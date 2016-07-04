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

#include "Cache/ObjectCache.hpp"

#include "Scene/Light/Light.hpp"
#include "Scene/Light/LightFactory.hpp"

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		04/02/2016
	\version	0.8.0
	\~english
	\brief		Helper structure to get an object type name.
	\~french
	\brief		Structure permettant de récupérer le nom du type d'un objet.
	*/
	template<> struct CachedObjectNamer< Light >
	{
		C3D_API static const Castor::String Name;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		29/01/2016
	\version	0.8.0
	\~english
	\brief		Helper structure to enable attaching if a type supports it.
	\remarks	Specialisation for Light.
	\~french
	\brief		Structure permettant d'attacher les éléments qui le supportent.
	\remarks	Spécialisation pour Light.
	*/
	template<>
	struct ElementAttacher< Light >
	{
		/**
		 *\~english
		 *\brief		Attaches an element to the appropriate parent node.
		 *\param[in]	p_element			The scene node.
		 *\param[in]	p_parent			The parent scene node.
		 *\param[in]	p_rootNode			The root node.
		 *\param[in]	p_rootCameraNode	The cameras root node.
		 *\param[in]	p_rootObjectNode	The objects root node.
		 *\~french
		 *\brief		Attache un élément au parent approprié.
		 *\param[in]	p_element			Le noeud de scène.
		 *\param[in]	p_parent			Le noeud de scène parent.
		 *\param[in]	p_rootNode			Le noeud racine.
		 *\param[in]	p_rootCameraNode	Le noeud racine des caméras.
		 *\param[in]	p_rootObjectNode	Le noeud racine des objets.
		 */
		inline void operator()( std::shared_ptr< Light > p_element, SceneNodeSPtr p_parent, SceneNodeSPtr p_rootNode, SceneNodeSPtr p_rootCameraNode, SceneNodeSPtr p_rootObjectNode )
		{
			if ( p_parent )
			{
				p_parent->AttachObject( p_element );
			}
			else
			{
				p_rootObjectNode->AttachObject( p_element );
			}
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		04/07/2016
	\version	0.9.0
	\~english
	\brief		Helper structure to create an element.
	\~french
	\brief		Structure permettant de créer un élément.
	*/
	template<>
	struct ElementProducer< Light, Castor::String, Scene, SceneNodeSPtr, eLIGHT_TYPE >
	{
		using ElemPtr = std::shared_ptr< Light >;

		inline ElementProducer()
		{
		}

		inline ElemPtr operator()( Castor::String const & p_name, Scene & p_scene, SceneNodeSPtr p_node, eLIGHT_TYPE p_lightType )
		{
			return std::make_shared< Light >( p_name, p_scene, p_node, m_lightFactory, p_lightType );
		}
		//!\~english	The lights factory.
		//!\~french		La fabrique de lumières.
		LightFactory m_lightFactory;
	};
	using LightProducer = ElementProducer< Light, Castor::String, Scene, SceneNodeSPtr, eLIGHT_TYPE >;
	/*!
	\author 	Sylvain DOREMUS
	\date 		29/01/2016
	\version	0.8.0
	\~english
	\brief		Light cache.
	\~french
	\brief		Cache de Light.
	*/
	class LightCache
		: public ObjectCache< Light, Castor::String, LightProducer >
	{
		friend ElementAttacher< Light >;
		using MyObjectCache = ObjectCache< Light, Castor::String, LightProducer >;
		using Producer = LightProducer;
		using Initialiser = typename MyObjectCache::Initialiser;
		using Cleaner = typename MyObjectCache::Cleaner;
		using Attacher = typename MyObjectCache::Attacher;
		using Detacher = typename MyObjectCache::Detacher;
		using Merger = typename MyObjectCache::Merger;

	public:
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
		C3D_API LightCache( SceneNodeSPtr p_rootNode
							, SceneNodeSPtr p_rootCameraNode
							, SceneNodeSPtr p_rootObjectNode
							, SceneGetter && p_get
							, Producer && p_produce
							, Initialiser && p_initialise = Initialiser{}
							, Cleaner && p_clean = Cleaner{}
							, Attacher && p_attach = Attacher{}
							, Detacher && p_detach = Detacher{}
							, Merger && p_merge = Merger{} );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~LightCache();
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
		 *\brief		Creates a light.
		 *\param[in]	p_name		The object name.
		 *\param[in]	p_parent	The parent scene node.
		 *\param[in]	p_lightType	The light source type.
		 *\return		The created object.
		 *\~french
		 *\brief		Crée une source lumineuse.
		 *\param[in]	p_name		Le nom d'objet.
		 *\param[in]	p_parent	Le noeud de scène parent.
		 *\param[in]	p_lightType	Le type de source lumineuse.
		 *\return		L'objet créé.
		 */
		C3D_API LightSPtr Add( Castor::String const & p_name, SceneNodeSPtr p_parent, eLIGHT_TYPE p_lightType );
		/**
		 *\~english
		 *\brief		Removes an object, given a name.
		 *\param[in]	p_name		The object name.
		 *\~french
		 *\brief		Retire un objet à partir d'un nom.
		 *\param[in]	p_name		Le nom d'objet.
		 */
		C3D_API void Remove( Castor::String const & p_name );
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
		C3D_API void DoAddLight( LightSPtr p_light );

	private:
		using LightsArray = std::vector< LightSPtr >;
		using LightsMap = std::map< eLIGHT_TYPE, LightsArray >;

	private:
		//!\~english The lights sorted byt light type	\~french Les lumières, triées par type de lumière.
		LightsMap m_typeSortedLights;
		//!\~english The lights texture	\~french La texture contenant les lumières
		TextureUnitSPtr m_lightsTexture;
	};
	/**
	 *\~english
	 *\brief		Creates a Light cache.
	 *\param[in]	p_rootNode			The root node.
	 *\param[in]	p_rootCameraNode	The cameras root node.
	 *\param[in]	p_rootObjectNode	The objects root node.
	 *\param[in]	p_get				The engine getter.
	 *\param[in]	p_produce			The element producer.
	 *\~french
	 *\brief		Crée un cache de Light.
	 *\param[in]	p_rootNode			Le noeud racine.
	 *\param[in]	p_rootCameraNode	Le noeud racine des caméras.
	 *\param[in]	p_rootObjectNode	Le noeud racine des objets.
	 *\param[in]	p_get				Le récupérteur de moteur.
	 *\param[in]	p_produce			Le créateur d'objet.
	 */
	inline std::unique_ptr< LightCache >
	MakeObjectCache( SceneNodeSPtr p_rootNode, SceneNodeSPtr p_rootCameraNode , SceneNodeSPtr p_rootObjectNode, SceneGetter && p_get, LightProducer && p_produce )
	{
		return std::make_unique< LightCache >( p_rootNode, p_rootCameraNode, p_rootObjectNode, std::move( p_get ), std::move( p_produce ) );
	}
}

#endif
