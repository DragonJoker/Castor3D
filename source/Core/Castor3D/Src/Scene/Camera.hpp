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
#ifndef ___C3D_CAMERA_H___
#define ___C3D_CAMERA_H___

#include "MovableObject.hpp"
#include "Render/Viewport.hpp"

#include <PlaneEquation.hpp>
#include <OwnedBy.hpp>

namespace Castor3D
{
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
	/*!
	\author 	Sylvain DOREMUS
	\version	0.1
	\date		09/02/2010
	\~english
	\brief		Class which represents a Camera
	\remark		Gives its position, orientation, viewport ...
	\~french
	\brief		Classe de représentation de Camera
	\remark		Donne la position, orientation, viewport ...
	*/
	class Camera
		: public MovableObject
	{
	public:
		/*!
		\author		Sylvain DOREMUS
		\version	0.6.1.0
		\date		19/10/2011
		\~english
		\brief		Camera loader
		\~french
		\brief		Loader de Camera
		*/
		class TextWriter
			: public Castor::TextWriter< Camera >
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor
			 *\~french
			 *\brief		Constructeur
			 */
			C3D_API explicit TextWriter( Castor::String const & p_tabs );
			/**
			 *\~english
			 *\brief		Writes a camera into a text file
			 *\param[in]	p_file		The file to save the camera in
			 *\param[in]	p_camera	The cameras to save
			 *\~french
			 *\brief		Ecrit une caméra dans un fichier texte
			 *\param[in]	p_file		Le fichier
			 *\param[in]	p_camera	La camera
			 */
			C3D_API bool operator()( Camera const & p_camera, Castor::TextFile & p_file )override;
		};

