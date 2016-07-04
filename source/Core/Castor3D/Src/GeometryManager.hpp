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
#ifndef ___C3D_GEOMETRY_MANAGER_H___
#define ___C3D_GEOMETRY_MANAGER_H___

#include "Manager/ObjectManager.hpp"

#include "Event/Frame/FunctorEvent.hpp"
#include "Scene/Geometry.hpp"

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
	template<>
	struct CachedObjectNamer< Geometry >
	{
		C3D_API static const Castor::String Name;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		13/10/2015
	\version	0.8.0
	\~english
	\brief		Helper structure to enable cleanup if a type supports it.
	\remarks	Specialisation for types that support cleanup.
	\~french
	\brief		Structure permettant de nettoyer les éléments qui le supportent.
	\remarks	Spécialisation pour les types qui supportent le cleanup.
	*/
	template<>
	struct ElementInitialiser< Geometry >
	{
		ElementInitialiser()
		{
		}

		void operator()( Engine & p_engine, Geometry & p_element )
		{
			p_engine.PostEvent( MakeFunctorEvent( eEVENT_TYPE_PRE_RENDER, [&p_element, this]()
			{
				p_element.CreateBuffers( m_facesCount, m_vertexCount );
			} ) );
		}

		uint32_t m_facesCount{ 0 };
		uint32_t m_vertexCount{ 0 };
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		29/01/2016
	\version	0.8.0
	\~english
	\brief		Helper structure to enable attaching if a type supports it.
	\remarks	Specialisation for Geometry.
	\~french
	\brief		Structure permettant d'attacher les éléments qui le supportent.
	\remarks	Spécialisation pour Geometry.
	*/
	template<>
	struct ElementAttacher< Geometry >
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
		static void Attach( std::shared_ptr< Geometry > p_element, SceneNodeSPtr p_parent, SceneNodeSPtr p_rootNode, SceneNodeSPtr p_rootCameraNode, SceneNodeSPtr p_rootObjectNode )
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
	struct ElementProducer< Geometry, Castor::String, Scene, SceneNodeSPtr, MeshSPtr >
	{
		using ElemPtr = std::shared_ptr< Geometry >;

		ElemPtr operator()( Castor::String const & p_key, Scene & p_scene, SceneNodeSPtr p_parent, MeshSPtr const & p_mesh )
		{
			return std::make_shared< Geometry >( p_key, p_scene, p_parent, p_mesh );
		}
	};
	using GeometryProducer = ElementProducer< Geometry, Castor::String, Scene, SceneNodeSPtr, MeshSPtr >;
	/**
	 *\~english
	 *\brief		Creates a Geometry cache.
	 *\param[in]	p_get		The engine getter.
	 *\param[in]	p_produce	The element producer.
	 *\~french
	 *\brief		Crée un cache de Geometry.
	 *\param[in]	p_get		Le récupérteur de moteur.
	 *\param[in]	p_produce	Le créateur d'objet.
	 */
	template<>
	std::unique_ptr< ObjectCache< Geometry, Castor::String, GeometryProducer > >
	MakeObjectCache( SceneNodeSPtr p_rootNode, SceneNodeSPtr p_rootCameraNode , SceneNodeSPtr p_rootObjectNode, SceneGetter const & p_get, GeometryProducer const & p_produce )
	{
		return std::make_unique< ObjectCache< Geometry, Castor::String, GeometryProducer > >( p_get, p_produce, p_rootNode, p_rootCameraNode, p_rootObjectNode );
	}
}

#endif
