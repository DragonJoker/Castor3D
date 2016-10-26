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
#ifndef ___C3D_CAMERA_H___
#define ___C3D_CAMERA_H___

#include "MovableObject.hpp"
#include "Miscellaneous/Frustum.hpp"
#include "Render/Viewport.hpp"

#include <Math/PlaneEquation.hpp>
#include <Design/OwnedBy.hpp>

namespace Castor3D
{
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
		 *\brief		Constructor.
		 *\remarks		Not to be used by the user, use Scene::CreateCamera instead
		 *\param[in]	p_name		The camera name
		 *\param[in]	p_scene		The parent scene
		 *\param[in]	p_node		The parent scene node
		 *\param[in]	p_viewport	Viewport to copy
		 *\~french
		 *\brief		Constructeur.
		 *\remarks		L'utilisateur ne devrait pas s'en servir, préférer l'utilisation de Scene::CreateCamera
		 *\param[in]	p_name		Le nom de la caméra
		 *\param[in]	p_scene		La scène parente
		 *\param[in]	p_node		Le noeud de scène parent
		 *\param[in]	p_viewport	Viewport à copier
		 */
		C3D_API Camera( Castor::String const & p_name, Scene & p_scene, const SceneNodeSPtr p_node, Viewport && p_viewport );
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
		C3D_API ~Camera();
		/**
		 *\~english
		 *\brief		Attaches this light to a Material
		 *\param[in]	p_node	The new light's parent node
		 *\~french
		 *\brief		Attache cette lumière au node donné
		 *\param[in]	p_node	Le nouveau node parent de cette lumière
		 */
		C3D_API void AttachTo( SceneNodeSPtr p_node )override;
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
		 *\brief		Updates the viewport, the frustum...
		 *\~french
		 *\brief		Met à jour le viewport, frustum...
		 */
		C3D_API void Update();
		/**
		 *\~english
		 *\brief		Applies the viewport.
		 *\~french
		 *\brief		Applique le viewport.
		 */
		C3D_API void Apply()const;
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
		 *\brief		Retrieves the viewport type
		 *\return		The viewport type
		 *\~french
		 *\brief		Récupère le type de viewport
		 *\return		Le type de viewport
		 */
		C3D_API ViewportType GetViewportType()const;
		/**
		 *\~english
		 *\brief		Sets the viewport type
		 *\param[in]	val	The viewport type
		 *\~french
		 *\brief		Définit le type de viewport
		 *\param[in]	val	Le type de viewport
		 */
		C3D_API void SetViewportType( ViewportType val );
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
		 *\brief		Checks if given SphereBox is in the view frustum
		 *\param[in]	p_box				The SphereBox
		 *\param[in]	m_transformations	The SphereBox transformations matrix
		 *\return		\p false if the SphereBox is completely out of the view frustum
		 *\~french
		 *\brief
		 *\brief		Vérifie si la SphereBox donnée est dans le frustum de vue
		 *\param[in]	p_box				La SphereBox
		 *\param[in]	m_transformations	La SphereBox de transformations de la CubeBox
		 *\return		\p false si la SphereBox est complètement en dehors du frustum de vue
		 */
		C3D_API bool IsVisible( Castor::SphereBox const & p_box, Castor::Matrix4x4r const & m_transformations )const;
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
		 *\brief		Puts the camera values into the given constants buffer.
		 *\param[in]	p_sceneBuffer	The constants buffer.
		 *\~french
		 *\brief		Met les valeurs de la caméra dans le tampon de constantes donné.
		 *\param[in]	p_sceneBuffer	Le tampon de constantes.
		 */
		C3D_API void FillShader( FrameVariableBuffer const & p_sceneBuffer )const;
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
		inline Castor::Matrix4x4r const & GetView()const
		{
			return m_view;
		}
		/**
		 *\~english
		 *\brief		Sets the view matrix.
		 *\param[in]	p_view	The new value.
		 *\~french
		 *\brief		Définit la matrice de vue.
		 *\param[in]	p_view	La nouvelle valeur.
		 */
		inline void SetView( Castor::Matrix4x4r const & p_view )
		{
			m_view = p_view;
		}
		/**
		 *\~english
		 *\brief		Connects a client to the changed notification signal.
		 *\param[in]	p_function	The client function.
		 *\return		The connection.
		 *\~french
		 *\brief		Connecte un client au signal de notification de changement.
		 *\param[in]	p_function	La fonction du client.
		 *\return		La connexion.
		 */
		inline uint32_t Connect( std::function< void( Camera const & ) > p_function )
		{
			return m_signalChanged.connect( p_function );
		}
		/**
		 *\~english
		 *\brief		Disconnects a client from the changed notification signal.
		 *\return		The connection.
		 *\~french
		 *\brief		Déconnecte un client du signal de notification de changement.
		 *\return		La connexion.
		 */
		inline void Disconnect( uint32_t p_connection )
		{
			m_signalChanged.disconnect( p_connection );
		}

	private:
		void OnNodeChanged( SceneNode const & p_node );

	private:
		friend class Scene;
		//!\~english	The viewport of the camera.
		//!\~french		Le viewport de la caméra.
		Viewport m_viewport;
		//!\~english	The view frustum.
		//!\~french		Le frustum de vue.
		Frustum m_frustum;
		//!\~english	The view matrix.
		//!\~french		La matrice vue.
		Castor::Matrix4x4r m_view;
		//!\~english	Tells if the parent node has changed.
		//!\~french		Dit si le noeud parent a changé.
		bool m_nodeChanged{ true };
		//!\~english	The signal raised when the camera has changed.
		//!\~french		Le signal levé lorsque la caméra a changé.
		Castor::Signal< std::function< void( Camera const & ) > > m_signalChanged;
	};
}

#endif