	public:
		/**
		 *\~english
		 *\brief		Constructor, needs the camera renderer, the name, window size and projection type. Creates a viewport renderer and a viewport
		 *\remarks		Not to be used by the user, use Scene::CreateCamera instead
		 *\param[in]	p_name		The camera name
		 *\param[in]	p_scene		The parent scene
		 *\param[in]	p_node		The parent scene node
		 *\param[in]	p_viewport	Viewport to copy
		 *\~french
		 *\brief		Constructeur
		 *\remarks		L'utilisateur ne devrait pas s'en servir, préférer l'utilisation de Scene::CreateCamera
		 *\param[in]	p_name		Le nom de la caméra
		 *\param[in]	p_scene		La scène parente
		 *\param[in]	p_node		Le noeud de scène parent
		 *\param[in]	p_viewport	Viewport à copier
		 */
		C3D_API Camera( Castor::String const & p_name, Scene & p_scene, const SceneNodeSPtr p_node, Viewport const & p_viewport );
		/**
		 *\~english
		 *\brief		Constructor, needs the camera renderer, the name, window size and projection type. Creates a viewport renderer and a viewport
		 *\remarks		Not to be used by the user, use Scene::CreateCamera instead
		 *\param[in]	p_name	The camera name
		 *\param[in]	p_scene	The parent scene
		 *\param[in]	p_node	The parent scene node
		 *\~french
		 *\brief		Constructeur
		 *\remarks		L'utilisateur ne devrait pas s'en servir, préférer l'utilisation de Scene::CreateCamera
		 *\param[in]	p_name	Le nom de la caméra
		 *\param[in]	p_scene	La scène parente
		 *\param[in]	p_node	SceneNode parent
		 */
		C3D_API Camera( Castor::String const & p_name, Scene & p_scene, const SceneNodeSPtr p_node );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API virtual ~Camera();
		/**
		 *\~english
		 *\brief		Applies the viewport, the rotation ...
		 *\param[in]	p_pipeline	The render pipeline.
		 *\~french
		 *\brief		Applique le viewport, la rotation ...
		 *\param[in]	p_pipeline	Le pipeline de rendu
		 */
		C3D_API virtual void Render( Pipeline & p_pipeline );
		/**
		 *\~english
		 *\brief		Removes the transformations
		 *\~french
		 *\brief		Enlève les transformations
		 */
		C3D_API virtual void EndRender();
		/**
		 *\~english
		 *\brief		Resizes the viewport
		 *\param[in]	p_width, p_height	Display window size
		 *\~french
		 *\brief		Redimensionne le viewport
		 *\param[in]	p_width, p_height	Dimensions de la fenêtre d'affichage
		 */
		C3D_API void Resize( uint32_t p_width, uint32_t p_height );
		/**
		 *\~english
		 *\brief		Resizes the viewport
		 *\param[in]	p_size	Display window size
		 *\~french
		 *\brief		Redimensionne le viewport
		 *\param[in]	p_size	Dimensions de la fenêtre d'affichage
		 */
		C3D_API void Resize( Castor::Size const & p_size );
		/**
		 *\~english
		 *\brief		Sets the orientation to identity
		 *\~french
		 *\brief		Met l'orientation à l'identité
		 */
		C3D_API void ResetOrientation();
		/**
		 *\~english
		 *\brief		Sets the position to 0
		 *\~french
		 *\brief		Réinitialise la position
		 */
		C3D_API void ResetPosition();
		/**
		 *\~english
		 *\brief		Retrieves the viewport type
		 *\return		The viewport type
		 *\~french
		 *\brief		Récupère le type de viewport
		 *\return		Le type de viewport
		 */
		C3D_API eVIEWPORT_TYPE GetViewportType()const;
		/**
		 *\~english
		 *\brief		Sets the viewport type
		 *\param[in]	val	The viewport type
		 *\~french
		 *\brief		Définit le type de viewport
		 *\param[in]	val	Le type de viewport
		 */
		C3D_API void SetViewportType( eVIEWPORT_TYPE val );
		/**
		 *\~english
		 *\brief		Retrieves the viewport width
		 *\return		The width
		 *\~french
		 *\brief		Récupère la largeur du viewport
		 *\return		La largeur
		 */
		C3D_API uint32_t GetWidth()const;
		/**
		 *\~english
		 *\brief		Retrieves the viewport height
		 *\return		The height
		 *\~french
		 *\brief		Récupère la hauteur du viewport
		 *\return		La hauteur
		 */
		C3D_API uint32_t GetHeight()const;
		/**
		 *\~english
		 *\brief		Checks if given CubeBox is in the view frustum
		 *\param[in]	p_box				The CubeBox
		 *\param[in]	m_transformations	The CubeBox transformations matrix
		 *\return		\p false if the CubeBox is completely out of the view frustum
		 *\~french
		 *\brief
		 *\brief		Vérifie si la CubeBox donnée est dans le frustum de vue
		 *\param[in]	p_box				La CubeBox
		 *\param[in]	m_transformations	La matrice de transformations de la CubeBox
		 *\return		\p false si la CubeBox est complètement en dehors du frustum de vue
		 */
		C3D_API bool IsVisible( Castor::CubeBox const & p_box, Castor::Matrix4x4r const & m_transformations )const;
		/**
		 *\~english
		 *\brief		Checks if given point is in the view frustum
		 *\param[in]	p_point	The point
		 *\return		\p false if the point out of the view frustum
		 *\~french
		 *\brief
		 *\brief		Vérifie si le point donné est dans le frustum de vue
		 *\param[in]	p_point	Le point
		 *\return		\p false si le point en dehors du frustum de vue
		 */
		C3D_API bool IsVisible( Castor::Point3r const & p_point )const;
		/**
		 *\~english
		 *\brief		Retrieves the Viewport
		 *\return		The Viewport
		 *\~french
		 *\brief		Récupère le Viewport
		 *\return		Le Viewport
		 */
		inline Viewport const & GetViewport()const
		{
			return m_viewport;
		}
		/**
		 *\~english
		 *\brief		Retrieves the Viewport
		 *\return		The Viewport
		 *\~french
		 *\brief		Récupère le Viewport
		 *\return		Le Viewport
		 */
		inline Viewport & GetViewport()
		{
			return m_viewport;
		}
		/**
		 *\~english
		 *\brief		Retrieves the view matrix
		 *\~french
		 *\brief		Récupère la matrice de vue
		 */
		virtual Castor::Matrix4x4r const & GetView()const
		{
			return m_view;
		}

	private:
		friend class Scene;
		friend class CameraRenderer;
		//!\~english The viewport of the camera	\~french Le viewport de la caméra
		Viewport m_viewport;
		//!\~english The view frustum's planes	\~french Les plans du frustum de vue
		Castor::PlaneEquation< real > m_planes[eFRUSTUM_PLANE_COUNT];
		//!\~english The view matrix	\~french La matrice vue
		Castor::Matrix4x4r m_view;
	};
}

#endif
