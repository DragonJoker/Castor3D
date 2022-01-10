/*
See LICENSE file in root folder
*/
#ifndef ___CI_CTRL_BUTTON_H___
#define ___CI_CTRL_BUTTON_H___

#include "CastorGui/Controls/CtrlControl.hpp"
#include "CastorGui/Theme/StyleButton.hpp"

namespace CastorGui
{
	/**
	\brief		Button control.
	*/
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
		ButtonCtrl( castor::String const & name
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
		ButtonCtrl( castor::String const & name
			, ButtonStyleRPtr style
			, ControlRPtr parent
			, uint32_t id
			, castor::String const & caption
			, castor::Position const & position
			, castor::Size const & size
			, uint32_t flags = 0
			, bool visible = true );

		/** Sets the horizontal alignment for the text.
		*\param[in]	p_align	The new value.
		*/
		void setHAlign( castor3d::HAlign align );

		/** Sets the vertical alignment for the text.
		*\param[in]	p_align	The new value.
		*/
		void setVAlign( castor3d::VAlign align );

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
		/** @copydoc CastorGui::Control::doCreate
		*/
		void doCreate()override;

		/** @copydoc CastorGui::Control::doDestroy
		*/
		void doDestroy()override;

		/** @copydoc CastorGui::Control::doSetPosition
		*/
		void doSetPosition( castor::Position const & value )override;

		/** @copydoc CastorGui::Control::doSetSize
		*/
		void doSetSize( castor::Size const & value )override;

		/** @copydoc CastorGui::Control::doUpdateStyle
		*/
		void doUpdateStyle()override;

		/** @copydoc CastorGui::Control::doSetCaption
		*/
		void doSetCaption( castor::String const & caption )override;

		/** @copydoc CastorGui::Control::doSetVisible
		*/
		void doSetVisible( bool visible )override;

		/** Event when mouse enters the control
		 *\param[in]	event		The mouse event
		 */
		void onMouseEnter( castor3d::MouseEvent const & event );

		/** Event when mouse leaves the control
		 *\param[in]	event		The mouse event
		 */
		void onMouseLeave( castor3d::MouseEvent const & event );

		/** Event when mouse left button is pressed.
		 *\param[in]	event		The mouse event.
		 */
		void onMouseButtonDown( castor3d::MouseEvent const & event );

		/** Event when mouse left button is released.
		 *\param[in]	event		The mouse event.
		 */
		void onMouseButtonUp( castor3d::MouseEvent const & event );

		/** Creates a material with an ambient colour equal to p_material->ambient + p_offset
		 *\param[in]	material	The material.
		 *\param[in]	offset		The colour offset.
		 *\return		The created material.
		 */
		castor3d::MaterialRPtr doCreateMaterial( castor3d::MaterialRPtr material
			, float offset );

	private:
		castor::String m_caption;
		castor3d::TextOverlayWPtr m_text;
		OnButtonEvent m_signals[size_t( ButtonEvent::eCount )];
	};
}

#endif
