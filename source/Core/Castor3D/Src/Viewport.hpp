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
#ifndef ___C3D_VIEWPORT_H___
#define ___C3D_VIEWPORT_H___

#include "Castor3DPrerequisites.hpp"
#include "BinaryParser.hpp"

#include <Angle.hpp>
#include <PlaneEquation.hpp>
#include <OwnedBy.hpp>

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.1
	\date		09/02/2010
	\~english
	\brief		Class to represent a viewport
	\remark		Gives its projection type, FOV, ...
	\~french
	\brief		Classe de représentation de viewport
	\remark		Donne le type de projection FOV, ...
	*/
	class Viewport
		: public Castor::Aligned< CASTOR_ALIGN_OF( Castor::PlaneEquation< real > ) >
	{
	public:
		C3D_API static const Castor::String string_type[eVIEWPORT_TYPE_COUNT];

		/*!
		\author		Sylvain DOREMUS
		\version	0.6.1.0
		\date		19/10/2011
		\~english
		\brief		Viewport loader
		\~french
		\brief		Loader de Viewport
		*/
		class TextLoader
			: public Castor::Loader< Viewport, Castor::eFILE_TYPE_TEXT, Castor::TextFile >
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor
			 *\~french
			 *\brief		Constructeur
			 */
			C3D_API TextLoader( Castor::File::eENCODING_MODE p_encodingMode = Castor::File::eENCODING_MODE_ASCII );
			/**
			 *\~english
			 *\brief		Writes a Viewport into a text file
			 *\param[in]	p_file		The file to save the cameras in
			 *\param[in]	p_viewport	The Viewport to save
			 *\~french
			 *\brief		Ecrit un Viewport dans un fichier texte
			 *\param[in]	p_file		Le fichier
			 *\param[in]	p_viewport	Le Viewport
			 */
			C3D_API virtual bool operator()( Castor3D::Viewport const & p_viewport, Castor::TextFile & p_file );
		};
		/*!
		\author		Sylvain DOREMUS
		\version	0.7.0.0
		\date		15/04/2013
		\~english
		\brief		Viewport loader
		\~french
		\brief		Loader de Viewport
		*/
		class BinaryParser
			: public Castor3D::BinaryParser< Viewport >
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
			C3D_API BinaryParser( Castor::Path const & p_path );
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
			C3D_API virtual bool Fill( Viewport const & p_obj, BinaryChunk & p_chunk )const;
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
			C3D_API virtual bool Parse( Viewport & p_obj, BinaryChunk & p_chunk )const;
		};

	private:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_engine	The core engine.
		 *\param[in]	p_type		Projection type.
		 *\param[in]	p_fovy		Y Field of View.
		 *\param[in]	p_aspect	Width / Height ratio.
		 *\param[in]	p_left		Left clipping plane value.
		 *\param[in]	p_right		Right clipping plane value.
		 *\param[in]	p_bottom	Bottom clipping plane value.
		 *\param[in]	p_top		Top clipping plane value.
		 *\param[in]	p_near		Near clipping plane value.
		 *\param[in]	p_far		Far clipping plane value.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_engine	Le moteur.
		 *\param[in]	p_type		Type de projection.
		 *\param[in]	p_fovy		Angle de vision Y.
		 *\param[in]	p_aspect	Ratio Largeur / Hauteur.
		 *\param[in]	p_left		Position du plan gauche.
		 *\param[in]	p_right		Position du plan droit.
		 *\param[in]	p_bottom	Position du plan bas.
		 *\param[in]	p_top		Position du plan haut.
		 *\param[in]	p_near		Position du plan proche.
		 *\param[in]	p_far		Position du plan éloigné.
		 */
		C3D_API Viewport( Engine & p_engine, eVIEWPORT_TYPE p_type, Castor::Angle const & p_fovy, real p_aspect, real p_left, real p_right, real p_bottom, real p_top, real p_near, real p_far );

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_engine	The core engine
		 *\param[in]	p_type		Projection type
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_engine	Le moteur
		 *\param[in]	p_type		Type de projection
		 */
		C3D_API Viewport( Engine & p_engine, eVIEWPORT_TYPE p_type );
		/**
		 *\~english
		 *\brief		Builds a centered perspective viewport.
		 *\param[in]	p_engine	The core engine
		 *\param[in]	p_fovy		Y Field of View.
		 *\param[in]	p_aspect	Width / Height ratio.
		 *\param[in]	p_near		Near clipping plane value.
		 *\param[in]	p_far		Far clipping plane value.
		 *\return		The viewport.
		 *\~french
		 *\brief		Construit vioewport en perspective centrée.
		 *\param[in]	p_engine	Le moteur
		 *\param[in]	p_fovy		Angle de vision Y.
		 *\param[in]	p_aspect	Ratio Largeur / Hauteur.
		 *\param[in]	p_near		Position du plan proche.
		 *\param[in]	p_far		Position du plan éloigné.
		 *\return		Le viewport.
		 */
		C3D_API static Viewport Perspective( Engine & p_engine, Castor::Angle const & p_fovy, real p_aspect, real p_near, real p_far );
		/**
		 *\~english
		 *\brief		Builds a matrix that sets a non centered perspective projection from the given parameters.
		 *\param[in]	p_engine	The core engine
		 *\param[in]	p_left		Left clipping plane value.
		 *\param[in]	p_right		Right clipping plane value.
		 *\param[in]	p_bottom	Bottom clipping plane value.
		 *\param[in]	p_top		Top clipping plane value.
		 *\param[in]	p_near		Near clipping plane value.
		 *\param[in]	p_far		Far clipping plane value.
		 *\return		The viewport.
		 *\~french
		 *\brief		Construit une matrice de projection en perspective non centrée.
		 *\param[in]	p_engine	Le moteur
		 *\param[in]	p_left		Position du plan gauche.
		 *\param[in]	p_right		Position du plan droit.
		 *\param[in]	p_bottom	Position du plan bas.
		 *\param[in]	p_top		Position du plan haut.
		 *\param[in]	p_near		Position du plan proche.
		 *\param[in]	p_far		Position du plan éloigné.
		 *\return		Le viewport.
		 */
		C3D_API static Viewport Frustum( Engine & p_engine, real p_left, real p_right, real p_bottom, real p_top, real p_near, real p_far );
		/**
		 *\~english
		 *\brief		Builds a matrix that sets an orthogonal projection.
		 *\param[in]	p_engine	The core engine
		 *\param[in]	p_left		Left clipping plane value.
		 *\param[in]	p_right		Right clipping plane value.
		 *\param[in]	p_bottom	Bottom clipping plane value.
		 *\param[in]	p_top		Top clipping plane value.
		 *\param[in]	p_near		Near clipping plane value.
		 *\param[in]	p_far		Far clipping plane value.
		 *\return		The viewport.
		 *\~french
		 *\brief		Construit une matrice de projection orthographique.
		 *\param[in]	p_engine	Le moteur
		 *\param[in]	p_left		Position du plan gauche.
		 *\param[in]	p_right		Position du plan droit.
		 *\param[in]	p_bottom	Position du plan bas.
		 *\param[in]	p_top		Position du plan haut.
		 *\param[in]	p_near		Position du plan proche.
		 *\param[in]	p_far		Position du plan éloigné.
		 *\return		Le viewport.
		 */
		C3D_API static Viewport Ortho( Engine & p_engine, real p_left, real p_right, real p_bottom, real p_top, real p_near, real p_far );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API virtual ~Viewport();
		/**
		 *\~english
		 *\brief		Renders the viewport specifics
		 *\remark		Applies the perspective
		 *\return		\p true if the frustum view has been modified
		 *\~french
		 *\brief		Rend le viewport
		 *\remark		Applique la perspective
		 *\return		\p true si le frustum de vue a été modifié
		 */
		C3D_API bool Render( Pipeline & p_pipeline );
		/**
		 *\~english
		 *\brief		Retrieves the viewport render size
		 *\return		The value
		 *\~french
		 *\brief		Récupère les dimensions de rendu du viewport
		 *\return		La valeur
		 */
		inline const Castor::Size & GetSize()const
		{
			return m_size;
		}
		/**
		 *\~english
		 *\brief		Sets the viewport projection type
		 *\return		The new value
		 *\~french
		 *\brief		Définit le type de projection du viewport
		 *\return		La nouvelle valeur
		 */
		inline eVIEWPORT_TYPE GetType()const
		{
			return m_type;
		}
		/**
		 *\~english
		 *\brief		Sets the aspect ratio
		 *\return		The new value
		 *\~french
		 *\brief		Définit le ratio d'aspect
		 *\return		La nouvelle valeur
		 */
		inline real GetRatio()const
		{
			return m_ratio;
		}
		/**
		 *\~english
		 *\brief		Sets the near plane position
		 *\param[in]	The new value
		 *\~french
		 *\brief		Définit la position du plan proche
		 *\param[in]	La nouvelle valeur
		 */
		inline real GetNear()const
		{
			return m_near;
		}
		/**
		 *\~english
		 *\brief		Sets the far plane position
		 *\param[in]	The new value
		 *\~french
		 *\brief		Définit la position du plan lointain
		 *\param[in]	La nouvelle valeur
		 */
		inline real GetFar()const
		{
			return m_far;
		}
		/**
		 *\~english
		 *\brief		Sets the view vertical angle
		 *\param[in]	The new value
		 *\~french
		 *\brief		Définit l'angle de vue vertical
		 *\param[in]	La nouvelle valeur
		 */
		inline Castor::Angle GetFovY()const
		{
			return m_fovY;
		}
		/**
		 *\~english
		 *\brief		Sets the left plane position
		 *\param[in]	The new value
		 *\~french
		 *\brief		Définit la position du plan gauche
		 *\param[in]	La nouvelle valeur
		 */
		inline real GetLeft()const
		{
			return m_left;
		}
		/**
		 *\~english
		 *\brief		Sets the right plane position
		 *\param[in]	The new value
		 *\~french
		 *\brief		Définit la position du plan droit
		 *\param[in]	La nouvelle valeur
		 */
		inline real GetRight()const
		{
			return m_right;
		}
		/**
		 *\~english
		 *\brief		Sets the top plane position
		 *\param[in]	The new value
		 *\~french
		 *\brief		Définit la position du plan haut
		 *\param[in]	La nouvelle valeur
		 */
		inline real GetTop()const
		{
			return m_top;
		}
		/**
		 *\~english
		 *\brief		Sets the bottom plane position
		 *\param[in]	The new value
		 *\~french
		 *\brief		Définit la position du plan bas
		 *\param[in]	La nouvelle valeur
		 */
		inline real GetBottom()const
		{
			return m_bottom;
		}
		/**
		 *\~english
		 *\brief		Retrieves the viewport width
		 *\return		The value
		 *\~french
		 *\brief		Récupère la largeur du viewport
		 *\return		La valeur
		 */
		inline uint32_t GetWidth()const
		{
			return m_size.width();
		}
		/**
		 *\~english
		 *\brief		Retrieves the viewport height
		 *\return		The value
		 *\~french
		 *\brief		Récupère la hauteur du viewport
		 *\return		La valeur
		 */
		inline uint32_t GetHeight()const
		{
			return m_size.height();
		}
		/**
		 *\~english
		 *\brief		Retrieves the viewport modificiation status
		 *\return		The value
		 *\~french
		 *\brief		Récupère le statut de modification du viewport
		 *\return		La valeur
		 */
		inline bool IsModified()const
		{
			return m_modified;
		}
		/**
		*\~english
		*\brief		Sets the viewport render size
		*\param[in]	p_size	The new value
		*\~french
		*\brief		Définit les dimensions de rendu du viewport
		*\param[in]	p_size	La nouvelle valeur
		*/
		inline void SetSize( const Castor::Size & p_size )
		{
			m_size = p_size;
		}
		/**
		 *\~english
		 *\brief		Retrieves the viewport projection type
		 *\return		The value
		 *\~french
		 *\brief		Récupère le type de projection du viewport
		 *\return		La valeur
		 */
		inline void SetType( eVIEWPORT_TYPE p_type )
		{
			m_modified |= m_type != p_type;
			m_type = p_type;
		}
		/**
		 *\~english
		 *\brief		Retrieves the aspect ratio
		 *\return		The value
		 *\~french
		 *\brief		Récupère le ratio d'aspect
		 *\return		La valeur
		 */
		inline void SetRatio( real p_rRatio )
		{
			m_modified |= m_ratio != p_rRatio;
			m_ratio = p_rRatio;
		}
		/**
		 *\~english
		 *\brief		Retrieves the near plane position
		 *\return		The value
		 *\~french
		 *\brief		Récupère la position du plan proche
		 *\return		La valeur
		 */
		inline void SetNear( real p_rNear )
		{
			m_modified |= m_near != p_rNear;
			m_near = p_rNear;
		}
		/**
		 *\~english
		 *\brief		Retrieves the far plane position
		 *\return		The value
		 *\~french
		 *\brief		Récupère la position du plan lointain
		 *\return		La valeur
		 */
		inline void SetFar( real p_rFar )
		{
			m_modified |= m_far != p_rFar;
			m_far = p_rFar;
		}
		/**
		 *\~english
		 *\brief		Retrieves the vertical view angle
		 *\return		The value
		 *\~french
		 *\brief		Récupère l'angle de vue vertical
		 *\return		La valeur
		 */
		inline void SetFovY( Castor::Angle const & p_aFovY )
		{
			m_modified |= m_fovY != p_aFovY;
			m_fovY = p_aFovY;
		}
		/**
		 *\~english
		 *\brief		Retrieves the left plane position
		 *\return		The value
		 *\~french
		 *\brief		Récupère la position du plan gauche
		 *\return		La valeur
		 */
		inline void SetLeft( real p_rLeft )
		{
			m_modified |= m_left != p_rLeft;
			m_left = p_rLeft;
		}
		/**
		 *\~english
		 *\brief		Retrieves the right plane position
		 *\return		The value
		 *\~french
		 *\brief		Récupère la position du plan droit
		 *\return		La valeur
		 */
		inline void SetRight( real p_rRight )
		{
			m_modified |= m_right != p_rRight;
			m_right = p_rRight;
		}
		/**
		 *\~english
		 *\brief		Retrieves the top plane position
		 *\return		The value
		 *\~french
		 *\brief		Récupère la position du plan haut
		 *\return		La valeur
		 */
		inline void SetTop( real p_rTop )
		{
			m_modified |= m_top != p_rTop;
			m_top = p_rTop;
		}
		/**
		 *\~english
		 *\brief		Retrieves the bottom plane position
		 *\return		The value
		 *\~french
		 *\brief		Récupère la position du plan bas
		 *\return		La valeur
		 */
		inline void SetBottom( real p_rBottom )
		{
			m_modified |= m_bottom != p_rBottom;
			m_bottom = p_rBottom;
		}
		/**
		 *\~english
		 *\brief		Retrieves the wanted frustum view plane
		 *\param[in]	p_ePlane	The wanted plane index
		 *\return		The value
		 *\~french
		 *\brief		Récupère le plan voulu du frustum de vue
		 *\param[in]	p_ePlane	L'index du plan voulu
		 *\return		La valeur
		 */
		inline const Castor::PlaneEquation< real > & GetFrustumPlane( eFRUSTUM_PLANE p_ePlane )const
		{
			return m_planes[p_ePlane];
		}
		/**
		 *\~english
		 *\brief		Retrieves the projection matrix
		 *\return		The matrix
		 *\~french
		 *\brief		Récupère la matrice de projection
		 *\return		La matrice
		 */
		inline Castor::Matrix4x4r const & GetProjection()const
		{
			return m_projection;
		}

	private:
		//!\~english The left viewport plane	\~french Position du plan gauche
		real m_left;
		//!\~english The right viewport plane	\~french Position du plan droit
		real m_right;
		//!\~english The top viewport plane	\~french Position du plan haut
		real m_top;
		//!\~english The bottom viewport plane	\~french Position du plan bas
		real m_bottom;
		//!\~english The viewport farthest viewable distance	\~french Position du plan lointain
		real m_far;
		//!\~english The viewport nearest viewable distance	\~french Position du plan proche
		real m_near;
		//!\~english The viewport vertical FOV 	\~french Angle de vue vezrtical
		Castor::Angle m_fovY;
		//!\~english The projection type	\~french Type de projection
		eVIEWPORT_TYPE m_type;
		//!\~english The viewport render size	\~french Dimensions du viewport
		Castor::Size m_size;
		//!\~english The display window ratio (4:3, 16:9, ...)	\~french Ratio d'affichage
		real m_ratio;
		//!\~english Tells the view frustum's planes need to be updated	\~french Dit que les plans du frustum de vue doivent être mis à jour
		bool m_modified;
		//!\~english The view frustum's planes	\~french Les plans du frustum de vue
		Castor::PlaneEquation< real > m_planes[eFRUSTUM_PLANE_COUNT];
		//!\~english The projection matrix.	\~french La matrice de projection.
		Castor::Matrix4x4r m_projection;
	};
}

#endif
