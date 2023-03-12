/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ButtonCtrl_H___
#define ___C3D_ButtonCtrl_H___

#include "Castor3D/Gui/Controls/CtrlControl.hpp"
#include "Castor3D/Gui/Theme/StyleButton.hpp"

namespace castor3d
{
	class ButtonCtrl
		: public Control
	{
	public:
		/** Constructor
		 *\param[in]	name	The control name
		 *\param[in]	style	The control style
		 *\param[in]	parent	The parent control, if any
		 *\param[in]	id		The control ID
		 */
		ButtonCtrl( SceneRPtr scene
			, castor::String const & name
			, ButtonStyleRPtr style
			, ControlRPtr parent
			, uint32_t id );

		/** Constructor
		 *\param[in]	name		The control name
		 *\param[in]	style		The control style
		 *\param[in]	parent		The parent control, if any
		 *\param[in]	caption		The caption
		 *\param[in]	id			The control ID
		 *\param[in]	position	The position
		 *\param[in]	size		The size
		 *\param[in]	flags		The configuration flags
		 *\param[in]	visible		Initial visibility status
		 */
		ButtonCtrl( SceneRPtr scene
			, castor::String const & name
			, ButtonStyleRPtr style
			, ControlRPtr parent
			, uint32_t id
			, castor::String const & caption
			, castor::Position const & position
			, castor::Size const & size
			, uint32_t flags = 0
			, bool visible = true );

		/** Sets the horizontal alignment for the text.
		*\param[in]	align	The new value.
		*/
		void setHAlign( HAlign align );

		/** Sets the vertical alignment for the text.
		*\param[in]	align	The new value.
		*/
		void setVAlign( VAlign align );

		/** Retrieves the caption
		 *\return		The value
		 */
		castor::String const & getCaption()const
		{
			return m_caption;
		}

		/** Connects a function to a button event
		 *\param[in]	event		The event type
		 *\param[in]	function	The function
		 *\return		The internal function index, to be able to disconnect it
		 */
		OnButtonEventConnection connect( ButtonEvent event
			, OnButtonEventFunction function )
		{
			return m_signals[size_t( event )].connect( function );
		}

		/**
		*\return	The button style
		*/
		ButtonStyle const & getStyle()const
		{
			return static_cast< ButtonStyle const & >( getBaseStyle() );
		}

		static ControlType constexpr Type{ ControlType::eButton };

	private:
		ButtonStyle & getStyle()
		{
			return static_cast< ButtonStyle & >( getBaseStyle() );
		}
		/** @copydoc Control::doCreate
		*/
		void doCreate()override;

		/** @copydoc Control::doDestroy
		*/
		void doDestroy()override;

		/** @copydoc Control::doSetPosition
		*/
		void doSetPosition( castor::Position const & value )override;

		/** @copydoc Control::doSetSize
		*/
		void doSetSize( castor::Size const & value )override;

		/** @copydoc Control::doUpdateStyle
		*/
		void doUpdateStyle()override;

		/** @copydoc Control::doSetCaption
		*/
		void doSetCaption( castor::String const & caption )override;

		/** @copydoc Control::doSetVisible
		*/
		void doSetVisible( bool visible )override;

		/** Event when mouse enters the control
		 *\param[in]	event		The mouse event
		 */
		void onMouseEnter( MouseEvent const & event );

		/** Event when mouse leaves the control
		 *\param[in]	event		The mouse event
		 */
		void onMouseLeave( MouseEvent const & event );

		/** Event when mouse left button is pressed.
		 *\param[in]	event		The mouse event.
		 */
		void onMouseButtonDown( MouseEvent const & event );

		/** Event when mouse left button is released.
		 *\param[in]	event		The mouse event.
		 */
		void onMouseButtonUp( MouseEvent const & event );

		/** Creates a material with an ambient colour equal to material->colour + offset
		 *\param[in]	material	The material.
		 *\param[in]	offset		The colour offset.
		 *\return		The created material.
		 */
		MaterialRPtr doCreateMaterial( MaterialRPtr material
			, float offset );

	private:
		castor::String m_caption;
		TextOverlayWPtr m_text;
		OnButtonEvent m_signals[size_t( ButtonEvent::eCount )];
		OnEnableConnection m_onEnable;
	};
}

#endif
