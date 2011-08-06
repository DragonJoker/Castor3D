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
#include "Renderable.hpp"

namespace Castor
{	namespace Resources
{
	//! Overlay loader
	/*!
	Loads and saves overlays from/into a file
	\author Sylvain DOREMUS
	\date 14/02/2010
	*/
	template <> class C3D_API Loader<Castor3D::OverlayCategory>
	{
	public:
		/**
		 * Saves an overlay into a text file
		 *@param p_file : [in] the file to save the overlay in
		 *@param p_overlay : [in] the overlay to save
		 */
		static bool Write( const Castor3D::OverlayCategory & p_overlay, Utils::File & p_file);
	};
	//! PanelOverlay loader
	/*!
	Loads and saves overlays from/into a file
	\author Sylvain DOREMUS
	\date 14/02/2010
	*/
	template <> class C3D_API Loader<Castor3D::PanelOverlay>
	{
	public:
		/**
		 * Saves an overlay into a text file
		 *@param p_file : [in] the file to save the overlay in
		 *@param p_overlay : [in] the overlay to save
		 */
		static bool Write( const Castor3D::PanelOverlay & p_overlay, Utils::File & p_file);
	};
	//! BorderPanelOverlay loader
	/*!
	Loads and saves overlays from/into a file
	\author Sylvain DOREMUS
	\date 14/02/2010
	*/
	template <> class C3D_API Loader<Castor3D::BorderPanelOverlay>
	{
	public:
		/**
		 * Saves an overlay into a text file
		 *@param p_file : [in] the file to save the overlay in
		 *@param p_overlay : [in] the overlay to save
		 */
		static bool Write( const Castor3D::BorderPanelOverlay & p_overlay, Utils::File & p_file);
	};
	//! TextOverlay loader
	/*!
	Loads and saves overlays from/into a file
	\author Sylvain DOREMUS
	\date 14/02/2010
	*/
	template <> class C3D_API Loader<Castor3D::TextOverlay>
	{
	public:
		/**
		 * Saves an overlay into a text file
		 *@param p_file : [in] the file to save the overlay in
		 *@param p_overlay : [in] the overlay to save
		 */
		static bool Write( const Castor3D::TextOverlay & p_overlay, Utils::File & p_file);
	};
}
}

namespace Castor3D
{
	class OverlayCategory : public Serialisable, public Clonable<OverlayCategory>
	{
	private:
		Overlay			*	m_pOverlay;			//!< The parent overlay, if any
		Point2r				m_ptPosition;		//!< The relative position (to parent or screen)
		Point2r				m_ptSize;			//!< The relative size (to parent or screen)
		bool				m_bVisible;			//!< The visibility
		MaterialPtr			m_pMaterial;		//!< The material used by the overlay
		int					m_iCurrentZIndex;	//!< The z index of next children to add
		String				m_strMatName;		//!< The material name
		eOVERLAY_TYPE		m_eType;

	public:
		OverlayCategory( eOVERLAY_TYPE p_eType);
		virtual ~OverlayCategory();
		/**
		 * Draws the overlay
		 */
		virtual void Render( OverlayRendererPtr p_pRenderer, ePRIMITIVE_TYPE p_displayMode)=0;

		void SetPosition( Overlay * p_pParent, const Point2r & p_ptPosition);
		void SetSize( Overlay * p_pParent, const Point2r & p_ptSize);
		void SetMaterial( MaterialPtr p_pMaterial);

		inline const Point2r &	GetSize		()const	{ return m_ptSize; }
		inline const Point2r &	GetPosition	()const	{ return m_ptPosition; }
		inline Point2r &		GetSize		()		{ return m_ptSize; }
		inline Point2r &		GetPosition	()		{ return m_ptPosition; }
		inline eOVERLAY_TYPE	GetType		()const	{ return m_eType; }
		inline bool				IsVisible	()const { return m_bVisible; }
		inline MaterialPtr		GetMaterial	()const { return m_pMaterial; }
		inline int				GetZIndex	()const	{ return m_iCurrentZIndex; }
		inline Overlay *		GetOverlay	()const { return m_pOverlay; }

		inline void SetOverlay	( Overlay * val)		{ m_pOverlay = val; }
		inline void SetVisible	( bool val)				{ m_bVisible = val; }
		inline void SetPosition	( const Point2r & val)	{ m_ptPosition = val; }
		inline void SetSize		( const Point2r & val)	{ m_ptSize = val; }
		inline void SetZIndex	( int p_iZIndex)		{ m_iCurrentZIndex = p_iZIndex; }

