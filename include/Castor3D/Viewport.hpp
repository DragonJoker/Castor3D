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
#ifndef ___C3D_Viewport___
#define ___C3D_Viewport___

#include "Prerequisites.hpp"

#pragma warning( push )
#pragma warning( disable:4251 )
#pragma warning( disable:4275 )

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
	class C3D_API Viewport
	{
	public:
		static const Castor::String string_type[2];
		
		/*!
		\author		Sylvain DOREMUS
		\version	0.6.1.0
		\date		19/10/2011
		\~english
		\brief		Viewport loader
		\~french
		\brief		Loader de Viewport
		*/
		class C3D_API TextLoader : public Castor::Loader< Viewport, Castor::eFILE_TYPE_TEXT, Castor::TextFile >, CuNonCopyable
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor
			 *\~french
			 *\brief		Constructeur
			 */
			TextLoader( Castor::File::eENCODING_MODE p_eEncodingMode=Castor::File::eENCODING_MODE_ASCII );
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
			virtual bool operator ()( Castor3D::Viewport const & p_viewport, Castor::TextFile & p_file );
		};

	private:
		//!\~english	The left viewport plane	\~french Position du plan gauche
		real m_rLeft;
		//!\~english	The right viewport plane	\~french Position du plan droit
		real m_rRight;
		//!\~english	The top viewport plane	\~french Position du plan haut
		real m_rTop;
		//!\~english	The bottom viewport plane	\~french Position du plan bas
		real m_rBottom;
		//!\~english	The viewport farthest viewable distance	\~french Position du plan lointain
		real m_rFar;
		//!\~english	The viewport nearest viewable distance	\~french Position du plan proche
		real m_rNear;
		//!\~english	The viewport vertical FOV 	\~french Angle de vue vezrtical
		Castor::Angle m_aFovY;
		//!\~english	The projection type	\~french Type de projection
		eVIEWPORT_TYPE m_eType;
		//!\~english	The viewport render size	\~french Dimensions du viewport
		Castor::Size m_size;
		//!\~english	The display window ratio (4:3, 16:9, ...)	\~french Ratio d'affichage
		real m_rRatio;
		//!\~english	The core engine	\~french Le moteur
		Engine * m_pEngine;
		//!\~english	Tells the view frustum's planes need to be updated	\~french Dit que les plans du frustum de vue doivent être mis à jour
		bool m_bModified;
		//!\~english	The view frustum's planes	\~french Les plans du frustum de vue
		Castor::PlaneEq< real >	m_planes[eFRUSTUM_PLANE_COUNT];

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_pEngine	The core engine
		 *\param[in]	p_size		The viewport render size
		 *\param[in]	p_eType		Projection type
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_pEngine	Le moteur
		 *\param[in]	p_size		Les dimensions de rendu du viewport
		 *\param[in]	p_pNode		SceneNode parent
		 *\param[in]	p_eType		Type de projection
		 */
		Viewport( Engine * p_pEngine, Castor::Size const & p_size, eVIEWPORT_TYPE p_eType );
		/**
		 *\~english
		 *\brief		Copy constructor
		 *\param[in]	p_object	The object to copy
		 *\~french
		 *\brief		Constructeur par copie
		 *\param[in]	p_object	L'objet à copier
		 */
		Viewport( Viewport const & p_object );
		/**
		 *\~english
		 *\brief		Move constructor
		 *\param[in]	p_object	The object to move
		 *\~french
		 *\brief		Constructeur par déplacement
		 *\param[in]	p_object	L'objet à déplacer
		 */
		Viewport( Viewport && p_object );
		/**
		 *\~english
		 *\brief		Copy assignment operator
		 *\param[in]	p_object	The object to copy
		 *\return		A reference to this object
		 *\~french
		 *\brief		Opérateur d'affectation par copie
		 *\param[in]	p_object	L'objet à copier
		 *\return		Une référence sur cet objet
		 */
		Viewport & operator =( Viewport const & p_object );
		/**
		 *\~english
		 *\brief		Move assignment operator
		 *\param[in]	p_object	The object to move
		 *\return		A reference to this object
		 *\~french
		 *\brief		Opérateur d'affectation par déplacement
		 *\param[in]	p_object	L'objet à déplacer
		 *\return		Une référence sur cet objet
		 */
		Viewport & operator =( Viewport && p_object );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~Viewport();
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
		bool Render();
		/**
		 *\~english
		 *\brief		Asks the direction from the renderer and returns it
		 *\param[in]	p_ptMouse	The in-screen position
		 *\param[out]	p_ptResult	The computed direction
		 *\~french
		 *\brief		Calcule la direction du point donné
		 *\param[in]	p_ptMouse	La position dans l'écran
		 *\param[out]	p_ptResult	La direction calculée
		 */
		void GetDirection( Castor::Point2i const & p_ptMouse, Castor::Point3r & p_ptResult);
		/**
		 *\~english
		 *\brief		Retrieves the viewport render size
		 *\return		The value
		 *\~french
		 *\brief		Récupère les dimensions de rendu du viewport
		 *\return		La valeur
		 */
		inline const Castor::Size & GetSize()const { return m_size; }
		/**
		 *\~english
		 *\brief		Sets the viewport render size
		 *\param[in]	p_size	The new value
		 *\~french
		 *\brief		Définit les dimensions de rendu du viewport
		 *\param[in]	p_size	La nouvelle valeur
		 */
		inline void SetSize( const Castor::Size & p_size ) { m_size = p_size; }
		/**
		 *\~english
		 *\brief		Sets the viewport projection type
		 *\param[in]		The new value
		 *\~french
		 *\brief		Définit le type de projection du viewport
		 *\param[in]		La nouvelle valeur
		 */
		inline eVIEWPORT_TYPE GetType()const { return m_eType; }
		/**
		 *\~english
		 *\brief		Sets the aspect ratio
		 *\param[in]	The new value
		 *\~french
		 *\brief		Définit le ratio d'aspect
		 *\param[in]	La nouvelle valeur
		 */
		inline real GetRatio()const { return m_rRatio; }
		/**
		 *\~english
		 *\brief		Sets the near plane position
		 *\param[in]	The new value
		 *\~french
		 *\brief		Définit la position du plan proche
		 *\param[in]	La nouvelle valeur
		 */
		inline real GetNear()const { return m_rNear; }
		/**
		 *\~english
		 *\brief		Sets the far plane position
		 *\param[in]	The new value
		 *\~french
		 *\brief		Définit la position du plan lointain
		 *\param[in]	La nouvelle valeur
		 */
		inline real GetFar()const { return m_rFar; }
		/**
		 *\~english
		 *\brief		Sets the view vertical angle
		 *\param[in]	The new value
		 *\~french
		 *\brief		Définit l'angle de vue vertical
		 *\param[in]	La nouvelle valeur
		 */
		inline Castor::Angle GetFovY()const { return m_aFovY; }
		/**
		 *\~english
		 *\brief		Sets the left plane position
		 *\param[in]	The new value
		 *\~french
		 *\brief		Définit la position du plan gauche
		 *\param[in]	La nouvelle valeur
		 */
		inline real GetLeft()const { return m_rLeft; }
		/**
		 *\~english
		 *\brief		Sets the right plane position
		 *\param[in]	The new value
		 *\~french
		 *\brief		Définit la position du plan droit
		 *\param[in]	La nouvelle valeur
		 */
		inline real GetRight()const { return m_rRight; }
		/**
		 *\~english
		 *\brief		Sets the top plane position
		 *\param[in]	The new value
		 *\~french
		 *\brief		Définit la position du plan haut
		 *\param[in]	La nouvelle valeur
		 */
		inline real GetTop()const { return m_rTop; }
		/**
		 *\~english
		 *\brief		Sets the bottom plane position
		 *\param[in]	The new value
		 *\~french
		 *\brief		Définit la position du plan bas
		 *\param[in]	La nouvelle valeur
		 */
		inline real GetBottom()const { return m_rBottom; }
		/**
		 *\~english
		 *\brief		Retrieves the viewport width
		 *\return		The value
		 *\~french
		 *\brief		Récupère la largeur du viewport
		 *\return		La valeur
		 */
		inline uint32_t GetWidth()const { return m_size.width(); }
		/**
		 *\~english
		 *\brief		Retrieves the viewport height
		 *\return		The value
		 *\~french
		 *\brief		Récupère la hauteur du viewport
		 *\return		La valeur
		 */
		inline uint32_t GetHeight()const { return m_size.height(); }
		/**
		 *\~english
		 *\brief		Retrieves the viewport modificiation status
		 *\return		The value
		 *\~french
		 *\brief		Récupère le statut de modification du viewport
		 *\return		La valeur
		 */
		inline bool IsModified()const { return m_bModified; }
		/**
		 *\~english
		 *\brief		Retrieves the viewport projection type
		 *\return		The value
		 *\~french
		 *\brief		Récupère le type de projection du viewport
		 *\return		La valeur
		 */
		inline void SetType( eVIEWPORT_TYPE p_eType ) { m_eType = p_eType; }
		/**
		 *\~english
		 *\brief		Retrieves the aspect ratio
		 *\return		The value
		 *\~french
		 *\brief		Récupère le ratio d'aspect
		 *\return		La valeur
		 */
		inline void SetRatio( real p_rRatio ) { m_rRatio = p_rRatio; }
		/**
		 *\~english
		 *\brief		Retrieves the near plane position
		 *\return		The value
		 *\~french
		 *\brief		Récupère la position du plan proche
		 *\return		La valeur
		 */
		inline void SetNear( real p_rNear ) { m_rNear = p_rNear;m_bModified=true; }
		/**
		 *\~english
		 *\brief		Retrieves the far plane position
		 *\return		The value
		 *\~french
		 *\brief		Récupère la position du plan lointain
		 *\return		La valeur
		 */
		inline void SetFar( real p_rFar ) { m_rFar = p_rFar;m_bModified=true; }
		/**
		 *\~english
		 *\brief		Retrieves the vertical view angle
		 *\return		The value
		 *\~french
		 *\brief		Récupère l'angle de vue vertical
		 *\return		La valeur
		 */
		inline void SetFovY( Castor::Angle const & p_aFovY ) { m_aFovY = p_aFovY;m_bModified=true; }
		/**
		 *\~english
		 *\brief		Retrieves the left plane position
		 *\return		The value
		 *\~french
		 *\brief		Récupère la position du plan gauche
		 *\return		La valeur
		 */
		inline void SetLeft( real p_rLeft ) { m_rLeft = p_rLeft;m_bModified=true; }
		/**
		 *\~english
		 *\brief		Retrieves the right plane position
		 *\return		The value
		 *\~french
		 *\brief		Récupère la position du plan droit
		 *\return		La valeur
		 */
		inline void SetRight( real p_rRight ) { m_rRight = p_rRight;m_bModified=true; }
		/**
		 *\~english
		 *\brief		Retrieves the top plane position
		 *\return		The value
		 *\~french
		 *\brief		Récupère la position du plan haut
		 *\return		La valeur
		 */
		inline void SetTop( real p_rTop ) { m_rTop = p_rTop;m_bModified=true; }
		/**
		 *\~english
		 *\brief		Retrieves the bottom plane position
		 *\return		The value
		 *\~french
		 *\brief		Récupère la position du plan bas
		 *\return		La valeur
		 */
		inline void SetBottom( real p_rBottom ) { m_rBottom = p_rBottom;m_bModified=true; }
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
		inline const Castor::PlaneEq< real > & GetFrustumPlane( eFRUSTUM_PLANE p_ePlane )const { return m_planes[p_ePlane]; }
	};
}

#pragma warning( pop )

#endif
