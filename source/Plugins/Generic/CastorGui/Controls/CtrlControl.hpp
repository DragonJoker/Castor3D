/*
See LICENSE file in root folder
*/
#ifndef ___CI_CTRL_CONTROL_H___
#define ___CI_CTRL_CONTROL_H___

#include "CastorGui/CastorGuiPrerequisites.hpp"
#include "CastorGui/Theme/StyleControl.hpp"

#include <Castor3D/Event/UserInput/EventHandler.hpp>

#include <CastorUtils/Design/Named.hpp>
#include <CastorUtils/Graphics/Pixel.hpp>
#include <CastorUtils/Graphics/Position.hpp>
#include <CastorUtils/Graphics/Rectangle.hpp>
#include <CastorUtils/Graphics/Size.hpp>

namespace CastorGui
{
	/**
	 *\brief		Description of a control.
	*/
	class Control
		: public castor3d::NonClientEventHandler< Control >
		, public castor::Named
	{
		friend class ControlsManager;

	public:
		/** Constructor.
		 *\param[in]	name		The control name.
		 *\param[in]	type		The type.
		 *\param[in]	style		The control's style.
		 *\param[in]	parent		The parent control, if any.
		 *\param[in]	id			The control ID.
		 *\param[in]	position	The position.
		 *\param[in]	size		The size.
		 *\param[in]	flags		The configuration flags
		 *\param[in]	visible		Initial visibility status.
		 */
		Control( ControlType type
			, castor::String const & name
			, ControlStyleRPtr style
			, ControlRPtr parent
			, uint32_t id
			, castor::Position const & position
			, castor::Size const & size
			, uint32_t flags = 0
			, bool visible = true );

		/** Sets the style
		 *\param[in]	value	The new value
		 */
		void setStyle( ControlStyleRPtr value );

		/** Sets the position.
		 *\param[in]	value		The new value.
		 */
		void setPosition( castor::Position const & value );

		/** Retrieves the absolute control position.
		 *\return		The value.
		 */
		castor::Position getAbsolutePosition()const;

		/** sets the size.
		 *\param[in]	value		The new value.
		 */
		void setSize( castor::Size const & value );

		/** sets the background borders size.
		 *\param[in]	value		The new value.
		 */
		void setBackgroundBorders( castor::Rectangle const & value );

		/** sets the caption.
		 *\param[in]	caption	The new value
		 */
		void setCaption( castor::String const & caption );

		/** sets the visibility
		 *\param[in]	value		The new value.
		 */
		void setVisible( bool value );

		/** Retrieves the visibility status.
		 *\return		The value.
		 */
		bool isVisible()const;

		/** Retrieves a control.
		 *\param[in]	id		The control ID.
		 */
		ControlSPtr getChildControl( uint32_t id );

		/** Retrieves the style
		 *\return		The value
		 */
		void addFlag( uint32_t );

		/** Retrieves the style
		 *\return		The value
		 */
		void removeFlag( uint32_t );

		/** Retrieves the control ID.
		 *\return		The value.
		 */
		uint32_t getId()const
		{
			return m_id;
		}

		/** Retrieves the type
		 *\return		The value
		*/
		ControlType getType()const
		{
			return m_type;
		}

		/** Retrieves the style
		 *\return		The value
		 */
		uint32_t getFlags()const
		{
			return m_flags;
		}

		/** Retrieves the position
		 *\return		The value
		 */
		castor::Position const & getPosition()const
		{
			return m_position;
		}

		/** Retrieves the size
		 *\return		The value
		 */
		castor::Size const & getSize()const
		{
			return m_size;
		}

		/** Shows the control
		*/
		void show()
		{
			setVisible( true );
		}

		/** Hides the control
		*/
		void hide()
		{
			setVisible( false );
		}

		/** Retrieves the control's parent
		 *\return		The parent
		 */
		ControlRPtr getParent()const
		{
			return m_parent;
		}

		/** Retrieves the control's main overlay (to be the parent of child controls' overlays)
		 *\return		The main overlay
		 */
		castor3d::BorderPanelOverlaySPtr getBackground()const
		{
			return m_background.lock();
		}

		/** Retrieves the controls manager
		 *\return		The manager
		 */
		ControlsManagerSPtr getControlsManager()const
		{
			return m_ctrlManager.lock();
		}

		/** Retrieves the cursor when mouse is over this control
		 *\return		The main overlay
		 */
		MouseCursor getCursor()const
		{
			return m_style->getCursor();
		}

		/** Retrieves the engine.
		 *\return	The engine
		 */
		castor3d::Engine & getEngine()const
		{
			return m_style->getEngine();
		}

		/**
		 *\return	The base style.
		 */
		ControlStyle const & getBaseStyle()const
		{
			return *m_style;
		}

	protected:
		ControlStyle & getBaseStyle()
		{
			return *m_style;
		}
		/** Creates the control's overlays.
		 *\param[in]	p_ctrlManager	The controls manager.
		 */
		void create( ControlsManagerSPtr ctrlManager );

		/** Destroys the control's overlays.
		 */
		void destroy();

		/** Retrieves the visibility status
		 *\return		The value
		*/
		bool doIsVisible()const;

	private:
		/** Creates the control's overlays and sub-controls
		*/
		virtual void doCreate() = 0;

		/** Destroys the control's overlays and sub-controls
		*/
		virtual void doDestroy() = 0;

		/** sets the position
		*\param[in]	value		The new value
		*/
		virtual void doSetPosition( castor::Position const & value ) = 0;

		/** sets the size
		*\param[in]	value	The new value
		*/
		virtual void doSetSize( castor::Size const & value ) = 0;

		/** Updates the style
		*/
		virtual void doUpdateStyle() = 0;

		/** sets the caption.
		*\param[in]	caption	The new value
		*/
		virtual void doSetCaption( castor::String const & caption ){}

		/** Tells if the control catches mouse events
		*\remarks	A control catches mouse events when it is visible, enabled, and when it explicitly catches it (enables by default, except for static controls
		*\return	false if the mouse events don't affect the contro
		*/
		bool doCatchesMouseEvents()const override
		{
			return isVisible();
		}

		/** Tells if the control catches 'tab' key
		 *\remarks		A control catches 'tab' key when it is visible, enabled, and when it explicitly catches it (disabled by default
		 *\return		false if the 'tab' key doesn't affect the control
		 */
		bool doCatchesTabKey()const override
		{
			return isVisible();
		}

		/** Tells if the control catches 'return' key
		 *\remarks		A control catches 'return' key when it is visible, enabled, and when it explicitly catches it (disabled by default
		 *\return		false if the 'return' key doesn't affect the control
		 */
		bool doCatchesReturnKey()const override
		{
			return isVisible();
		}

		/** sets the visibility
		 *\remarks		Used for derived control specific behavious
		 *\param[in]	p_value		The new value
		 */
		virtual void doSetVisible( bool p_visible  )
		{
		}

		/** Updates the style relative stuff.
		*/
		virtual void doUpdateFlags()
		{
		}

	protected:
		//! The parent control, if any
		ControlRPtr m_parent;
		//! The cursor when mouse is over this control
		MouseCursor m_cursor;
		//! The background material
		castor3d::MaterialRPtr m_backgroundMaterial{};
		//! The foreground material
		castor3d::MaterialRPtr m_foregroundMaterial{};

	private:
		const uint32_t m_id;
		const ControlType m_type;
		ControlStyleRPtr m_style;
		uint32_t m_flags;
		castor::Position m_position;
		castor::Size m_size;
		castor::Rectangle m_borders;
		castor3d::BorderPanelOverlayWPtr m_background;
		std::vector< ControlWPtr > m_children;
		ControlsManagerWPtr m_ctrlManager;
	};
}

#endif
