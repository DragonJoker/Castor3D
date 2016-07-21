/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (At your option ) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___CI_CTRL_CONTROL_H___
#define ___CI_CTRL_CONTROL_H___

#include "CastorGuiPrerequisites.hpp"

#include <Event/UserInput/EventHandler.hpp>

#include <Pixel.hpp>
#include <Position.hpp>
#include <Graphics/Rectangle.hpp>
#include <Size.hpp>

namespace CastorGui
{
	/*!
	 *\author		Sylvain DOREMU
	 *\brief		Description of a contro
	*/
	class Control
		: public Castor3D::NonClientEventHandler< Control >
	{
	public:
		/** Constructor.
		 *\param[in]	p_type		The type.
		 *\param[in]	p_engine	The engine.
		 *\param[in]	p_parent	The parent control, if any.
		 *\param[in]	p_id		The control ID.
		 *\param[in]	p_position	The position.
		 *\param[in]	p_size		The size.
		 *\param[in]	p_style		The style.
		 *\param[in]	p_visible	Initial visibility status.
		 */
		Control( eCONTROL_TYPE p_type, Castor3D::Engine * p_engine, ControlRPtr p_parent, uint32_t p_id, Castor::Position const & p_position, Castor::Size const & p_size, uint32_t p_style = 0, bool p_visible = true );

		/** Destructor.
		 */
		virtual ~Control();

		/** Creates the control's overlays.
		 *\param[in]	p_ctrlManager	The controls manager.
		 */
		void Create( ControlsManagerSPtr p_ctrlManager );

		/** Destroys the control's overlays.
		 */
		void Destroy();

		/** Sets the position.
		 *\param[in]	p_value		The new value.
		 */
		void SetPosition( Castor::Position const & p_value );

		/** Retrieves the absolute control position.
		 *\return		The value.
		 */
		Castor::Position GetAbsolutePosition()const;

		/** Sets the size.
		 *\param[in]	p_value		The new value.
		 */
		void SetSize( Castor::Size const & p_value );

		/** Sets the background borders size.
		 *\param[in]	p_size		The new value.
		 */
		void SetBackgroundBorders( Castor::Rectangle const & p_size );

		/** Sets the background material.
		 *\param[in]	p_material		The new value.
		 */
		void SetBackgroundMaterial( Castor3D::MaterialSPtr p_material );

		/** Sets the foreground material.
		 *\param[in]	p_material		The new value.
		 */
		void SetForegroundMaterial( Castor3D::MaterialSPtr p_material );

		/** Sets the visibility
		 *\param[in]	p_value		The new value.
		 */
		void SetVisible( bool p_value );

		/** Retrieves the visibility status.
		 *\return		The value.
		 */
		bool IsVisible()const;

		/** Retrieves a control.
		 *\param[in]	p_id		The control ID.
		 */
		ControlSPtr GetChildControl( uint32_t p_id );

		/** Retrieves the style
		 *\return		The value
		 */
		void AddStyle( uint32_t );

		/** Retrieves the style
		 *\return		The value
		 */
		void RemoveStyle( uint32_t );

		/** Retrieves the control ID.
		 *\return		The value.
		 */
		inline uint32_t GetId()const
		{
			return m_id;
		}

		/** Retrieves the type
		 *\return		The value
		*/
		inline eCONTROL_TYPE GetType()const
		{
			return m_type;
		}

		/** Retrieves the style
		 *\return		The value
		*/
		inline uint32_t GetStyle()const
		{
			return m_style;
		}

		/** Retrieves the position
		 *\return		The value
		*/
		inline Castor::Position const & GetPosition()const
		{
			return m_position;
		}

		/** Retrieves the size
		 *\return		The value
		*/
		inline Castor::Size const & GetSize()const
		{
			return m_size;
		}

		/** Retrieves the background material
		 *\return		The value
		*/
		inline Castor3D::MaterialSPtr GetBackgroundMaterial()const
		{
			return m_backgroundMaterial.lock();
		}

		/** Retrieves the foreground material
		 *\return		The value
		*/
		inline Castor3D::MaterialSPtr GetForegroundMaterial()const
		{
			return m_foregroundMaterial.lock();
		}

		/** Shows the control
		*/
		inline void Show()
		{
			SetVisible( true );
		}

		/** Hides the control
		*/
		inline void Hide()
		{
			SetVisible( false );
		}

		/** Retrieves the control's parent
		 *\return		The parent
		*/
		inline ControlRPtr GetParent()const
		{
			return m_parent;
		}

		/** Retrieves the control's main overlay (to be the parent of child controls' overlays)
		 *\return		The main overlay
		*/
		inline Castor3D::BorderPanelOverlaySPtr GetBackground()const
		{
			return m_background.lock();
		}

		/** Retrieves the cursor when mouse is over this control
		 *\return		The main overlay
		*/
		inline eMOUSE_CURSOR GetCursor()const
		{
			return m_cursor;
		}

		/** Retrieves the engine.
		 *\return		The engine
		*/
		inline Castor3D::Engine * GetEngine()const
		{
			return m_engine;
		}

		/** Retrieves the controls manager
		 *\return		The manager
		*/
		inline ControlsManagerSPtr GetControlsManager()const
		{
			return m_ctrlManager.lock();
		}

	protected:
		/** Retrieves the visibility status
		 *\return		The value
		*/
		inline bool DoIsVisible()const
		{
			return m_visible;
		}

	private:
		/** Creates the control's overlays and sub-controls
		*/
		virtual void DoCreate() = 0;

		/** Destroys the control's overlays and sub-controls
		*/
		virtual void DoDestroy() = 0;

		/** Sets the position
		*\param[in]	p_value		The new value
		*/
		virtual void DoSetPosition( Castor::Position const & p_value ) = 0;

		/** Sets the size
		*\param[in]	p_value		The new value
		*/
		virtual void DoSetSize( Castor::Size const & p_value ) = 0;

		/** Sets the background material
		*\param[in]	p_material		The new value
		*/
		virtual void DoSetBackgroundMaterial( Castor3D::MaterialSPtr p_material ) = 0;

		/** Sets the foreground material
		*\param[in]	p_material		The new value
		*/
		virtual void DoSetForegroundMaterial( Castor3D::MaterialSPtr p_material ) = 0;

		/** Tells if the control catches mouse events
		 *\remarks		A control catches mouse events when it is visible, enabled, and when it explicitly catches it (enables by default, except for static controls
		 *\return		false if the mouse events don't affect the contro
		*/
		virtual bool DoCatchesMouseEvents()const
		{
			return IsVisible();
		}

		/** Tells if the control catches 'tab' key
		 *\remarks		A control catches 'tab' key when it is visible, enabled, and when it explicitly catches it (disabled by default
		 *\return		false if the 'tab' key doesn't affect the control
		 */
		virtual bool DoCatchesTabKey()const
		{
			return IsVisible();
		}

		/** Tells if the control catches 'return' key
		 *\remarks		A control catches 'return' key when it is visible, enabled, and when it explicitly catches it (disabled by default
		 *\return		false if the 'return' key doesn't affect the control
		 */
		virtual bool DoCatchesReturnKey()const
		{
			return IsVisible();
		}

		/** Sets the visibility
		 *\remarks		Used for derived control specific behavious
		 *\param[in]	p_value		The new value
		 */
		virtual void DoSetVisible( bool p_visible )
		{
		}

		/** Updates the style relative stuff.
		*/
		virtual void DoUpdateStyle()
		{
		}

	protected:
		//! The parent control, if any
		ControlRPtr m_parent;
		//! The cursor when mouse is over this control
		eMOUSE_CURSOR m_cursor;
		//! The background material
		Castor3D::MaterialWPtr m_backgroundMaterial;
		//! The foreground material
		Castor3D::MaterialWPtr m_foregroundMaterial;

	private:
		//! The ID
		const uint32_t m_id;
		//! The type
		const eCONTROL_TYPE m_type;
		//! The style
		uint32_t m_style;
		//! The position
		Castor::Position m_position;
		//! The dimensions
		Castor::Size m_size;
		//! The borders size
		Castor::Rectangle m_borders;
		//! Visibility status
		bool m_visible;
		//! The border panel overlay used as a background
		Castor3D::BorderPanelOverlayWPtr m_background;
		//! The child controls
		std::vector< ControlWPtr > m_children;
		//! The engine
		Castor3D::Engine * m_engine;
		//! The controls manager
		ControlsManagerWPtr m_ctrlManager;
	};
}

#endif
