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
		 *\param[in]	ownProjMtx	Tells if the projection matrix is held by the viewport \p false, or not \p true.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	name		Le nom de la caméra.
		 *\param[in]	scene		La scène parente.
		 *\param[in]	node		Le noeud de scène parent.
		 *\param[in]	viewport	Viewport à copier.
		 *\param[in]	ownProjMtx	Dit si la matrice de projection est définie par le viewport \p false, ou pas \p true.
		 */
		C3D_API Camera( castor::String const & name
			, Scene & scene
			, SceneNodeSPtr const node
			, Viewport && viewport
			, bool ownProjMtx = false );
		/**
		 *\~english
		 *\brief		Constructor, needs the camera renderer, the name, window size and projection type. Creates a viewport renderer and a viewport.
		 *\param[in]	name		The camera name.
		 *\param[in]	scene		The parent scene.
		 *\param[in]	node		The parent scene node.
		 *\param[in]	ownProjMtx	Tells if the projection matrix is held by the viewport \p false, or not \p true.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	name		Le nom de la caméra.
		 *\param[in]	scene		La scène parente.
		 *\param[in]	node		SceneNode parent.
		 *\param[in]	ownProjMtx	Dit si la matrice de projection est définie par le viewport \p false, ou pas \p true.
		 */
		C3D_API Camera( castor::String const & name
			, Scene & scene
			, SceneNodeSPtr const node
			, bool ownProjMtx = false );
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
		 *\brief		Updates the frustum.
		 *\~french
		 *\brief		Met à jour le frustum.
		 */
		C3D_API void updateFrustum();
		/**
		 *\~english
		 *\brief		Updates the viewport, the view matrix, and the frustum.
		 *\~french
		 *\brief		Met à jour le viewport, la matrice de vue et le frustum.
		 */
		C3D_API void update();
		/**
		 *\~english
		 *\brief		Sets the projection matrix, updates the viewport and the frustum.
		 *\param[in]	projection	The projection matrix.
		 *\~french
		 *\brief		Définit la matrice de projection, met à jour le viewport et le frustum.
		 *\param[in]	projection	La matrice de projection.
		 */
		C3D_API void setProjection( castor::Matrix4x4r const & projection );
		/**
		 *\~english
		 *\brief		Checks if a submesh is visible, through a geometry.
		 *\param[in]	geometry	The geometry.
		 *\param[in]	submesh		The submesh.
		 *\return		\p false if the submesh is not visible.
		 *\~french
		 *\brief
		 *\brief		Vérifie si un sous-maillage est visible, via une géométrie.
		 *\param[in]	geometry	La géométrie.
		 *\param[in]	submesh		Le sous-maillage.
		 *\return		\p false si le sous-maillage n'est pas visible.
		 */
		C3D_API bool isVisible( Geometry const & geometry, Submesh const & submesh )const;
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		inline Viewport const & getViewport()const
		{
			return m_viewport;
		}

		inline Viewport & getViewport()
		{
			return m_viewport;
		}

		inline castor::Matrix4x4r const & getView()const
		{
			return m_view;
		}

		inline castor::Matrix4x4r const & getProjection()const
		{
			return m_ownProjection
				? m_projection
				: m_viewport.getProjection();
		}

		inline ViewportType getViewportType()const
		{
			return m_viewport.getType();
		}

		inline castor::Size const & getSize()const
		{
			return m_viewport.getSize();
		}

		inline uint32_t getWidth()const
		{
			return m_viewport.getWidth();
		}

		inline uint32_t getHeight()const
		{
			return m_viewport.getHeight();
		}

		inline float getNear()const
		{
			return m_viewport.getNear();
		}

		inline float getFar()const
		{
			return m_viewport.getFar();
		}

		inline float getRatio()const
		{
			return m_viewport.getRatio();
		}

		inline castor::Angle const & getFovY()const
		{
			return m_viewport.getFovY();
		}

		bool isVisible( castor::BoundingBox const & box
			, castor::Matrix4x4r const & transformations )const
		{
			return m_frustum.isVisible( box
				, transformations );
		}

		bool isVisible( castor::BoundingSphere const & sphere
			, castor::Matrix4x4r const & transformations
			, castor::Point3r const & scale )const
		{
			return m_frustum.isVisible( sphere
				, transformations
				, scale );
		}

		bool isVisible( castor::Point3r const & point )const
		{
			return m_frustum.isVisible( point );
		}
		/**@}*/
		/**
		*\~english
		*name
		*	Mutators.
		*\~french
		*name
		*	Mutateurs.
		*/
		/**@{*/
		inline void setView( castor::Matrix4x4r const & view )
		{
			m_view = view;
			onChanged( *this );
		}

		void resize( uint32_t width, uint32_t height )
		{
			resize( castor::Size( width, height ) );
		}

		inline void resize( castor::Size const & size )
		{
			if ( m_viewport.getSize() != size )
			{
				m_viewport.resize( size );
				onChanged( *this );
			}
		}

		inline void setViewportType( ViewportType value )
		{
			m_viewport.updateType( value );
			onChanged( *this );
		}
		/**@}*/

	private:
		void onNodeChanged( SceneNode const & node );

	public:
		//!\~english	The signal raised when the camera has changed.
		//!\~french		Le signal levé lorsque la caméra a changé.
		mutable OnCameraChanged onChanged;

	private:
		friend class Scene;
		Viewport m_viewport;
		Frustum m_frustum;
		castor::Matrix4x4r m_view;
		bool m_nodeChanged{ true };
		bool m_ownProjection{ false };
		castor::Matrix4x4r m_projection;
	};
}

#endif