		DECLARE_SERIALISE_MAP()
	};
	//! The overlay abstract class C3D_API
	/*!
	An overlay is a 2D element which is displayed at foreground
	\author Sylvain DOREMUS
	\date 25/08/2010
	*/
	class C3D_API Overlay : public Serialisable, public Renderable<Overlay, OverlayRenderer>
	{
	protected:
		typedef OverlayPtrIntMap::iterator iterator;
		typedef OverlayPtrIntMap::const_iterator const_iterator;
		String						m_strName;			//!< The overlay name
		Overlay					*	m_pParent;			//!< The parent overlay, if any
		OverlayPtrIntMap			m_mapOverlays;		//!< The childs, if any
		OverlayCategoryPtr			m_pOverlayCategory;
		Scene					*	m_pScene;
		mutable OverlayPtrArray		m_arrayOverlays;	//!< The childs, if any

	public:
		/**
		 * Constructor
		 *@param p_eType : [in] The overlay type
		 */
		Overlay( eOVERLAY_TYPE p_eType);
		/**
		 * Constructor
		 *@param p_pScene : [in] The scene holding the overlay
		 *@param p_strName : [in] The overlay name
		 *@param p_pParent : [in] The parent overlay (if any)
		 *@param p_eType : [in] The overlay type
		 */
		Overlay( Scene * p_pScene, String const & p_strName, Overlay * p_pParent, eOVERLAY_TYPE p_eType);
		/**
		 * Destructor
		 */
		virtual ~Overlay();
		/**
		 * Registers every overlay type
		 */
		static void Register();
		/**
		 * Draws the overlay
		 */
		virtual void Render( ePRIMITIVE_TYPE p_displayMode);
		/**
		 * Adds a child to the overlay
		 *@param p_pOverlay : [in] The overlay to add
		 *@param p_iZIndex : [in] The wanted Z Index for the child
		 *@return true if successful, false if not
		 */
		bool AddChild( OverlayPtr p_pOverlay, int p_iZIndex);

		static int GetAvailableZIndex( Overlay * p_pParent, Scene * p_pScene, int p_iZIndex);

		/**@name Accessors */
		//@{
		inline String				GetName				()const { return m_strName; }
		inline const Point2r &		GetPosition			()const { return m_pOverlayCategory->GetPosition(); }
		inline const Point2r &		GetSize				()const { return m_pOverlayCategory->GetSize(); }
		inline bool					IsVisible			()const { return m_pOverlayCategory->IsVisible(); }
		inline eOVERLAY_TYPE		GetType				()const { return m_pOverlayCategory->GetType(); }
		inline Overlay *			GetParent			()const { return m_pParent; }
		inline MaterialPtr			GetMaterial			()const { return m_pOverlayCategory->GetMaterial(); }
		inline Point2r &			GetPosition			()		{ return m_pOverlayCategory->GetPosition(); }
		inline Point2r &			GetSize				()		{ return m_pOverlayCategory->GetSize(); }
		inline Scene *				GetScene			()const	{ return m_pScene; }
		inline int					GetZIndex			()const	{ return m_pOverlayCategory->GetZIndex(); }
		inline size_t				GetChildsCount		()const	{ return m_mapOverlays.size(); }
		inline iterator				Begin				()		{ return m_mapOverlays.begin(); }
		inline const_iterator		Begin				()const { return m_mapOverlays.begin(); }
		inline const_iterator		End					()const { return m_mapOverlays.end(); }
		inline OverlayCategoryPtr	GetOverlayCategory	()const { return m_pOverlayCategory; }

		inline void SetVisible	( bool val)						{ m_pOverlayCategory->SetVisible( val); }
		inline void SetZIndex	( int p_iZIndex)				{ m_pOverlayCategory->SetZIndex( p_iZIndex); }
		inline void SetMaterial( MaterialPtr p_pMaterial)		{ m_pOverlayCategory->SetMaterial( p_pMaterial); }
		/**
		 * Sets the position of the overlay
		 *@param p_ptPosition : [in] The new position, relative to parent
		 */
		inline void SetPosition( const Point2r & p_ptPosition) { m_pOverlayCategory->SetPosition( m_pParent, p_ptPosition); }
		/**
		 * Sets the size of the overlay
		 *@param p_ptSize : [in] The new size, relative to parent
		 */
		inline void SetSize( const Point2r & p_ptSize)			{ m_pOverlayCategory->SetSize( m_pParent, p_ptSize); }
		//@}

