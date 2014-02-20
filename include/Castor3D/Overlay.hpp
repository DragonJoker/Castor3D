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
#ifndef ___C3D_Overlay___
#define ___C3D_Overlay___

#include "Prerequisites.hpp"

#pragma warning( push )
#pragma warning( disable:4251 )
#pragma warning( disable:4275 )

#ifdef DrawText
#	undef DrawText
#endif

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		09/02/2010
	\version	0.1
	\~english
	\brief		Holds specific members for an overlay category
	\~french
	\brief		Contient les membres spécifiques à une catégorie d'incrustation
	*/
	class OverlayCategory 
	{
	public:
		/*!
		\author 	Sylvain DOREMUS
		\date 		14/02/2010
		\~english
		\brief		OverlayCategory loader
		\remark		Loads and saves overlays from/into a file
		\~french
		\brief		OverlayCategory loader
		\remark		Charge et enregistre les incrustations dans des fichiers
		*/
		class C3D_API TextLoader : public Castor::Loader< OverlayCategory, Castor::eFILE_TYPE_TEXT, Castor::TextFile >, CuNonCopyable
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
			 *\brief		Saves an overlay into a text file
			 *\param[in]	p_file		the file to save the overlay in
			 *\param[in]	p_overlay	the overlay to save
			 *\return		\p true if everything is OK
			 *\~french
			 *\brief		Sauvegarde l'incrustation donnée dans un fichier texte
			 *\param[in]	p_file		Le fichier où enregistrer l'incrustation
			 *\param[in]	p_overlay	L'incrustation à enregistrer
			 *\return		\p true si tout s'est bien passé
			 */
			virtual bool operator ()( OverlayCategory const & p_overlay, Castor::TextFile & p_file);
		};

	protected:
		//!\~english	The overlay	\~french	L'incrustation
		Overlay * m_pOverlay;
		//!\~english	The relative position (to parent or screen)	\~french	La position relative (au parent ou à l'écran)
		Castor::Point2r m_ptPosition;
		//!\~english	The relative size (to parent or screen)	\~french	La taille relative (à l'écran ou au parent)
		Castor::Point2r m_ptSize;
		//!\~english	The visibility	\~french	La visibilité
		bool m_bVisible;
		//!\~english	The material used by the overlay	\~french	Le matériau utilisé par l'incrustation
		MaterialWPtr m_pMaterial;
		//!\~english	The overlay z index	\~french	Le Z index de l'overlay
		int m_iCurrentZIndex;
		//!\~english	The material name	\~french	Le nom du matériau
		Castor::String m_strMatName;
		//!\~english	The overlay type	\~french	Le type de l'incrustation
		eOVERLAY_TYPE m_eType;
		//!\~english	The overlay renderer	\~french	Le renderer d'incrustation
		OverlayRendererWPtr m_pRenderer;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_eType		The overlay type
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_eType		Le type de l'incrustation
		 */
		OverlayCategory( eOVERLAY_TYPE p_eType );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~OverlayCategory();
		/**
		 *\~english
		 *\brief		Draws the overlay
		 *\~french
		 *\brief		Dessine l'incrustation
		 */
		virtual void Render()=0;
		/**
		 *\~english
		 *\brief		Sets the overlay position
		 *\param[in]	p_pParent		The parent overlay
		 *\param[in]	p_ptPosition	The new value
		 *\~french
		 *\brief		Définit la position de l'incrustation
		 *\param[in]	p_pParent		L'overlay parent
		 *\param[in]	p_ptPosition	La nouvelle valeur
		 */
		void SetPosition( OverlaySPtr p_pParent, Castor::Point2r const & p_ptPosition );
		/**
		 *\~english
		 *\brief		Sets the overlay size
		 *\param[in]	p_pParent	The parent overlay
		 *\param[in]	p_ptSize	The new value
		 *\~french
		 *\brief		Définit la taille de l'incrustation
		 *\param[in]	p_pParent	L'overlay parent
		 *\param[in]	p_ptSize	La nouvelle valeur
		 */
		void SetSize( OverlaySPtr p_pParent, Castor::Point2r const & p_ptSize );
		/**
		 *\~english
		 *\brief		Sets the material
		 *\param[in]	p_pMaterial	The new value
		 *\~french
		 *\brief		Définit le matériau
		 *\param[in]	p_pMaterial	La nouvelle valeur
		 */
		void SetMaterial( MaterialSPtr p_pMaterial );
		/**
		 *\~english
		 *\brief		Retrieves the overlay name
		 *\return		The value
		 *\~french
		 *\brief		Récupère le nom de l'incrustation
		 *\return		La valeur
		 */
		Castor::String const & GetOverlayName()const;
		/**
		 *\~english
		 *\brief		Retrieves the overlay size
		 *\return		The value
		 *\~french
		 *\brief		Récupère la taille de l'incrustation
		 *\return		La valeur
		 */
		inline Castor::Point2r const & GetSize()const { return m_ptSize; }
		/**
		 *\~english
		 *\brief		Retrieves the overlay position
		 *\return		The value
		 *\~french
		 *\brief		Récupère la position de l'incrustation
		 *\return		La valeur
		 */
		inline Castor::Point2r const & GetPosition()const { return m_ptPosition; }
		/**
		 *\~english
		 *\brief		Retrieves the overlay size
		 *\return		The value
		 *\~french
		 *\brief		Récupère la taille de l'incrustation
		 *\return		La valeur
		 */
		inline Castor::Point2r & GetSize() { return m_ptSize; }
		/**
		 *\~english
		 *\brief		Retrieves the overlay position
		 *\return		The value
		 *\~french
		 *\brief		Récupère la position de l'incrustation
		 *\return		La valeur
		 */
		inline Castor::Point2r & GetPosition() { return m_ptPosition; }
		/**
		 *\~english
		 *\brief		Retrieves the overlay type
		 *\return		The value
		 *\~french
		 *\brief		Récupère le type de l'incrustation
		 *\return		La valeur
		 */
		inline eOVERLAY_TYPE GetType()const { return m_eType; }
		/**
		 *\~english
		 *\brief		Retrieves the visibility status
		 *\return		The value
		 *\~french
		 *\brief		Récupère le statut de visibilité
		 *\return		La valeur
		 */
		inline bool IsVisible()const { return m_bVisible; }
		/**
		 *\~english
		 *\brief		Retrieves the material
		 *\return		The value
		 *\~french
		 *\brief		Récupère le matériau
		 *\return		La valeur
		 */
		inline MaterialSPtr GetMaterial()const { return m_pMaterial.lock(); }
		/**
		 *\~english
		 *\brief		Retrieves the Z index
		 *\return		The value
		 *\~french
		 *\brief		Récupère le Z index
		 *\return		La valeur
		 */
		inline int GetZIndex()const { return m_iCurrentZIndex; }
		/**
		 *\~english
		 *\brief		Retrieves the overlay
		 *\return		The value
		 *\~french
		 *\brief		Récupère l'incrustation
		 *\return		La valeur
		 */
		inline Overlay const & GetOverlay()const { return *m_pOverlay; }
		/**
		 *\~english
		 *\brief		Retrieves the overlay
		 *\return		The value
		 *\~french
		 *\brief		Récupère l'incrustation
		 *\return		La valeur
		 */
		inline Overlay & GetOverlay() { return *m_pOverlay; }
		/**
		 *\~english
		 *\brief		Sets the overlay
		 *\param[in]	val	The new value
		 *\~french
		 *\brief		Définit l'incrustation
		 *\param[in]	val	La nouvelle valeur
		 */
		inline void SetOverlay( Overlay * val ) { m_pOverlay = val; }
		/**
		 *\~english
		 *\brief		Sets the visibility status
		 *\param[in]	val	The new value
		 *\~french
		 *\brief		Définit le statut de visibilité
		 *\param[in]	val	La nouvelle valeur
		 */
		inline void SetVisible( bool val ) { m_bVisible = val; }
		/**
		 *\~english
		 *\brief		Sets the overlay position
		 *\param[in]	val	The new value
		 *\~french
		 *\brief		Définit la position de l'incrustation
		 *\param[in]	val	La nouvelle valeur
		 */
		inline void SetPosition( Castor::Point2r const & val ) { m_ptPosition = val; }
		/**
		 *\~english
		 *\brief		Sets the overlay size
		 *\param[in]	val	The new value
		 *\~french
		 *\brief		Définit la taille de l'incrustation
		 *\param[in]	val	La nouvelle valeur
		 */
		inline void SetSize	( Castor::Point2r const & val ) { m_ptSize = val; }
		/**
		 *\~english
		 *\brief		Sets the Z index
		 *\param[in]	p_iZIndex	The new value
		 *\~french
		 *\brief		Définit le Z index
		 *\param[in]	p_iZIndex	La nouvelle valeur
		 */
		inline void SetZIndex( int p_iZIndex ) { m_iCurrentZIndex = p_iZIndex; }
		/**
		 *\~english
		 *\brief		Sets the overlay renderer
		 *\param[in]	p_pRenderer	The new value
		 *\~french
		 *\brief		Définit le renderer d'incrustations
		 *\param[in]	p_pRenderer	La nouvelle valeur
		 */
		inline void SetRenderer( OverlayRendererSPtr p_pRenderer ) { m_pRenderer = p_pRenderer; }
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		25/08/2010
	\~english
	\brief		The overlay class
	\remark		An overlay is a 2D element which is displayed at foreground
	\~french
	\brief		La classe d'incrustation
	\remark		Une incrustation est un élément 2D qui est affiché en premier plan
	*/
	class C3D_API Overlay : CuNonCopyable
	{
	public:
		/*!
		\author 	Sylvain DOREMUS
		\date 		14/02/2010
		\~english
		\brief		Overlay loader
		\remark		Loads and saves overlays from/into a file
		\~french
		\brief		Overlay loader
		\remark		Charge et enregistre les incrustations dans des fichiers
		*/
		class C3D_API TextLoader : public Castor::Loader< Overlay, Castor::eFILE_TYPE_TEXT, Castor::TextFile >, CuNonCopyable
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
			 *\brief		Saves an overlay into a text file
			 *\param[in]	p_file		the file to save the overlay in
			 *\param[in]	p_overlay	the overlay to save
			 *\return		\p true if everything is OK
			 *\~french
			 *\brief		Sauvegarde l'incrustation donné dans un fichier texte
			 *\param[in]	p_file		Le fichier où enregistrer l'incrustation
			 *\param[in]	p_overlay	L'incrustation à enregistrer
			 *\return		\p true si tout s'est bien passé
			 */
			virtual bool operator ()( Overlay const & p_overlay, Castor::TextFile & p_file);
		};

	protected:
		typedef OverlayPtrIntMap::iterator iterator;
		typedef OverlayPtrIntMap::const_iterator const_iterator;
		//!\~english	The overlay name	\~french	Le nom de l'incrustation
		Castor::String m_strName;
		//!\~english	The parent overlay, if any	\~french	L'incrustation parente, s'il y en a
		OverlayWPtr m_pParent;
		//!\~english	The childs	\~french	Les enfants
		OverlayPtrIntMap m_mapOverlays;
		//!\~english	The overlay category	\~french	La catégorie de l'incrustation
		OverlayCategorySPtr m_pOverlayCategory;
		//!\~english	The parent scene	\~french	La scène parente
		SceneWPtr m_pScene;
		//!\~english	The childs	\~french	Les enfants
		mutable OverlayPtrArray m_arrayOverlays;
		//!\~english	The factory	\~french	La fabrique
		OverlayFactory & m_factory;
		//!\~english	The engine	\~french	Le moteur
		Engine * m_pEngine;
		//!\~english	The render system	\~french	Le système de rendu
		RenderSystem * m_pRenderSystem;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_pEngine	The engine
		 *\param[in]	p_factory	The factory
		 *\param[in]	p_eType		The overlay type
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_pEngine	Le moteur
		 *\param[in]	p_factory	La fabrique
		 *\param[in]	p_eType		Le type de l'incrustation
		 */
		Overlay( Engine * p_pEngine, OverlayFactory & p_factory, eOVERLAY_TYPE p_eType );
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_pEngine	The engine
		 *\param[in]	p_factory	The factory
		 *\param[in]	p_eType		The overlay type
		 *\param[in]	p_pScene	The scene holding the overlay
		 *\param[in]	p_pParent	The parent overlay (if any)
		 *\~english
		 *\brief		Constructeur
		 *\param[in]	p_pEngine	Le moteur
		 *\param[in]	p_factory	La fabrique
		 *\param[in]	p_eType		Le type de l'incrustation
		 *\param[in]	p_pScene	La scène parent
		 *\param[in]	p_pParent	L'incrustation parente
		 */
		Overlay( Engine * p_pEngine, OverlayFactory & p_factory, eOVERLAY_TYPE p_eType, SceneSPtr p_pScene, OverlaySPtr p_pParent );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~Overlay();
		/**
		 *\~english
		 *\brief		Registers all overlay categories
		 *\~french
		 *\brief		Enregistre toutes les catégories
		 */
		static void Register( OverlayFactory & p_factory );
		/**
		 *\~english
		 *\brief		Draws the overlay
		 *\~french
		 *\brief		Dessine l'incrustation
		 */
		virtual void Render();
		/**
		 *\~english
		 *\brief		Adds a child to the overlay
		 *\param[in]	p_pOverlay	The overlay to add
		 *\param[in]	p_iZIndex	The wanted Z Index for the child
		 *\return		\p true if successful, false if not
		 *\~french
		 *\brief		Ajoute un enfant à l'incrustation
		 *\param[in]	p_pOverlay	L'incrustation enfant
		 *\param[in]	p_iZIndex	Le Z index souhaité pour l'enfant
		 *\return		\p true si tout s'est bien passé
		 */
		bool AddChild( OverlaySPtr p_pOverlay, int p_iZIndex );
		/**
		 *\~english
		 *\brief		Initialises the overlay
		 *\remark		Checks material used, adds automatic shader if needed.
						<br />Initialises renderer part
		 *\~french
		 *\brief		Initialise l'incrustation
		 *\remark		Vérifie le matériau utilisé, ajoute un shader automatique si besoin est
						<br />Initialise la partie rendu.
		 */
		void Initialise();
		/**
		 *\~english
		 *\brief		Retrieves the engine
		 *\return		The value
		 *\~french
		 *\brief		Récupère le moteur
		 *\return		La valeur
		 */
		inline Engine * GetEngine() { return m_pEngine; }
		/**
		 *\~english
		 *\brief		Retrieves the overlay name
		 *\return		The value
		 *\~french
		 *\brief		Récupère le nom de l'incrustation
		 *\return		La valeur
		 */
		inline Castor::String const & GetName()const { return m_strName; }
		/**
		 *\~english
		 *\brief		Retrieves the overlay position
		 *\return		The value
		 *\~french
		 *\brief		Récupère la position de l'incrustation
		 *\return		La valeur
		 */
		inline Castor::Point2r const & GetPosition()const { return m_pOverlayCategory->GetPosition(); }
		/**
		 *\~english
		 *\brief		Retrieves the overlay size
		 *\return		The value
		 *\~french
		 *\brief		Récupère la taille de l'incrustation
		 *\return		La valeur
		 */
		inline Castor::Point2r const & GetSize()const { return m_pOverlayCategory->GetSize(); }
		/**
		 *\~english
		 *\brief		Retrieves the visibility status
		 *\return		The value
		 *\~french
		 *\brief		Récupère le statut de visibilité
		 *\return		La valeur
		 */
		inline bool IsVisible()const { return m_pOverlayCategory->IsVisible(); }
		/**
		 *\~english
		 *\brief		Retrieves the overlay type
		 *\return		The value
		 *\~french
		 *\brief		Récupère le type de l'incrustation
		 *\return		La valeur
		 */
		inline eOVERLAY_TYPE GetType()const { return m_pOverlayCategory->GetType(); }
		/**
		 *\~english
		 *\brief		Retrieves the parent overlay
		 *\return		The value
		 *\~french
		 *\brief		Récupère l'incrustation parente
		 *\return		La valeur
		 */
		inline OverlaySPtr GetParent()const { return m_pParent.lock(); }
		/**
		 *\~english
		 *\brief		Retrieves the material
		 *\return		The value
		 *\~french
		 *\brief		Récupère le matériau
		 *\return		La valeur
		 */
		inline MaterialSPtr GetMaterial()const { return m_pOverlayCategory->GetMaterial(); }
		/**
		 *\~english
		 *\brief		Retrieves the overlay position
		 *\return		The value
		 *\~french
		 *\brief		Récupère la position de l'incrustation
		 *\return		La valeur
		 */
		inline Castor::Point2r & GetPosition() { return m_pOverlayCategory->GetPosition(); }
		/**
		 *\~english
		 *\brief		Retrieves the overlay size
		 *\return		The value
		 *\~french
		 *\brief		Récupère la taille de l'incrustation
		 *\return		La valeur
		 */
		inline Castor::Point2r & GetSize() { return m_pOverlayCategory->GetSize(); }
		/**
		 *\~english
		 *\brief		Retrieves the parent scene
		 *\return		The value
		 *\~french
		 *\brief		Récupère la scène parente
		 *\return		La valeur
		 */
		inline SceneSPtr GetScene()const { return m_pScene.lock(); }
		/**
		 *\~english
		 *\brief		Retrieves the Z index
		 *\return		The value
		 *\~french
		 *\brief		Récupère le Z index
		 *\return		La valeur
		 */
		inline int GetZIndex()const { return m_pOverlayCategory->GetZIndex(); }
		/**
		 *\~english
		 *\brief		Retrieves the childs count
		 *\return		The value
		 *\~french
		 *\brief		Récupère le nombre d'enfants
		 *\return		La valeur
		 */
		inline uint32_t GetChildsCount()const { return uint32_t( m_mapOverlays.size() ); }
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the first child
		 *\return		The value
		 *\~french
		 *\brief		Récupère un itérateur sur le premier enfant
		 *\return		La valeur
		 */
		inline iterator Begin() { return m_mapOverlays.begin(); }
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the first child
		 *\return		The value
		 *\~french
		 *\brief		Récupère un itérateur sur le premier enfant
		 *\return		La valeur
		 */
		inline const_iterator Begin()const { return m_mapOverlays.begin(); }
		/**
		 *\~english
		 *\brief		Retrieves an iterator to after the last child
		 *\return		The value
		 *\~french
		 *\brief		Récupère un itérateur sur après le dernier enfant
		 *\return		La valeur
		 */
		inline iterator End() { return m_mapOverlays.end(); }
		/**
		 *\~english
		 *\brief		Retrieves an iterator to after the last child
		 *\return		The value
		 *\~french
		 *\brief		Récupère un itérateur sur après le dernier enfant
		 *\return		La valeur
		 */
		inline const_iterator End()const { return m_mapOverlays.end(); }
		/**
		 *\~english
		 *\brief		Retrieves the overlay category
		 *\return		The value
		 *\~french
		 *\brief		Récupère la catégorie de l'overlay
		 *\return		La valeur
		 */
		inline OverlayCategorySPtr GetOverlayCategory()const { return m_pOverlayCategory; }
		/**
		 *\~english
		 *\brief		Sets the visibility status
		 *\param[in]	val	The new value
		 *\~french
		 *\brief		Définit le statut de visibilité
		 *\param[in]	val	La nouvelle valeur
		 */
		inline void SetVisible( bool val ) { m_pOverlayCategory->SetVisible( val); }
		/**
		 *\~english
		 *\brief		Sets the Z index
		 *\param[in]	p_iZIndex	The new value
		 *\~french
		 *\brief		Définit le Z index
		 *\param[in]	p_iZIndex	La nouvelle valeur
		 */
		inline void SetZIndex( int p_iZIndex ) { m_pOverlayCategory->SetZIndex( p_iZIndex); }
		/**
		 *\~english
		 *\brief		Sets the material
		 *\param[in]	p_pMaterial	The new value
		 *\~french
		 *\brief		Définit le matériau
		 *\param[in]	p_pMaterial	La nouvelle valeur
		 */
		inline void SetMaterial( MaterialSPtr p_pMaterial ) { m_pOverlayCategory->SetMaterial( p_pMaterial); }
		/**
		 *\~english
		 *\brief		Sets the position of the overlay
		 *\param[in]	p_ptPosition	The new position, relative to parent
		 *\~french
		 *\brief		Sets the position of the overlay
		 *\param[in]	p_ptPosition	The new position, relative to parent
		 */
		inline void SetPosition( Castor::Point2r const & p_ptPosition) { m_pOverlayCategory->SetPosition( m_pParent.lock(), p_ptPosition ); }
		/**
		 *\~english
		 *\brief		Sets the size of the overlay
		 *\param[in]	p_ptSize	The new size, relative to parent
		 *\~french
		 *\brief		Sets the size of the overlay
		 *\param[in]	p_ptSize	The new size, relative to parent
		 */
		inline void SetSize( Castor::Point2r const & p_ptSize) { m_pOverlayCategory->SetSize( m_pParent.lock(), p_ptSize ); }
		/**
		 *\~english
		 *\brief		Sets the name of the overlay
		 *\param[in]	p_strName	The new name
		 *\~french
		 *\brief		Sets the name of the overlay
		 *\param[in]	p_strName	The new name
		 */
		inline void SetName( Castor::String const & p_strName ) { m_strName = p_strName; }
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		25/08/2010
	\~english
	\brief		A simple rectangular overlay
	\~french
	\brief		Une simple incrustation rectangulaire
	*/
	class C3D_API PanelOverlay : public OverlayCategory
	{
	public:
		/*!
		\author 	Sylvain DOREMUS
		\date 		14/02/2010
		\~english
		\brief		PanelOverlay loader
		\remark		Loads and saves overlays from/into a file
		\~french
		\brief		PanelOverlay loader
		\remark		Charge et enregistre les incrustations dans des fichiers
		*/
		class C3D_API TextLoader : public OverlayCategory::TextLoader
		{
		public:
			/**
			 *\~english
			 *\brief		Saves an overlay into a text file
			 *\param[in]	p_file		the file to save the overlay in
			 *\param[in]	p_overlay	the overlay to save
			 *\return		\p true if everything is OK
			 *\~french
			 *\brief		Sauvegarde l'incrustation donnée dans un fichier texte
			 *\param[in]	p_file		Le fichier où enregistrer l'incrustation
			 *\param[in]	p_overlay	L'incrustation à enregistrer
			 *\return		\p true si tout s'est bien passé
			 */
			virtual bool operator ()( PanelOverlay const & p_overlay, Castor::TextFile & p_file);
		};

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		PanelOverlay();
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~PanelOverlay();
		/**
		 *\~english
		 *\brief		Creation function, used by the factory
		 *\return		An overlay
		 *\~french
		 *\brief		Fonction de création utilisée par la fabrique
		 *\return		Un overlay
		 */
		static OverlayCategorySPtr Create();
		/**
		 *\~english
		 *\brief		Draws the overlay
		 *\~french
		 *\brief		Desisne l'incrustation
		 */
		virtual void Render();
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		25/08/2010
	\~english
	\brief		A rectangular overlay with a border
	\remark		Uses a spceific material for the border
	\~french
	\brief		Une incrustation rectangulaire avec un bord
	\remark		Utilise un matériau spécifique pour le bord
	*/
	class C3D_API BorderPanelOverlay : public OverlayCategory
	{
	public:
		/*!
		\author 	Sylvain DOREMUS
		\date 		14/02/2010
		\~english
		\brief		BorderPanelOverlay loader
		\remark		Loads and saves overlays from/into a file
		\~french
		\brief		BorderPanelOverlay loader
		\remark		Charge et enregistre les incrustations dans des fichiers
		*/
		class C3D_API TextLoader : public OverlayCategory::TextLoader
		{
		public:
			/**
			 *\~english
			 *\brief		Saves an overlay into a text file
			 *\param[in]	p_file		the file to save the overlay in
			 *\param[in]	p_overlay	the overlay to save
			 *\return		\p true if everything is OK
			 *\~french
			 *\brief		Sauvegarde l'incrustation donnée dans un fichier texte
			 *\param[in]	p_file		Le fichier où enregistrer l'incrustation
			 *\param[in]	p_overlay	L'incrustation à enregistrer
			 *\return		\p true si tout s'est bien passé
			 */
			virtual bool operator ()( Castor3D::BorderPanelOverlay const & p_overlay, Castor::TextFile & p_file);
		};

	protected:
		//!\~english	The border material	\~french	Le matériau des bords
		MaterialWPtr m_pBorderMaterial;
		//!\~english	The border size	\~french	La taille des bords
		Castor::Point4r m_ptBorderSize;
		//!\~english	The border material name	\~french	Le nom du matériau des bords
		Castor::String m_strBorderMatName;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		BorderPanelOverlay();
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~BorderPanelOverlay();
		/**
		 *\~english
		 *\brief		Creation function, used by the factory
		 *\return		An overlay
		 *\~french
		 *\brief		Fonction de création utilisée par la fabrique
		 *\return		Un overlay
		 */
		static OverlayCategorySPtr Create();
		/**
		 *\~english
		 *\brief		Draws the overlay
		 *\~french
		 *\brief		Desisne l'incrustation
		 */
		virtual void Render();
		/**
		 *\~english
		 *\brief		Sets the border material
		 *\param[in]	p_pMaterial	The new value
		 *\~french
		 *\brief		Définit le matériau des bords
		 *\param[in]	p_pMaterial	La nouvelle valeur
		 */
		void SetBorderMaterial( MaterialSPtr p_pMaterial );
		/**
		 *\~english
		 *\brief		Retrieves the left border thickness
		 *\return		The value
		 *\~french
		 *\brief		Récupère l'épaisseur du bord gauche
		 *\return		La valeur
		 */
		inline real GetLeftBorderSize()const { return m_ptBorderSize[0]; }
		/**
		 *\~english
		 *\brief		Retrieves the right border thickness
		 *\return		The value
		 *\~french
		 *\brief		Récupère l'épaisseur du bord droit
		 *\return		La valeur
		 */
		inline real GetRightBorderSize()const { return m_ptBorderSize[1]; }
		/**
		 *\~english
		 *\brief		Retrieves the top border thickness
		 *\return		The value
		 *\~french
		 *\brief		Récupère l'épaisseur du bord haut
		 *\return		La valeur
		 */
		inline real GetTopBorderSize()const { return m_ptBorderSize[2]; }
		/**
		 *\~english
		 *\brief		Retrieves the bottom border thickness
		 *\return		The value
		 *\~french
		 *\brief		Récupère l'épaisseur du bord bas
		 *\return		La valeur
		 */
		inline real GetBottomBorderSize()const { return m_ptBorderSize[3]; }
		/**
		 *\~english
		 *\brief		Retrieves the borders thicknesses
		 *\return		The value
		 *\~french
		 *\brief		Récupère les épaisseurs des bords
		 *\return		La valeur
		 */
		inline Castor::Point4r const & GetBorderSize()const { return m_ptBorderSize; }
		/**
		 *\~english
		 *\brief		Retrieves the borders thicknesses
		 *\return		The value
		 *\~french
		 *\brief		Récupère les épaisseurs des bords
		 *\return		La valeur
		 */
		inline Castor::Point4r & GetBorderSize() { return m_ptBorderSize; }
		/**
		 *\~english
		 *\brief		Retrieves the border material
		 *\return		The value
		 *\~french
		 *\brief		Récupère le matériau des bords
		 *\return		La valeur
		 */
		inline MaterialSPtr GetBorderMaterial()const { return m_pBorderMaterial.lock(); }
		/**
		 *\~english
		 *\brief		Sets the left border thickness
		 *\param[in]	p_fSize	The new value
		 *\~french
		 *\brief		Définit l'épaisseur du bord gauche
		 *\param[in]	p_fSize	La nouvelle valeur
		 */
		inline void SetLeftBorderSize( real p_fSize ) { m_ptBorderSize[0] = p_fSize; }
		/**
		 *\~english
		 *\brief		Sets the right border thickness
		 *\param[in]	p_fSize	The new value
		 *\~french
		 *\brief		Définit l'épaisseur du bord droit
		 *\param[in]	p_fSize	La nouvelle valeur
		 */
		inline void SetRightBorderSize( real p_fSize ) { m_ptBorderSize[1] = p_fSize; }
		/**
		 *\~english
		 *\brief		Sets the top border thickness
		 *\param[in]	p_fSize	The new value
		 *\~french
		 *\brief		Définit l'épaisseur du bord haut
		 *\param[in]	p_fSize	La nouvelle valeur
		 */
		inline void SetTopBorderSize( real p_fSize ) { m_ptBorderSize[2] = p_fSize; }
		/**
		 *\~english
		 *\brief		Sets the bottom border thickness
		 *\param[in]	p_fSize	The new value
		 *\~french
		 *\brief		Définit l'épaisseur du bord bas
		 *\param[in]	p_fSize	La nouvelle valeur
		 */
		inline void SetBottomBorderSize( real p_fSize ) { m_ptBorderSize[3] = p_fSize; }
		/**
		 *\~english
		 *\brief		Sets the borders thicknesses
		 *\param[in]	p_ptSize	The new value
		 *\~french
		 *\brief		Définit l'épaisseur des bords
		 *\param[in]	p_ptSize	La nouvelle valeur
		 */
		inline void SetBorderSize( Castor::Point4r const & p_ptSize ) { m_ptBorderSize = p_ptSize; }
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		25/08/2010
	\~english
	\brief		An overlay with a text
	\~french
	\brief		Une incrustation avec du texte
	*/
	class C3D_API TextOverlay : public OverlayCategory
	{
	public:
		/*!
		\author 	Sylvain DOREMUS
		\date 		14/02/2010
		\~english
		\brief		TextOverlay loader
		\remark		Loads and saves overlays from/into a file
		\~french
		\brief		TextOverlay loader
		\remark		Charge et enregistre les incrustations dans des fichiers
		*/
		class C3D_API TextLoader : public OverlayCategory::TextLoader
		{
		public:
			/**
			 *\~english
			 *\brief		Saves an overlay into a text file
			 *\param[in]	p_file		the file to save the overlay in
			 *\param[in]	p_overlay	the overlay to save
			 *\return		\p true if everything is OK
			 *\~french
			 *\brief		Sauvegarde l'incrustation donnée dans un fichier texte
			 *\param[in]	p_file		Le fichier où enregistrer l'incrustation
			 *\param[in]	p_overlay	L'incrustation à enregistrer
			 *\return		\p true si tout s'est bien passé
			 */
			virtual bool operator ()( Castor3D::TextOverlay const & p_overlay, Castor::TextFile & p_file);
		};

	protected:
		//!\~english	The overlay caption	\~french	Le texte de l'incrustation
		Castor::String m_strCaption;
		//!\~english	The text font	\~french	La police du texte
		TextFontSPtr m_pFont;
		//!\~english	The font name	\~french	Le nom de la police
		Castor::String m_strFontName;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		TextOverlay();
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~TextOverlay();
		/**
		 *\~english
		 *\brief		Creation function, used by the factory
		 *\return		An overlay
		 *\~french
		 *\brief		Fonction de création utilisée par la fabrique
		 *\return		Un overlay
		 */
		static OverlayCategorySPtr Create();
		/**
		 *\~english
		 *\brief		Draws the overlay
		 *\~french
		 *\brief		Desisne l'incrustation
		 */
		virtual void Render();
		/**
		 *\~english
		 *\brief		Sets the text font
		 *\param[in]	p_strFont	The new value
		 *\~french
		 *\brief		Définit la police du texte
		 *\param[in]	p_strFont	La nouvelle valeur
		 */
		void SetFont( Castor::String const & p_strFont );
		/**
		 *\~english
		 *\brief		Sets the material
		 *\param[in]	p_pMaterial	The new value
		 *\~french
		 *\brief		Définit le matériau
		 *\param[in]	p_pMaterial	La nouvelle valeur
		 */
		void SetMaterial( MaterialSPtr p_pMaterial );
		/**
		 *\~english
		 *\brief		Retrieves the font name
		 *\return		The value
		 *\~french
		 *\brief		Récupère le nom de la police
		 *\return		La valeur
		 */
		Castor::String const & GetFontName()const;
		/**
		 *\~english
		 *\brief		Retrieves the overlay text
		 *\return		The value
		 *\~french
		 *\brief		Récupère le texte de l'incrustation
		 *\return		La valeur
		 */
		inline Castor::String GetCaption()const { return m_strCaption; }
		/**
		 *\~english
		 *\brief		Sets the overlay text
		 *\param[in]	p_strCaption	The new value
		 *\~french
		 *\brief		Définit le texte de l'incrustation
		 *\param[in]	p_strCaption	La nouvelle valeur
		 */
		inline void SetCaption( Castor::String const & p_strCaption ) { m_strCaption = p_strCaption; }
		/**
		 *\~english
		 *\brief		Displays the text with given colour
		 *\param[in]	p_clrColour		The colour
		 *\~french
		 *\brief		Affiche le texte avec la couleur donnée
		 *\param[in]	p_clrColour		La couleur
		 */
		void Draw( Castor::Colour const & p_clrColour, ShaderProgramBaseSPtr p_pProgram );

	private:
		void _updateMaterial();
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		09/02/2010
	\~english
	\brief		The overlay renderer
	\~french
	\brief		Le renderer d'incrustation
	*/
	class C3D_API OverlayRenderer
	{
	protected:
		struct stVertex
		{
			real coords[3];
			real texture[2];
		};
		//!\~english	Pointer over geometry vertex buffer	\~french	Pointeur sur le tampon de sommets
		VertexBufferWPtr m_pVertexBuffer;
		//!\~english	Geometry indices buffer	\~french	Le tampon de sommets
		IndexBufferWPtr m_pIndexBuffer;
		//!\~english	Geometry buffers	\~french	Tampons de la géometrie
		GeometryBuffersSPtr m_pGeometryBuffer;
		//!\~english	The buffer elements declaration	\~french	La déclaration des éléments du tampon
		BufferDeclarationSPtr m_pDeclaration;
		//!\~english	The render system	\~french	Le render system
		RenderSystem * m_pRenderSystem;
		//!	4 * [3(vertex position) 2(texture coordinates)]
		stVertex m_pBuffer[6];
		//!\~english	Vertex array (quad definition)	\~french	Tableau de sommets (définition du quad)
		std::array< Castor3D::BufferElementGroupSPtr, 6 > m_arrayVertex;
		//!\~english	The shader programs used to render a panel (used for borders too)	\~french	Les programmes de shader utilisés pour rendre un panneau (utilisé pour les bords aussi)
		std::map< uint32_t, ShaderProgramBaseSPtr > m_mapPanelPrograms;
		//!\~english	The shader programs used to render texts	\~french	Les programmes de shader utilisés pour rendre les textes
		std::map< uint32_t, ShaderProgramBaseSPtr > m_mapTextPrograms;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		OverlayRenderer( RenderSystem * p_pRenderSystem );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~OverlayRenderer();
		/**
		 *\~english
		 *\brief		Initialises the buffers
		 *\~french
		 *\brief		Initialise les tampons
		 */
		void Initialise();
		/**
		 *\~english
		 *\brief		Flushes the renderer
		 *\~french
		 *\brief		Nettoie le renderer
		 */
		void Cleanup();
		/**
		 *\~english
		 *\brief		Function to draw a PanelOverlay
		 *\~french
		 *\brief		Fonction de dessin d'un PanelOverlay
		 */
		void DrawPanel( PanelOverlay & p_pPanelOverlay );
		/**
		 *\~english
		 *\brief		Function to draw a BorderPanelOverlay
		 *\~french
		 *\brief		Fonction de dessin d'un BorderPanelOverlay
		 */
		void DrawBorderPanel( BorderPanelOverlay & p_pBorderPanelOverlay );
		/**
		 *\~english
		 *\brief		Function to draw a TextOverlay
		 *\~french
		 *\brief		Fonction de dessin d'un TextOverlay
		 */
		void DrawText( TextOverlay & p_pTextPanelOverlay );
		/**
		 *\~english
		 *\brief		Begins the overlays rendering
		 *\param[in]	p_size	The render window size
		 *\~french
		 *\brief		Commence le rendu des incrustations
		 *\param[in]	p_size	La taille de la fenêtre de rendu
		 */
		void BeginRender( Castor::Size const & p_size );
		/**
		 *\~english
		 *\brief		Ends the overlays rendering
		 *\~french
		 *\brief		Termine le rendu des incrustations
		 */
		void EndRender();

	protected:
		/**
		 *\~english
		 *\brief		Retrieves a program compiled using given flags
		 *\param[in]	p_uiFlags	Bitwise ORed eTEXTURE_CHANNEL
		 *\return		The program
		 *\~french
		 *\brief		Récupère un programme compilé en utilisant les indicateurs donnés
		 *\param[in]	p_uiFlags	Combinaison de eTEXTURE_CHANNEL
		 *\return		Le programme
		 */
		virtual ShaderProgramBaseSPtr DoGetProgram( uint32_t p_uiFlags )=0;
		/**
		 *\~english
		 *\brief		Initialises the buffers
		 *\~french
		 *\brief		Initialise les tampons
		 */
		virtual void DoInitialise()=0;
		/**
		 *\~english
		 *\brief		Flushes the renderer
		 *\~french
		 *\brief		Nettoie le renderer
		 */
		virtual void DoCleanup()=0;
		/**
		 *\~english
		 *\brief		Retrieves a panel program compiled using given flags
		 *\param[in]	p_uiFlags	Bitwise ORed eTEXTURE_CHANNEL
		 *\return		The program
		 *\~french
		 *\brief		Récupère un programme de panneau compilé en utilisant les indicateurs donnés
		 *\param[in]	p_uiFlags	Combinaison de eTEXTURE_CHANNEL
		 *\return		Le programme
		 */
		ShaderProgramBaseSPtr GetPanelProgram( uint32_t p_uiFlags );
		/**
		 *\~english
		 *\brief		Retrieves a text program compiled using given flags
		 *\param[in]	p_uiFlags	Bitwise ORed eTEXTURE_CHANNEL
		 *\return		The program
		 *\~french
		 *\brief		Récupère un programme de texte compilé en utilisant les indicateurs donnés
		 *\param[in]	p_uiFlags	Combinaison de eTEXTURE_CHANNEL
		 *\return		Le programme
		 */
		ShaderProgramBaseSPtr GetTextProgram( uint32_t p_uiFlags );
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		09/02/2010
	\version	0.1
	\~english
	\brief		Overlay collection, with additional add and remove functions to manage Z-Index
	\~french
	\brief		Collection d'incrustations, avec des fonctions additionnelles d'ajout et de suppression pour gérer les Z-Index
	*/
	class C3D_API OverlayManager : private OverlayCollection
	{
	public:
		typedef OverlayCollectionIt			iterator;
		typedef OverlayCollectionConstIt	const_iterator;

	private:
		//!\~english	Current available Z index	\~french	Z index disponible
		int m_iCurrentZIndex;
		//!\~english	The overlays, sorted by Z index	\~french	Les incrustations, triées par Z index
		OverlayPtrIntMap m_mapOverlaysByZIndex;
		//!\~english	The engine	\~french	Le moteur
		Engine * m_pEngine;
		//!\~english	The overlay renderer	\~french	le renderer d'incrustation
		OverlayRendererSPtr m_pRenderer;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		OverlayManager( Engine * p_pEngine );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~OverlayManager();
		/**
		 *\~english
		 *\brief		Clears all overlays lists
		 *\~french
		 *\brief		Vide les listes d'incrustations
		 */
		void ClearOverlays();
		/**
		 *\~english
		 *\brief		Add an overlay to the lists, given it's name
		 *\param[in]	p_strName	The overlay name
		 *\param[in]	p_pOverlay	The overlay
		 *\param[in]	p_pParent	The parent overlay
		 *\~french
		 *\brief		Ajoute une incrustation aux listes, selon son nom
		 *\param[in]	p_strName	Le nom
		 *\param[in]	p_pOverlay	L'incrustation
		 *\param[in]	p_pParent	L'incrustation parente
		 */
		void AddOverlay( Castor::String const & p_strName, OverlaySPtr p_pOverlay, OverlaySPtr p_pParent );
		/**
		 *\~english
		 *\brief		Add an overlay to the lists, given it's z-index
		 *\param[in]	p_iZIndex	The wanted z-index
		 *\param[in]	p_pOverlay	The overlay
		 *\~french
		 *\brief		Ajoute une incrustation aux listes, selon son z-index
		 *\param[in]	p_iZIndex	Le z-index voulu
		 *\param[in]	p_pOverlay	L'incrustation
		 */
		void AddOverlay( int p_iZIndex, OverlaySPtr p_pOverlay );
		/**
		 *\~english
		 *\brief		Checks if an overlay exists at given z-index
		 *\param[in]	p_iZIndex	The z-index
		 *\return		\p true if an overlay is defined at the given z-index
		 *\~french
		 *\brief		Vérifie si une incrustation existe au z-index donné
		 *\param[in]	p_iZIndex	Le z-index
		 *\return		\p true si un overlayest défini au z-index donné
		 */
		bool HasOverlay( int p_iZIndex );
		/**
		 *\~english
		 *\brief		Retrieves the overlay with the given name
		 *\param[in]	p_strName	The name
		 *\return		The overlay, \p nullptr if not found
		 *\~french
		 *\brief		Récupère l'incrustation avec le nom donné
		 *\param[in]	p_strName	Le nom
		 *\return		L'incrustation, \p nullptr si non trouvée
		 */
		OverlaySPtr GetOverlay( Castor::String const & p_strName );
		/**
		 *\~english
		 *\brief		Retrieves the overlay at given z-index
		 *\param[in]	p_iZIndex	The z-index
		 *\return		The overlay, \p nullptr if not found
		 *\~french
		 *\brief		Récupère l'incrustation au z-index donné
		 *\param[in]	p_iZIndex	Le z-index
		 *\return		L'incrustation, \p nullptr si non trouvée
		 */
		OverlaySPtr GetOverlay( int p_iZIndex );
		/**
		 *\~english
		 *\brief		Checks if an overlay with the given name exists
		 *\param[in]	p_strName	The name
		 *\return		\p true if an overlay is defined with given name
		 *\~french
		 *\brief		Vérifie si une incrustation avec le nom donné existe
		 *\param[in]	p_strName	Le nom
		 *\return		\p true Si une incrustation est défini avec le nom donné
		 */
		bool HasOverlay( Castor::String const & p_strName );
		/**
		 *\~english
		 *\brief		Writes overlays in a text file
		 *\param[out]	p_file	The file
		 *\return		\p true if ok
		 *\~french
		 *\brief		Ecrit les overlays dans un fichier texte
		 *\param[out]	p_file	Le fichier
		 *\return		\p true si tout s'est bien passé
		 */
		bool WriteOverlays( Castor::TextFile & p_file )const;
		/**
		 *\~english
		 *\brief		Reads overlays from a text file
		 *\param[in]	p_file	The file
		 *\return		\p true if ok
		 *\~french
		 *\brief		Lit les overlays à partir d'un fichier texte
		 *\param[in]	p_file	Le fichier
		 *\return		\p true si tout s'est bien passé
		 */
		bool ReadOverlays( Castor::TextFile & p_file );
		/**
		 *\~english
		 *\brief		Writes overlays in a binary file
		 *\param[out]	p_file	The file
		 *\return		\p true if ok
		 *\~french
		 *\brief		Ecrit les overlays dans un fichier binaire
		 *\param[out]	p_file	Le fichier
		 *\return		\p true si tout s'est bien passé
		 */
		bool SaveOverlays( Castor::BinaryFile & p_file )const;
		/**
		 *\~english
		 *\brief		Reads overlays from a binary file
		 *\param[in]	p_file	The file
		 *\return		\p true if ok
		 *\~french
		 *\brief		Lit les overlays à partir d'un fichier binaire
		 *\param[in]	p_file	Le fichier
		 *\return		\p true si tout s'est bien passé
		 */
		bool LoadOverlays( Castor::BinaryFile & p_file );
		/**
		 *\~english
		 *\brief		Initialises or cleans up the OverlayRenderer, according to engine rendering status
		 *\~french
		 *\brief		Initialise ou nettoie l'OverlayRenderer, selon le statut du rendu
		 */
		void Update();
		/**
		 *\~english
		 *\brief		Retrieves the overlay renderer
		 *\return		The overlay renderer
		 *\~french
		 *\brief		Récupère le renderer d'incrustation 
		 *\return		Le renderer d'incrustation
		 */
		OverlayRendererSPtr GetRenderer()const { return m_pRenderer; }
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the first overlay
		 *\return		The value
		 *\~french
		 *\brief		Récupère un itérateur sur la première incrustation
		 *\return		La valeur
		 */
		inline OverlayPtrIntMap::iterator Begin() { return m_mapOverlaysByZIndex.begin(); }
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the first overlay
		 *\return		The value
		 *\~french
		 *\brief		Récupère un itérateur sur la première incrustation
		 *\return		La valeur
		 */
		inline OverlayPtrIntMap::const_iterator Begin()const { return m_mapOverlaysByZIndex.begin(); }
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the first overlay
		 *\return		The value
		 *\~french
		 *\brief		Récupère un itérateur sur la première incrustation
		 *\return		La valeur
		 */
		inline OverlayPtrIntMap::reverse_iterator RBegin() { return m_mapOverlaysByZIndex.rbegin(); }
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the first overlay
		 *\return		The value
		 *\~french
		 *\brief		Récupère un itérateur sur la première incrustation
		 *\return		La valeur
		 */
		inline OverlayPtrIntMap::const_reverse_iterator RBegin()const { return m_mapOverlaysByZIndex.rbegin(); }
		/**
		 *\~english
		 *\brief		Retrieves an iterator to after the last overlay
		 *\return		The value
		 *\~french
		 *\brief		Récupère un itérateur sur après la dernière incrustation
		 *\return		La valeur
		 */
		inline OverlayPtrIntMap::iterator End() { return m_mapOverlaysByZIndex.end(); }
		/**
		 *\~english
		 *\brief		Retrieves an iterator to after the last overlay
		 *\return		The value
		 *\~french
		 *\brief		Récupère un itérateur sur après la dernière incrustation
		 *\return		La valeur
		 */
		inline OverlayPtrIntMap::const_iterator End()const { return m_mapOverlaysByZIndex.end(); }
		/**
		 *\~english
		 *\brief		Retrieves an iterator to after the last overlay
		 *\return		The value
		 *\~french
		 *\brief		Récupère un itérateur sur après la dernière incrustation
		 *\return		La valeur
		 */
		inline OverlayPtrIntMap::reverse_iterator REnd() { return m_mapOverlaysByZIndex.rend(); }
		/**
		 *\~english
		 *\brief		Retrieves an iterator to after the last overlay
		 *\return		The value
		 *\~french
		 *\brief		Récupère un itérateur sur après la dernière incrustation
		 *\return		La valeur
		 */
		inline OverlayPtrIntMap::const_reverse_iterator REnd()const { return m_mapOverlaysByZIndex.rend(); }
		/**
		 *\~english
		 *\brief		Begins the overlays rendering
		 *\param[in]	p_size	The render window size
		 *\~french
		 *\brief		Commence le rendu des incrustations
		 *\param[in]	p_size	La taille de la fenêtre de rendu
		 */
		void BeginRendering( Castor::Size const & p_size );
		/**
		 *\~english
		 *\brief		Ends the overlays rendering
		 *\~french
		 *\brief		Termine le rendu des incrustations
		 */
		void EndRendering();

		using OverlayCollection::lock;
		using OverlayCollection::unlock;
	};
	typedef OverlayManager::iterator		OverlayManagerIt;
	typedef OverlayManager::const_iterator	OverlayManagerConstIt;
}

#pragma warning( pop )

#endif
