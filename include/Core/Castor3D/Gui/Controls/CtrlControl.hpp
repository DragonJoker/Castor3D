/*
See LICENSE file in root folder
*/
#ifndef ___C3D_Control_H___
#define ___C3D_Control_H___

#include "Castor3D/Gui/GuiModule.hpp"
#include "Castor3D/Gui/Theme/StyleControl.hpp"

#include "Castor3D/Event/UserInput/EventHandler.hpp"

#include <CastorUtils/Design/Named.hpp>
#include <CastorUtils/Graphics/Pixel.hpp>
#include <CastorUtils/Graphics/Position.hpp>
#include <CastorUtils/Graphics/Rectangle.hpp>
#include <CastorUtils/Graphics/Size.hpp>

namespace castor3d
{
	class Control
		: public NonClientEventHandler< Control >
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
			, SceneRPtr scene
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
		void setBackgroundBorders( castor::Point4ui const & value );

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

		/**@name Getters */
		//@{

		uint32_t getId()const noexcept
		{
			return m_id;
		}

		ControlType getType()const noexcept
		{
			return m_type;
		}

		uint32_t getFlags()const noexcept
		{
			return m_flags;
		}

		castor::Position const & getPosition()const noexcept
		{
			return m_position;
		}

		castor::Size const & getSize()const noexcept
		{
			return m_size;
		}

		ControlRPtr getParent()const noexcept
		{
			return m_parent;
		}

		BorderPanelOverlaySPtr getBackground()const noexcept
		{
			return m_background.lock();
		}

		castor::Point4ui const & getBackgroundBorders()const noexcept
		{
			return m_borders;
		}

		ControlsManagerSPtr getControlsManager()const noexcept
		{
			return m_ctrlManager.lock();
		}

		MouseCursor getCursor()const noexcept
		{
			return m_style->getCursor();
		}

		Engine & getEngine()const noexcept
		{
			return m_style->getEngine();
		}

		ControlStyle const & getBaseStyle()const noexcept
		{
			return *m_style;
		}

		//@}

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

	protected:
		ControlStyle & getBaseStyle()
		{
			return *m_style;
		}
		/** Creates the control's overlays.
		 *\param[in]	ctrlManager	The controls manager.
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
		 *\param[in]	value		The new value
		 */
		virtual void doSetVisible( bool visible  )
		{
		}

		/** Updates the style relative stuff.
		*/
		virtual void doUpdateFlags()
		{
		}

	protected:
		SceneRPtr m_scene{};
		//! The parent control, if any
		ControlRPtr m_parent;
		//! The cursor when mouse is over this control
		MouseCursor m_cursor;
		//! The background material
		MaterialRPtr m_backgroundMaterial{};
		//! The foreground material
		MaterialRPtr m_foregroundMaterial{};

	private:
		const uint32_t m_id;
		const ControlType m_type;
		ControlStyleRPtr m_style;
		uint32_t m_flags;
		castor::Position m_position;
		castor::Size m_size;
		castor::Point4ui m_borders;
		BorderPanelOverlayWPtr m_background;
		std::vector< ControlWPtr > m_children;
		ControlsManagerWPtr m_ctrlManager;
	};
}

#endif