		DECLARE_SERIALISE_MAP()
		DECLARE_POST_SERIALISE()
		DECLARE_PRE_UNSERIALISE()
		DECLARE_POST_UNSERIALISE()
	};
	//! A simple overlay area
	/*!
	Describes an actually rectangular overlay. Uses a material so it can contain an image
	\author Sylvain DOREMUS
	\date 25/08/2010
	*/
	class C3D_API PanelOverlay : public OverlayCategory
	{
	public:
		/**
		 * Constructor
		 */
		PanelOverlay();
		/**
		 * Destructor
		 */
		virtual ~PanelOverlay();
		/**
		 * Clone function, used by the factory
		 */
		OverlayCategoryPtr Clone();
		/**
		 * Draws the overlay
		 */
		virtual void Render( OverlayRendererPtr p_pRenderer, ePRIMITIVE_TYPE p_displayMode);
	};
	//! A simple overlay bordered area
	/*!
	Describes an actually rectangular overlay with a border. Uses a material for the panel and a material for the border
	\author Sylvain DOREMUS
	\date 25/08/2010
	*/
	class C3D_API BorderPanelOverlay : public OverlayCategory
	{
	protected:
		MaterialPtr m_pBorderMaterial;	//!< The border material
		Point4r m_ptBorderSize;			//!< The border size
		String m_strBorderMatName;

	public:
		/**
		 * Constructor
		 */
		BorderPanelOverlay();
		/**
		 * Destructor
		 */
		virtual ~BorderPanelOverlay();
		/**
		 * Clone function, used by the factory
		 */
		OverlayCategoryPtr Clone();
		/**
		 * Draws the overlay
		 */
		virtual void Render( OverlayRendererPtr p_pRenderer, ePRIMITIVE_TYPE p_displayMode);
		/**
		 * 
		 */
		void SetBorderMaterial( MaterialPtr p_pMaterial);

		/**@name Accessors */
		//@{
		inline real					GetLeftBorderSize	()const { return m_ptBorderSize[0]; }
		inline real					GetRightBorderSize	()const { return m_ptBorderSize[1]; }
		inline real					GetTopBorderSize	()const { return m_ptBorderSize[2]; }
		inline real					GetBottomBorderSize	()const { return m_ptBorderSize[3]; }
		inline Point4r const &		GetBorderSize		()const { return m_ptBorderSize; }
		inline Point4r &			GetBorderSize		()		{ return m_ptBorderSize; }
		inline MaterialPtr			GetBorderMaterial	()const	{ return m_pBorderMaterial; }
		inline void SetLeftBorderSize	( real p_fSize)				{ m_ptBorderSize[0] = p_fSize; }
		inline void SetRightBorderSize	( real p_fSize)				{ m_ptBorderSize[1] = p_fSize; }
		inline void SetTopBorderSize	( real p_fSize)				{ m_ptBorderSize[2] = p_fSize; }
		inline void SetBottomBorderSize	( real p_fSize)				{ m_ptBorderSize[3] = p_fSize; }
		inline void SetBorderSize		( Point4r const & p_ptSize)	{ m_ptBorderSize.copy( p_ptSize); }
		//@}
		DECLARE_SERIALISE_MAP()
		DECLARE_POST_UNSERIALISE()
	};
	//! A text overlay
	/*!
	Describes an overlay containing a text. To be implemented
	\author Sylvain DOREMUS
	\date 25/08/2010
	*/
	class C3D_API TextOverlay : public OverlayCategory
	{
	protected:
		String m_strCaption;		//!< The overlay caption
		TextFontPtr m_pFont;
		String m_strFontName;

	public:
		/**
		 * Constructor
		 */
		TextOverlay();
		/**
		 * Destructor
		 */
		virtual ~TextOverlay();
		/**
		 * Clone function, used by the factory
		 */
		OverlayCategoryPtr Clone();
		/**
		 * Draws the overlay
		 */
		virtual void Render( OverlayRendererPtr p_pRenderer, ePRIMITIVE_TYPE p_displayMode);
		void SetFont( String const & p_strFont);
		void SetMaterial( MaterialPtr p_pMaterial);

	public:
		/**@name Accessors */
		//@{
		inline String		GetCaption	()const { return m_strCaption; }
		inline TextFontPtr	GetFont		()const { return m_pFont; }

		inline void SetCaption	( String const & p_strCaption)	{ m_strCaption = p_strCaption; }

		VertexBufferPtr	GetVertexBuffer	()const;
		IndexBufferPtr	GetIndexBuffer	()const;
		//@}

	private:
		void _updateMaterial();
		DECLARE_SERIALISE_MAP()
	};
	//! The Overlay renderer
	/*!
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	*/
	class C3D_API OverlayRenderer : public Renderer<Overlay, OverlayRenderer>
	{
	protected:
		/**
		 * Constructor
		 */
		OverlayRenderer()
			:	Renderer<Overlay, OverlayRenderer>()
		{}
	public:
		/**
		 * Destructor
		 */
		virtual ~OverlayRenderer(){}

		virtual void DrawPanel()=0;
		virtual void DrawBorderPanel()=0;
		virtual void DrawText()=0;
	};
}

#endif
