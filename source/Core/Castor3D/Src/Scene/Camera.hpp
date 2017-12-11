/*
See LICENSE file in root folder
*/
#ifndef ___C3D_CAMERA_H___
#define ___C3D_CAMERA_H___

#include "MovableObject.hpp"
#include "Miscellaneous/Frustum.hpp"
#include "Render/Viewport.hpp"

#include <Math/PlaneEquation.hpp>
#include <Design/OwnedBy.hpp>

namespace castor3d
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
	\remark		donne la position, orientation, viewport ...
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
			: public castor::TextWriter< Camera >
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor
			 *\~french
			 *\brief		Constructeur
			 */
			C3D_API explicit TextWriter( castor::String const & tabs );
			/**
			 *\~english
			 *\brief		Writes a camera into a text file
			 *\param[in]	file	The file to save the camera in
			 *\param[in]	camera	The cameras to save
			 *\~french
			 *\brief		Ecrit une caméra dans un fichier texte
			 *\param[in]	file	Le fichier
			 *\param[in]	camera	La camera
			 */
			C3D_API bool operator()( Camera const & camera
				, castor::TextFile & file )override;
		};

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	name		The camera name.
		 *\param[in]	scene		The parent scene.
		 *\param[in]	node		The parent scene node.
		 *\param[in]	viewport	Viewport to copy.
		 *\param[in]	invertX		Tells if the X axis is inverted.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	name		Le nom de la caméra.
		 *\param[in]	scene		La scène parente.
		 *\param[in]	node		Le noeud de scène parent.
		 *\param[in]	viewport	Viewport à copier.
		 *\param[in]	invertX		Dit si l'axe des X est inversé.
		 */
		C3D_API Camera( castor::String const & name
			, Scene & scene
			, SceneNodeSPtr const node
			, Viewport && viewport
			, bool invertX = false );
		/**
		 *\~english
		 *\brief		Constructor, needs the camera renderer, the name, window size and projection type. Creates a viewport renderer and a viewport.
		 *\param[in]	name		The camera name.
		 *\param[in]	scene		The parent scene.
		 *\param[in]	node		The parent scene node.
		 *\param[in]	invertX		Tells if the X axis is inverted.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	name		Le nom de la caméra.
		 *\param[in]	scene		La scène parente.
		 *\param[in]	node		SceneNode parent.
		 *\param[in]	invertX		Dit si l'axe des X est inversé.
		 */
		C3D_API Camera( castor::String const & name
			, Scene & scene
			, SceneNodeSPtr const node
			, bool invertX = false );
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
		 *\param[in]	node	The new light's parent node
		 *\~french
		 *\brief		Attache cette lumière au node donné
		 *\param[in]	node	Le nouveau node parent de cette lumière
		 */
		C3D_API void attachTo( SceneNodeSPtr node )override;
		/**
		 *\~english
		 *\brief		Updates the viewport, the frustum...
		 *\~french
		 *\brief		Met à jour le viewport, frustum...
		 */
		C3D_API void update();
		/**
		 *\~english
		 *\brief		Applies the viewport.
		 *\~french
		 *\brief		Applique le viewport.
		 */
		C3D_API void apply()const;
		/**
		 *\~english
		 *\brief		Resizes the viewport
		 *\param[in]	width, height	Display window size
		 *\~french
		 *\brief		Redimensionne le viewport
		 *\param[in]	width, height	Dimensions de la fenêtre d'affichage
		 */
		C3D_API void resize( uint32_t width, uint32_t height );
		/**
		 *\~english
		 *\brief		Resizes the viewport
		 *\param[in]	size	Display window size
		 *\~french
		 *\brief		Redimensionne le viewport
		 *\param[in]	size	Dimensions de la fenêtre d'affichage
		 */
		C3D_API void resize( castor::Size const & size );
		/**
		 *\~english
		 *\brief		Retrieves the viewport type
		 *\return		The viewport type
		 *\~french
		 *\brief		Récupère le type de viewport
		 *\return		Le type de viewport
		 */
		C3D_API ViewportType getViewportType()const;
		/**
		 *\~english
		 *\brief		sets the viewport type
		 *\param[in]	value	The viewport type
		 *\~french
		 *\brief		Définit le type de viewport
		 *\param[in]	value	Le type de viewport
		 */
		C3D_API void setViewportType( ViewportType value );
		/**
		 *\~english
		 *\brief		Retrieves the viewport width
		 *\return		The width
		 *\~french
		 *\brief		Récupère la largeur du viewport
		 *\return		La largeur
		 */
		C3D_API uint32_t getWidth()const;
		/**
		 *\~english
		 *\brief		Retrieves the viewport height
		 *\return		The height
		 *\~french
		 *\brief		Récupère la hauteur du viewport
		 *\return		La hauteur
		 */
		C3D_API uint32_t getHeight()const;
		/**
		 *\~english
		 *\brief		Checks if given BoundingBox is in the view frustum
		 *\param[in]	box				The BoundingBox
		 *\param[in]	transformations	The BoundingBox transformations matrix
		 *\return		\p false if the BoundingBox is completely out of the view frustum
		 *\~french
		 *\brief
		 *\brief		Vérifie si la BoundingBox donnée est dans le frustum de vue
		 *\param[in]	box				La BoundingBox
		 *\param[in]	transformations	La matrice de transformations de la BoundingBox
		 *\return		\p false si la BoundingBox est complètement en dehors du frustum de vue
		 */
		C3D_API bool isVisible( castor::BoundingBox const & box
			, castor::Matrix4x4r const & transformations )const;
		/**
		 *\~english
		 *\brief		Checks if given BoundingSphere is in the view frustum.
		 *\param[in]	sphere		The BoundingSphere.
		 *\param[in]	position	The position for the BoundingSphere.
		 *\param[in]	scale		The scale for the BoundingSphere.
		 *\return		\p false if the BoundingSphere is completely out of the view frustum.
		 *\~french
		 *\brief
		 *\brief		Vérifie si la SphereBox donnée est dans le frustum de vue.
		 *\param[in]	sphere		La BoundingSphere.
		 *\param[in]	position	La position de la BoundingSphere.
		 *\param[in]	scale		L'échelle de la SphereBox.
		 *\return		\p false si la BoundingSphere est complètement en dehors du frustum de vue.
		 */
		C3D_API bool isVisible( castor::BoundingSphere const & sphere
			, castor::Point3r const & position
			, castor::Point3r const & scale )const;
		/**
		 *\~english
		 *\brief		Checks if given point is in the view frustum
		 *\param[in]	point	The point
		 *\return		\p false if the point out of the view frustum
		 *\~french
		 *\brief
		 *\brief		Vérifie si le point donné est dans le frustum de vue
		 *\param[in]	point	Le point
		 *\return		\p false si le point en dehors du frustum de vue
		 */
		C3D_API bool isVisible( castor::Point3r const & point )const;
		/**
		 *\~english
		 *\brief		Retrieves the Viewport
		 *\return		The Viewport
		 *\~french
		 *\brief		Récupère le Viewport
		 *\return		Le Viewport
		 */
		inline Viewport const & getViewport()const
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
		inline Viewport & getViewport()
		{
			return m_viewport;
		}
		/**
		 *\~english
		 *\brief		Retrieves the view matrix
		 *\~french
		 *\brief		Récupère la matrice de vue
		 */
		inline castor::Matrix4x4r const & getView()const
		{
			return m_view;
		}
		/**
		 *\~english
		 *\brief		sets the view matrix.
		 *\param[in]	view	The new value.
		 *\~french
		 *\brief		Définit la matrice de vue.
		 *\param[in]	view	La nouvelle valeur.
		 */
		inline void setView( castor::Matrix4x4r const & view )
		{
			m_view = view;
		}

	private:
		void onNodeChanged( SceneNode const & node );

	public:
		//!\~english	The signal raised when the camera has changed.
		//!\~french		Le signal levé lorsque la caméra a changé.
		OnCameraChanged onChanged;

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
		castor::Matrix4x4r m_view;
		//!\~english	Tells if the X axis is inverted.
		//!\~french		Dit si l'axe des X est inversé.
		bool m_invertX;
		//!\~english	Tells if the parent node has changed.
		//!\~french		Dit si le noeud parent a changé.
		bool m_nodeChanged{ true };
	};
}

#endif
