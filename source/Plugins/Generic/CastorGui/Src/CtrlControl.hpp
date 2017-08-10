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
#ifndef ___CI_CTRL_CONTROL_H___
#define ___CI_CTRL_CONTROL_H___

#include "CastorGuiPrerequisites.hpp"

#include <Event/UserInput/EventHandler.hpp>

#include <Design/Named.hpp>
#include <Graphics/Pixel.hpp>
#include <Graphics/Position.hpp>
#include <Graphics/Rectangle.hpp>
#include <Graphics/Size.hpp>

namespace CastorGui
{
	/*!
	 *\author		Sylvain DOREMU
	 *\brief		Description of a contro
	*/
	class Control
		: public castor3d::NonClientEventHandler< Control >
		, public castor::Named
	{
		friend class ControlsManager;

	public:
		/** Constructor.
		 *\param[in]	p_name		The control name.
		 *\param[in]	p_type		The type.
		 *\param[in]	engine	The engine.
		 *\param[in]	p_parent	The parent control, if any.
		 *\param[in]	p_id		The control ID.
		 *\param[in]	p_position	The position.
		 *\param[in]	p_size		The size.
		 *\param[in]	p_style		The style.
		 *\param[in]	p_visible	Initial visibility status.
		 */
		Control( ControlType p_type
			, castor::String const & p_name
			, castor3d::Engine & engine
			, ControlRPtr p_parent
			, uint32_t p_id
			, castor::Position const & p_position
			, castor::Size const & p_size
			, uint32_t p_style = 0
			, bool p_visible = true );

		/** Destructor.
		 */
		virtual ~Control();

		/** sets the position.
		 *\param[in]	p_value		The new value.
		 */
		void setPosition( castor::Position const & p_value );

		/** Retrieves the absolute control position.
		 *\return		The value.
		 */
		castor::Position getAbsolutePosition()const;

		/** sets the size.
		 *\param[in]	p_value		The new value.
		 */
		void setSize( castor::Size const & p_value );

		/** sets the background borders size.
		 *\param[in]	p_size		The new value.
		 */
		void setBackgroundBorders( castor::Rectangle const & p_size );

		/** sets the background material.
		 *\param[in]	p_material		The new value.
		 */
		void setBackgroundMaterial( castor3d::MaterialSPtr p_material );

		/** sets the foreground material.
		 *\param[in]	p_material		The new value.
		 */
		void setForegroundMaterial( castor3d::MaterialSPtr p_material );

		/** sets the caption.
		 *\param[in]	p_caption	The new value
		 */
		void setCaption( castor::String const & p_caption );

		/** sets the visibility
		 *\param[in]	p_value		The new value.
		 */
		void setVisible( bool p_value );

		/** Retrieves the visibility status.
		 *\return		The value.
		 */
		bool isVisible()const;

		/** Retrieves a control.
		 *\param[in]	p_id		The control ID.
		 */
		ControlSPtr getChildControl( uint32_t p_id );

		/** Retrieves the style
		 *\return		The value
		 */
		void addStyle( uint32_t );

		/** Retrieves the style
		 *\return		The value
		 */
		void removeStyle( uint32_t );

		/** Retrieves the control ID.
		 *\return		The value.
		 */
		inline uint32_t getId()const
		{
			return m_id;
		}

		/** Retrieves the type
		 *\return		The value
		*/
		inline ControlType getType()const
		{
			return m_type;
		}

		/** Retrieves the style
		 *\return		The value
		*/
		inline uint32_t getStyle()const
		{
			return m_style;
		}

		/** Retrieves the position
		 *\return		The value
		*/
		inline castor::Position const & getPosition()const
		{
			return m_position;
		}

		/** Retrieves the size
		 *\return		The value
		*/
		inline castor::Size const & getSize()const
		{
			return m_size;
		}

		/** Retrieves the background material
		 *\return		The value
		*/
		inline castor3d::MaterialSPtr getBackgroundMaterial()const
		{
			return m_backgroundMaterial.lock();
		}

		/** Retrieves the foreground material
		 *\return		The value
		*/
		inline castor3d::MaterialSPtr getForegroundMaterial()const
		{
			return m_foregroundMaterial.lock();
		}

		/** Shows the control
		*/
		inline void show()
		{
			setVisible( true );
		}

		/** Hides the control
		*/
		inline void hide()
		{
			setVisible( false );
		}

		/** Retrieves the control's parent
		 *\return		The parent
		*/
		inline ControlRPtr getParent()const
		{
			return m_parent;
		}

		/** Retrieves the control's main overlay (to be the parent of child controls' overlays)
		 *\return		The main overlay
		*/
		inline castor3d::BorderPanelOverlaySPtr getBackground()const
		{
			return m_background.lock();
		}

		/** Retrieves the cursor when mouse is over this control
		 *\return		The main overlay
		*/
		inline MouseCursor getCursor()const
		{
			return m_cursor;
		}

		/** Retrieves the engine.
		 *\return		The engine
		*/
		inline castor3d::Engine const & getEngine()const
		{
			return m_engine;
		}

		/** Retrieves the engine.
		 *\return		The engine
		*/
		inline castor3d::Engine & getEngine()
		{
			return m_engine;
		}

		/** Retrieves the controls manager
		 *\return		The manager
		*/
		inline ControlsManagerSPtr getControlsManager()const
		{
			return m_ctrlManager.lock();
		}

	protected:
		/** Creates the control's overlays.
		 *\param[in]	p_ctrlManager	The controls manager.
		 */
		void create( ControlsManagerSPtr p_ctrlManager );

		/** Destroys the control's overlays.
		 */
		void destroy();

		/** Retrieves the visibility status
		 *\return		The value
		*/
		inline bool doIsVisible()const
		{
			return m_visible;
		}

	private:
		/** Creates the control's overlays and sub-controls
		*/
		virtual void doCreate() = 0;

		/** Destroys the control's overlays and sub-controls
		*/
		virtual void doDestroy() = 0;

		/** sets the position
		*\param[in]	p_value		The new value
		*/
		virtual void doSetPosition( castor::Position const & p_value ) = 0;

		/** sets the size
		*\param[in]	p_value		The new value
		*/
		virtual void doSetSize( castor::Size const & p_value ) = 0;

		/** sets the background material
		*\param[in]	p_material		The new value
		*/
		virtual void doSetBackgroundMaterial( castor3d::MaterialSPtr p_material ) = 0;

		/** sets the foreground material
		*\param[in]	p_material		The new value
		*/
		virtual void doSetForegroundMaterial( castor3d::MaterialSPtr p_material ) = 0;

		/** sets the caption.
		 *\param[in]	p_caption	The new value
		 */
		virtual void doSetCaption( castor::String const & p_caption ){}

		/** Tells if the control catches mouse events
		 *\remarks		A control catches mouse events when it is visible, enabled, and when it explicitly catches it (enables by default, except for static controls
		 *\return		false if the mouse events don't affect the contro
		*/
		virtual bool doCatchesMouseEvents()const
		{
			return isVisible();
		}

		/** Tells if the control catches 'tab' key
		 *\remarks		A control catches 'tab' key when it is visible, enabled, and when it explicitly catches it (disabled by default
		 *\return		false if the 'tab' key doesn't affect the control
		 */
		virtual bool doCatchesTabKey()const
		{
			return isVisible();
		}

		/** Tells if the control catches 'return' key
		 *\remarks		A control catches 'return' key when it is visible, enabled, and when it explicitly catches it (disabled by default
		 *\return		false if the 'return' key doesn't affect the control
		 */
		virtual bool doCatchesReturnKey()const
		{
			return isVisible();
		}

		/** sets the visibility
		 *\remarks		Used for derived control specific behavious
		 *\param[in]	p_value		The new value
		 */
		virtual void doSetVisible( bool p_visible )
		{
		}

		/** Updates the style relative stuff.
		*/
		virtual void doUpdateStyle()
		{
		}

	protected:
		//! The parent control, if any
		ControlRPtr m_parent;
		//! The cursor when mouse is over this control
		MouseCursor m_cursor;
		//! The background material
		castor3d::MaterialWPtr m_backgroundMaterial;
		//! The foreground material
		castor3d::MaterialWPtr m_foregroundMaterial;

	private:
		//! The ID
		const uint32_t m_id;
		//! The type
		const ControlType m_type;
		//! The style
		uint32_t m_style;
		//! The position
		castor::Position m_position;
		//! The dimensions
		castor::Size m_size;
		//! The borders size
		castor::Rectangle m_borders;
		//! Visibility status
		bool m_visible;
		//! The border panel overlay used as a background
		castor3d::BorderPanelOverlayWPtr m_background;
		//! The child controls
		std::vector< ControlWPtr > m_children;
		//! The engine
		castor3d::Engine & m_engine;
		//! The controls manager
		ControlsManagerWPtr m_ctrlManager;
	};
}

#endif
