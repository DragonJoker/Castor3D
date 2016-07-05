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
	using LightsArray = std::vector< LightSPtr >;
	using LightsMap = std::map< eLIGHT_TYPE, LightsArray >;
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
	\date 		13/10/2015
	\version	0.8.0
	\~english
	\brief		Helper structure to enable initialisation if a type supports it.
	\remarks	Specialisation for Light.
	\~french
	\brief		Structure permettant d'initialiser les éléments qui le supportent.
	\remarks	Spécialisation pour Light.
	*/
	template<>
	struct ElementInitialiser< Light >
	{
		ElementInitialiser( LightsMap & p_typeSortedLights )
			: m_typeSortedLights{ p_typeSortedLights }
		{
		}

		inline void operator()( Engine & p_engine, LightSPtr p_element )
		{
			auto l_it = m_typeSortedLights.insert( { p_element->GetLightType(), LightsArray() } ).first;
			bool l_found = std::binary_search( l_it->second.begin(), l_it->second.end(), p_element );

			if ( !l_found )
			{
				l_it->second.push_back( p_element );
			}
		}

		LightsMap & m_typeSortedLights;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		13/10/2015
	\version	0.8.0
	\~english
	\brief		Helper structure to enable cleanup if a type supports it.
	\remarks	Specialisation for Light.
	\~french
	\brief		Structure permettant de nettoyer les éléments qui le supportent.
	\remarks	Spécialisation pour Light.
	*/
	template<>
	struct ElementCleaner< Light >
	{
		ElementCleaner( LightsMap & p_typeSortedLights )
			: m_typeSortedLights{ p_typeSortedLights }
		{
		}

		inline void operator()( Engine & p_engine, LightSPtr p_element )
		{
			auto l_itMap = m_typeSortedLights.find( p_element->GetLightType() );

			if ( l_itMap != m_typeSortedLights.end() )
			{
				auto l_it = std::find( l_itMap->second.begin(), l_itMap->second.end(), p_element );

				if ( l_it != l_itMap->second.end() )
				{
					l_itMap->second.erase( l_it );
				}
			}
		}

		LightsMap & m_typeSortedLights;
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
