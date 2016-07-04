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
#ifndef ___C3D_BILLBOARD_MANAGER_H___
#define ___C3D_BILLBOARD_MANAGER_H___

#include "Manager/ObjectManager.hpp"

#include "Scene/BillboardList.hpp"

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
	template<> struct CachedObjectNamer< BillboardList >
	{
		C3D_API static const Castor::String Name;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		29/01/2016
	\version	0.8.0
	\~english
	\brief		Helper structure to enable attaching if a type supports it.
	\remarks	Specialisation for BillboardList.
	\~french
	\brief		Structure permettant d'attacher les éléments qui le supportent.
	\remarks	Spécialisation pour BillboardList.
	*/
	template<>
	struct ElementAttacher< BillboardList >
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
		static void Attach( std::shared_ptr< BillboardList > p_element, SceneNodeSPtr p_parent, SceneNodeSPtr p_rootNode, SceneNodeSPtr p_rootCameraNode, SceneNodeSPtr p_rootObjectNode )
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
	struct ElementProducer< BillboardList, Castor::String, Scene, SceneNodeSPtr, RenderSystem >
	{
		using ElemPtr = std::shared_ptr< BillboardList >;

		ElemPtr operator()( Castor::String const & p_key, Scene & p_scene, SceneNodeSPtr p_parent, RenderSystem & p_renderSystem )
		{
			return std::make_shared< BillboardList >( p_key, p_scene, p_parent, p_renderSystem );
		}
	};
	using BillboardProducer = ElementProducer< BillboardList, Castor::String, Scene, SceneNodeSPtr, RenderSystem >;
	/**
	 *\~english
	 *\brief		Creates a BillboardList cache.
	 *\param[in]	p_get		The engine getter.
	 *\param[in]	p_produce	The element producer.
	 *\~french
	 *\brief		Crée un cache de BillboardList.
	 *\param[in]	p_get		Le récupérteur de moteur.
	 *\param[in]	p_produce	Le créateur d'objet.
	 */
	template<>
	std::unique_ptr< ObjectCache< BillboardList, Castor::String, BillboardProducer > >
	MakeObjectCache( SceneNodeSPtr p_rootNode, SceneNodeSPtr p_rootCameraNode , SceneNodeSPtr p_rootObjectNode, SceneGetter const & p_get, BillboardProducer const & p_produce )
	{
		return std::make_unique< ObjectCache< BillboardList, Castor::String, BillboardProducer > >( p_get, p_produce, p_rootNode, p_rootCameraNode, p_rootObjectNode);
	}
}

#endif
