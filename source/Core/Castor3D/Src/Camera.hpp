/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

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

#include "Renderable.hpp"
#include "MovableObject.hpp"
#include "BinaryParser.hpp"

#include <PlaneEquation.hpp>

#pragma warning( push )
#pragma warning( disable:4251 )
#pragma warning( disable:4275 )

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\date
	\~english
	\brief		The selection mode enumeration
	\~french
	\brief		Enumération des modes de sélection
	*/
	typedef enum eSELECTION_MODE CASTOR_TYPE( uint8_t )
	{
		eSELECTION_MODE_VERTEX,		//!< Vertex selection mode
		eSELECTION_MODE_EDGE,		//!< Edge selection mode
		eSELECTION_MODE_FACE,		//!< Face selection mode
		eSELECTION_MODE_SUBMESH,	//!< Submesh selection mode
		eSELECTION_MODE_OBJECT,		//!< Geometry selection mode
		eSELECTION_MODE_COUNT
	}	eSELECTION_MODE;
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0
	\date		27/02/2013
	\~english
	\brief		Struct holding a selection result
	\~french
	\brief		Structure contenant le résultat d'une sélection
	*/
	struct C3D_API stSELECT_RESULT
	{
		//!\~english A selection can be a vertex	\~french Une sélection peut être un vertex
		VertexWPtr m_pVertex;
		//!\~english A selection can be a face	\~french Une sélection peut être une face
		FaceWPtr m_pFace;
		//!\~english A selection can be a submesh	\~french Une sélection peut être un sous-maillage
		SubmeshWPtr m_pSubmesh;
		//!\~english A selection can be a mesh	\~french Une sélection peut être un maillage
		MeshWPtr m_pMesh;
		//!\~english A selection can be a geometry	\~french Une sélection peut être une géométrie
		GeometryWPtr m_pGeometry;
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
	class C3D_API Camera
		:	public MovableObject
		,	public Renderable< Camera, CameraRenderer >
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
		class C3D_API TextLoader
			:	public MovableObject::TextLoader
		{
		public:
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
			virtual bool operator()( Camera const & p_camera, Castor::TextFile & p_file );
		};
		/*
		\author		Sylvain DOREMUS
		\date		14/02/2010
		\~english
		\brief		Camera loader
		\~english
		\brief		Loader de Camera
		*/
		class C3D_API BinaryParser
			:	public MovableObject::BinaryParser
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor
			 *\param[in]	p_path	The current folder path
			 *\~french
			 *\brief		Constructeur
			 *\param[in]	p_path	Le chemin d'accès au dossier courant
			 */
			BinaryParser( Castor::Path const & p_path );
			/**
			 *\~english
			 *\brief		Function used to fill the chunk from specific data
			 *\param[in]	p_obj	The object to write
			 *\param[out]	p_chunk	The chunk to fill
			 *\return		\p false if any error occured
			 *\~french
			 *\brief		Fonction utilisée afin de remplir le chunk de données spécifiques
			 *\param[in]	p_obj	L'objet à écrire
			 *\param[out]	p_chunk	Le chunk à remplir
			 *\return		\p false si une erreur quelconque est arrivée
			 */
			virtual bool Fill( Camera const & p_obj, BinaryChunk & p_chunk )const;
			/**
			 *\~english
			 *\brief		Function used to retrieve specific data from the chunk
			 *\param[out]	p_obj	The object to read
			 *\param[in]	p_chunk	The chunk containing data
			 *\return		\p false if any error occured
			 *\~french
			 *\brief		Fonction utilisée afin de récupérer des données spécifiques à partir d'un chunk
			 *\param[out]	p_obj	L'objet à lire
			 *\param[in]	p_chunk	Le chunk contenant les données
			 *\return		\p false si une erreur quelconque est arrivée
			 */
			virtual bool Parse( Camera & p_obj, BinaryChunk & p_chunk )const;
		};
		/**
		 *\~english
		 *\brief		Constructor, needs the camera renderer, the name, window size and projection type. Creates a viewport renderer and a viewport
		 *\remark		Not to be used by the user, use Scene::CreateCamera instead
		 *\param[in]	p_strName			The camera name
		 *\param[in]	p_pNode				The parent camera node
		 *\param[in]	p_pViewport			Viewport to copy
		 *\param[in]	p_ePrimitiveType	The camera display mode
		 *\~french
		 *\brief		Constructeur
		 *\remark		L'utilisateur ne devrait pas s'en servir, préférer l'utilisation de Scene::CreateCamera
		 *\param[in]	p_strName			Le nom de la caméra
		 *\param[in]	p_pNode				SceneNode parent
		 *\param[in]	p_pViewport			Viewport à copier
		 *\param[in]	p_ePrimitiveType	Mode d'affichage de la caméra
		 */
		Camera( SceneSPtr p_pScene, Castor::String const & p_strName, const SceneNodeSPtr p_pNode, ViewportSPtr p_pViewport, eTOPOLOGY p_ePrimitiveType = eTOPOLOGY_TRIANGLES );
		/**
		 *\~english
		 *\brief		Constructor, needs the camera renderer, the name, window size and projection type. Creates a viewport renderer and a viewport
		 *\remark		Not to be used by the user, use Scene::CreateCamera instead
		 *\param[in]	p_strName			The camera name
		 *\param[in]	p_pNode				The parent camera node
		 *\param[in]	p_size				The viewport render size
		 *\param[in]	p_eType				Projection type
		 *\param[in]	p_ePrimitiveType	The camera display mode
		 *\~french
		 *\brief		Constructeur
		 *\remark		L'utilisateur ne devrait pas s'en servir, préférer l'utilisation de Scene::CreateCamera
		 *\param[in]	p_strName			Le nom de la caméra
		 *\param[in]	p_pNode				SceneNode parent
		 *\param[in]	p_size				Les dimensions de rendu du viewport
		 *\param[in]	p_eType				Type de projection
		 *\param[in]	p_ePrimitiveType	Mode d'affichage de la caméra
		 */
		Camera( SceneSPtr p_pScene, Castor::String const & p_strName, const SceneNodeSPtr p_pNode, Castor::Size const & p_size, eVIEWPORT_TYPE p_eType, eTOPOLOGY p_ePrimitiveType = eTOPOLOGY_TRIANGLES );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~Camera();
		/**
		 *\~english
		 *\brief		Applies the viewport, the rotation ...
		 *\~french
		 *\brief		Applique le viewport, la rotation ...
		 */
		virtual void Render();
		/**
		 *\~english
		 *\brief		Removes the transformations
		 *\~french
		 *\brief		Enlève les transformations
		 */
		virtual void EndRender();
		/**
		 *\~english
		 *\brief		Resizes the viewport
		 *\param[in]	p_uiWidth, p_uiHeight	Display window size
		 *\~french
		 *\brief		Redimensionne le viewport
		 *\param[in]	p_uiWidth, p_uiHeight	Dimensions de la fenêtre d'affichage
		 */
		void Resize( uint32_t p_uiWidth, uint32_t p_uiHeight );
		/**
		 *\~english
		 *\brief		Resizes the viewport
		 *\param[in]	p_size	Display window size
		 *\~french
		 *\brief		Redimensionne le viewport
		 *\param[in]	p_size	Dimensions de la fenêtre d'affichage
		 */
		void Resize( Castor::Size const & p_size );
		/**
		 *\~english
		 *\brief		Sets the orientation to identity
		 *\~french
		 *\brief		Met l'orientation à l'identité
		 */
		void ResetOrientation();
		/**
		 *\~english
		 *\brief		Sets the position to 0
		 *\~french
		 *\brief		Réinitialise la position
		 */
		void ResetPosition();
		/**
		 *\~english
		 *\brief		Returns the first object at mouse coords x and y
		 *\param[in]	p_pScene	The scene used for the selection
		 *\param[in]	p_eMode		The selection mode (vertex, face, submesh, geometry)
		 *\param[in]	p_iX		The x mouse coordinate
		 *\param[in]	p_iY		The y mouse coordinate
		 *\param[out]	p_stFound	The selection result
		 *\return		\p true if something was found, false if not
		 *\~french
		 *\brief		Récupère l'objet le plus proche aux coordonnées souris x et y
		 *\param[in]	p_pScene	La scène où on doit sélectionner un objet
		 *\param[in]	p_eMode		Le mode de sélection (vertex, face, submesh, geometry)
		 *\param[in]	p_iX, p_iY	Les coordonnées de la souris
		 *\param[out]	p_stFound	Le résultat de la sélection
		 *\return		\p false si aucun objet n'a été trouvé
		*/
		bool Select( SceneSPtr p_pScene, eSELECTION_MODE p_eMode, int p_iX, int p_iY, stSELECT_RESULT & p_stFound );
		/**
		 *\~english
		 *\brief		Retrieves the Viewport
		 *\return		The Viewport
		 *\~french
		 *\brief		Récupère le Viewport
		 *\return		Le Viewport
		 */
		inline ViewportSPtr	GetViewport()const
		{
			return m_pViewport;
		}
		/**
		 *\~english
		 *\brief		Retrieves the display mode
		 *\return		The display mode
		 *\~french
		 *\brief		Récupère le mode d'affichage
		 *\return		Le mode d'affichage
		 */
		inline eTOPOLOGY GetPrimitiveType()const
		{
			return m_ePrimitiveType;
		}
		/**
		 *\~english
		 *\brief		Sets the display mode
		 *\param[in]	val	The display mode
		 *\~french
		 *\brief		Définit le mode d'affichage
		 *\param[in]	val	Le mode d'affichage
		 */
		inline void SetPrimitiveType( eTOPOLOGY val )
		{
			m_ePrimitiveType = val;
		}
		/**
		 *\~english
		 *\brief		Retrieves the viewport type
		 *\return		The viewport type
		 *\~french
		 *\brief		Récupère le type de viewport
		 *\return		Le type de viewport
		 */
		eVIEWPORT_TYPE GetViewportType()const;
		/**
		 *\~english
		 *\brief		Sets the viewport type
		 *\param[in]	val	The viewport type
		 *\~french
		 *\brief		Définit le type de viewport
		 *\param[in]	val	Le type de viewport
		 */
		void SetViewportType( eVIEWPORT_TYPE val );
		/**
		 *\~english
		 *\brief		Retrieves the viewport width
		 *\return		The width
		 *\~french
		 *\brief		Récupère la largeur du viewport
		 *\return		La largeur
		 */
		uint32_t GetWidth()const;
		/**
		 *\~english
		 *\brief		Retrieves the viewport height
		 *\return		The height
		 *\~french
		 *\brief		Récupère la hauteur du viewport
		 *\return		La hauteur
		 */
		uint32_t GetHeight()const;
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
		bool IsVisible( Castor::CubeBox const & p_box, Castor::Matrix4x4r const & m_transformations )const;
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
		bool IsVisible( Castor::Point3r const & p_point )const;

	private:
		friend class Scene;
		friend class CameraRenderer;
		//!\~english The viewport of the camera	\~french Le viewport de la caméra
		ViewportSPtr m_pViewport;
		//!\~english Primitive display type	\~french Type des primitives d'affichage
		eTOPOLOGY m_ePrimitiveType;
		//!\~english The view frustum's planes	\~french Les plans du frustum de vue
		Castor::PlaneEquation< real > m_planes[eFRUSTUM_PLANE_COUNT];
	};
}

#pragma warning( pop )

#endif
