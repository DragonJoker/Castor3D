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
		friend class ScrollableCtrl;
		friend class ProgressCtrl;

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
		C3D_API Control( ControlType type
			, SceneRPtr scene
			, castor::String const & name
			, ControlStyleRPtr style
			, ControlRPtr parent
			, castor::Position const & position
			, castor::Size const & size
			, ControlFlagType flags = 0
			, bool visible = true );
		C3D_API ~Control()noexcept;

		/** Sets the style
		 *\param[in]	value	The new value
		 */
		C3D_API void setStyle( ControlStyleRPtr value );

		/** Sets the position.
		 *\param[in]	value		The new value.
		 */
		C3D_API void setPosition( castor::Position const & value );

		/** Sets the size.
		 *\param[in]	value		The new value.
		 */
		C3D_API void setSize( castor::Size const & value );

		/** Sets the background centerUV.
		 *\param[in]	value		The new value.
		 */
		C3D_API void setUV( castor::Point4d const & value );

		/** Sets the background borders size.
		 *\param[in]	value		The new value.
		 */
		C3D_API void setBorderSize( castor::Point4ui const & value );

		/** Sets the background borders inner UV.
		 *\param[in]	value		The new value.
		 */
		C3D_API void setBorderInnerUV( castor::Point4d const & value );

		/** Sets the background borders outer UV.
		 *\param[in]	value		The new value.
		 */
		C3D_API void setBorderOuterUV( castor::Point4d const & value );

		/** Sets the caption.
		 *\param[in]	caption	The new value
		 */
		C3D_API void setCaption( castor::U32String const & caption );

		/** Sets the visibility
		 *\param[in]	value		The new value.
		 */
		C3D_API void setVisible( bool value );

		/** Retrieves the absolute control position.
		 *\return		The value.
		 */
		C3D_API castor::Position getAbsolutePosition()const;

		/** Retrieves the visibility status.
		 *\return		The value.
		 */
		C3D_API bool isVisible()const;

		/** Retrieves the background invisibility status.
		 *\return		\p true if the background is invisible, making the control not targetable (but it's children still are).
		 */
		C3D_API bool isBackgroundInvisible()const
		{
			return m_style->isBackgroundInvisible();
		}

		/** Retrieves a control.
		 *\param[in]	id		The control ID.
		 */
		C3D_API ControlSPtr getChildControl( ControlID id )const;

		/** Checks if the position is on any border (N, W, S, E).
		 *\param[in]	The position to check for.
		 *\return		One bool per direction.
		 */
		C3D_API std::array< bool, 4u > isInResizeRange( castor::Position const & position )const;

		/** Adds a flag.
		 */
		template< ControlFlagTypeT FlagTypeT >
		void addFlag( FlagTypeT flag )
		{
			m_flags |= ControlFlagType( flag );
			doUpdateFlags();
		}

		/** Adds a flag.
		 */
		template< ControlFlagTypeT FlagTypeT >
		void addFlag( castor::FlagCombination< FlagTypeT > flag )
		{
			m_flags |= ControlFlagType( flag );
			doUpdateFlags();
		}

		/** Removes a flag.
		 */
		template< ControlFlagTypeT FlagTypeT >
		void removeFlag( FlagTypeT flag )
		{
			m_flags &= ~ControlFlagType( flag );
			doUpdateFlags();
		}

		/** Removes a flag.
		 */
		template< ControlFlagTypeT FlagTypeT >
		void removeFlag( castor::FlagCombination< FlagTypeT > flag )
		{
			m_flags &= ~ControlFlagType( flag );
			doUpdateFlags();
		}

		/**@name Getters */
		//@{

		C3D_API castor::Point4d const & getBorderInnerUV()const;
		C3D_API castor::Point4d const & getBorderOuterUV()const;
		C3D_API BorderPosition getBorderPosition()const;
		C3D_API castor::Point4d const & getUV()const;
		C3D_API bool isBackgroundVisible()const;

		ControlID getId()const noexcept
		{
			return m_id;
		}

		ControlType getType()const noexcept
		{
			return m_type;
		}

		uint64_t getFlags()const noexcept
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

		castor::Point4ui const & getBorderSize()const noexcept
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
			return m_engine;
		}

		ControlStyle const & getBaseStyle()const noexcept
		{
			return *m_style;
		}

		std::vector< ControlWPtr > const & getChildren()const noexcept
		{
			return m_children;
		}

		bool hasScene()const noexcept
		{
			return m_scene != nullptr;
		}

		Scene & getScene()const noexcept
		{
			CU_Require( hasScene() );
			return *m_scene;
		}

		uint32_t getResizeBorderSize()const noexcept
		{
			return m_resizeBorderSize;
		}

		castor::Point4ui const & getClientRect()const noexcept
		{
			return m_clientRect;
		}

		castor::Position getClientOffset()const
		{
			return { int32_t( m_clientRect->x )
				, int32_t( m_clientRect->y ) };
		}

		castor::Size getClientSize()const
		{
			return { m_clientRect->z - m_clientRect->x
				, m_clientRect->w - m_clientRect->y };
		}
		//@}

		/** \return	The always on top status of the control.
		 */
		bool isAlwaysOnTop()const noexcept
		{
			return castor::checkFlag( getFlags(), ControlFlag::eAlwaysOnTop );
		}

		/** \return	The movable status of the control.
		 */
		bool isMovable()const noexcept
		{
			return castor::checkFlag( getFlags(), ControlFlag::eMovable );
		}

		/** \return	The resizable status of the control.
		 */
		bool isResizable()const noexcept
		{
			return castor::checkFlag( getFlags(), ControlFlag::eResizable );
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

		/** Signal called when position, size, borders or visibility are changed.
		*/
		OnControlChanged onChanged;

	protected:
		/** \return
		 *	The background overlay.
		 */
		C3D_API Overlay & getBackgroundOverlay();

		/** Sets the background material.
		 *\param[in]	value		The new value.
		 */
		C3D_API void setBackgroundMaterial( MaterialRPtr value );

		/** Sets the background size.
		 *\param[in]	value		The new value.
		 */
		C3D_API void setBackgroundSize( castor::Size const & value );

		/** Sets the background borders position.
		 *\param[in]	value		The new value.
		 */
		C3D_API void setBackgroundBorderPosition( BorderPosition value );

		/** Sets the background borders material.
		 *\param[in]	value		The new value.
		 */
		C3D_API void setBackgroundBorderMaterial( MaterialRPtr value );

		/** Creates the control's overlays.
		 *\param[in]	ctrlManager	The controls manager.
		 */
		C3D_API void create( ControlsManagerSPtr ctrlManager );

		/** Destroys the control's overlays.
		 */
		C3D_API void destroy();

		/** Adds a child control.
		 */
		C3D_API void addChild( ControlSPtr control );

		ControlStyle & getBaseStyle()
		{
			return *m_style;
		}

	private:
		void updateZIndex( uint32_t & index
			, std::vector< Control * > & controls
			, std::vector< Control * > & topControls );
		void adjustZIndex( uint32_t offset );

		/** Event when mouse enters the control
		 *\param[in]	event		The mouse event
		 */
		void onMouseEnter( MouseEvent const & event );

		/** Event when mouse left button is pressed.
		 *\param[in]	event		The mouse event.
		 */
		void onMouseButtonDown( MouseEvent const & event );

		/** Event when mouse left button is released.
		 *\param[in]	event		The mouse event.
		 */
		void onMouseButtonUp( MouseEvent const & event );

		/** Event when mouse is moving over the control.
		 *\param[in]	event		The mouse event.
		 */
		void onMouseMove( MouseEvent const & event );

		/** Event when mouse leaves the control.
		 *\param[in]	event		The mouse event.
		 */
		void onMouseLeave( MouseEvent const & event );

		/** Begins dragging of the control.
		 *\param[in]	event		The mouse event.
		 */
		bool beginMove( MouseEvent const & event );

		/** Drags the control.
		 *\param[in]	event		The mouse event.
		 */
		void move( MouseEvent const & event );

		/** Ends dragging of the control.
		 *\param[in]	event		The mouse event.
		 */
		void endMove( MouseEvent const & event );

		/** Begins dragging of the control.
		 *\param[in]	event		The mouse event.
		 */
		bool beginResize( MouseEvent const & event );

		/** Drags the control.
		 *\param[in]	event		The mouse event.
		 */
		void resize( MouseEvent const & event );

		/** Ends dragging of the control.
		 *\param[in]	event		The mouse event.
		 */
		void endResize( MouseEvent const & event );

		/** Updates the area where sub-controls can be draw.
		 */
		void updateClientRect();

		/** Creates the control's overlays and sub-controls
		*/
		virtual void doCreate() = 0;

		/** Destroys the control's overlays and sub-controls
		*/
		virtual void doDestroy() = 0;

		/** Adds a child control.
		 */
		virtual void doAddChild( ControlSPtr control )
		{
		}

		/** Allows child classes to modify the border size before it being set.
		 */
		virtual castor::Point4ui doUpdateBorderSize( castor::Point4ui const & value )const noexcept
		{
			return value;
		}

		/** Allows child classes to modify the position before it being set.
		 */
		virtual castor::Position doUpdatePosition( castor::Position const & value )const noexcept
		{
			return value;
		}

		/** Allows child classes to modify the size before it being set.
		 */
		virtual castor::Size doUpdateSize( castor::Size const & value )const noexcept
		{
			return value;
		}

		/** Updates the area where sub-controls can be draw.
		 */
		virtual castor::Point4ui doUpdateClientRect( castor::Point4ui const & clientRect )
		{
			return clientRect;
		}

		/** Sets the background borders size.
		 *\param[in]	value		The new value.
		 */
		virtual void doSetBorderSize( castor::Point4ui const & value ) {}

		/** Sets the position
		*\param[in]	value		The new value
		*/
		virtual void doSetPosition( castor::Position const & value ) = 0;

		/** Sets the size
		*\param[in]	value	The new value
		*/
		virtual void doSetSize( castor::Size const & value ) = 0;

		/** Updates the style
		*/
		virtual void doUpdateStyle() = 0;

		/** sets the caption.
		*\param[in]	caption	The new value
		*/
		virtual void doSetCaption( castor::U32String const & caption ) {}

		/** Event when mouse enters the control
		 *\param[in]	event		The mouse event
		 */
		virtual void doOnMouseEnter( MouseEvent const & event ) {}

		/** Event when mouse left button is pressed.
		 *\param[in]	event		The mouse event.
		 */
		virtual void doOnMouseButtonDown( MouseEvent const & event ) {}

		/** Event when mouse left button is released.
		 *\param[in]	event		The mouse event.
		 */
		virtual void doOnMouseButtonUp( MouseEvent const & event ) {}

		/** Event when mouse is moving over the control.
		 *\param[in]	event		The mouse event.
		 */
		virtual void doOnMouseMove( MouseEvent const & event ) {}

		/** Event when mouse leaves the control.
		 *\param[in]	event		The mouse event.
		 */
		virtual void doOnMouseLeave( MouseEvent const & event ) {}

		/** Tells if the control catches mouse events
		*\remarks	A control catches mouse events when it is visible, enabled
		*			and when it explicitly catches it (enables by default, except for static controls.
		*			Setting both background and foreground invisible will also prevent mouse events catching.
		*\return	\p false if the mouse events don't affect the control.
		*/
		bool doCatchesMouseEvents()const override
		{
			return isVisible()
				&& m_style
				&& ( !m_style->isBackgroundInvisible() && !m_style->isForegroundInvisible() );
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

		/** Updates the overlays Z index.
		*/
		virtual void doUpdateZIndex( uint32_t & index )
		{
		}

		/** Adjusts the overlays Z index.
		*/
		virtual void doAdjustZIndex( uint32_t offset )
		{
		}

		BorderPanelOverlay & doGetBackground()const
		{
			auto bg = m_background.lock();

			if ( !bg )
			{
				CU_Exception( "No background set" );
			}

			return *bg;
		}

		bool isMoving()const noexcept
		{
			return m_moving;
		}

		bool isResizing()const noexcept
		{
			return m_resizingN
				|| m_resizingW
				|| m_resizingS
				|| m_resizingE;
		}

	protected:
		SceneRPtr m_scene{};
		//! The parent control, if any
		ControlRPtr m_parent{};
		//! The background material
		MaterialRPtr m_backgroundMaterial{};
		//! The foreground material
		MaterialRPtr m_foregroundMaterial{};
		//! The control flgas.
		ControlFlagType m_flags{};

	private:
		const ControlID m_id;
		const ControlType m_type;
		Engine & m_engine;
		ControlStyleRPtr m_style{};
		castor::Position m_position{};
		castor::Size m_size{};
		castor::Point4ui m_borders{};
		BorderPanelOverlayWPtr m_background{};
		std::vector< ControlWPtr > m_children{};
		ControlsManagerWPtr m_ctrlManager{};
		bool m_moving{};
		bool m_resizingN{};
		bool m_resizingW{};
		bool m_resizingS{};
		bool m_resizingE{};
		castor::Position m_mouseStartMousePosition{};
		castor::Position m_mouseStartPosition{};
		castor::Size m_mouseStartSize{};
		uint32_t m_resizeBorderSize{};
		castor::Point4ui m_clientRect;
	};
}

#endif
