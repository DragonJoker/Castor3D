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
#ifndef ___C3D_CAMERA_MANAGER_H___
#define ___C3D_CAMERA_MANAGER_H___

#include "Manager/ObjectManager.hpp"

#include "Scene/Camera.hpp"

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
	template<> struct CachedObjectNamer< Camera >
	{
		C3D_API static const Castor::String Name;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		29/01/2016
	\version	0.8.0
	\~english
	\brief		Helper structure to enable attaching if a type supports it.
	\remarks	Specialisation for Camera.
	\~french
	\brief		Structure permettant d'attacher les éléments qui le supportent.
	\remarks	Spécialisation pour Camera.
	*/
	template<>
	struct ElementAttacher< Camera >
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
		static void Attach( std::shared_ptr< Camera > p_element, SceneNodeSPtr p_parent, SceneNodeSPtr p_rootNode, SceneNodeSPtr p_rootCameraNode, SceneNodeSPtr p_rootObjectNode )
		{
			if ( p_parent )
			{
				p_parent->AttachObject( p_element );
			}
			else
			{
				p_rootCameraNode->AttachObject( p_element );
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
	struct ElementProducer< Camera, Castor::String, Scene, SceneNodeSPtr, Viewport >
	{
		using ElemPtr = std::shared_ptr< Camera >;

		ElemPtr operator()( Castor::String const & p_key, Scene & p_scene, SceneNodeSPtr p_parent, Viewport const & p_viewport )
		{
			return std::make_shared< Camera >( p_key, p_scene, p_parent, p_viewport );
		}
	};
	using CameraProducer = ElementProducer< Camera, Castor::String, Scene, SceneNodeSPtr, Viewport >;
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
	std::unique_ptr< ObjectCache< Camera, Castor::String, CameraProducer > >
	MakeObjectCache( SceneNodeSPtr p_rootNode, SceneNodeSPtr p_rootCameraNode , SceneNodeSPtr p_rootObjectNode, SceneGetter const & p_get, CameraProducer const & p_produce )
	{
		return std::make_unique< ObjectCache< Camera, Castor::String, CameraProducer > >( p_get, p_produce, p_rootNode, p_rootCameraNode, p_rootObjectNode);
	}
	/**
	 *\~english
	 *\brief		Bind the camera.
	 *\param[in]	p_sceneBuffer	The constants buffer.
	 *\~french
	 *\brief		Attache la caméra.
	 *\param[in]	p_sceneBuffer	Le tampon de constantes.
	 */
	C3D_API void BindCamera( ObjectCache< Camera, Castor::String, CameraProducer > const & p_cache, FrameVariableBuffer & p_sceneBuffer );
}

#endif
