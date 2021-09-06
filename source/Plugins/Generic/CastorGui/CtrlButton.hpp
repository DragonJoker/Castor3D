/*
See LICENSE file in root folder
*/
#ifndef ___CI_CTRL_BUTTON_H___
#define ___CI_CTRL_BUTTON_H___

#include "CastorGui/CtrlControl.hpp"

namespace CastorGui
{
	/**
	\author		Sylvain DOREMU
	\date		16/02/201
	\version	0.10.0
	\brief		Button control
	*/
	class ButtonCtrl
		: public Control
	{
	public:
		/** Constructor
		 *\param[in]	engine	The engine
		 *\param[in]	p_parent	The parent control, if any
		 *\param[in]	p_id		The control ID
		 */
		ButtonCtrl( castor::String const & p_name
			, castor3d::Engine & engine
			, ControlRPtr p_parent
			, uint32_t p_id );

		/** Constructor
		 *\param[in]	engine		The engine
		 *\param[in]	p_parent		The parent control, if any
		 *\param[in]	p_caption		The caption
		 *\param[in]	p_id			The control ID
		 *\param[in]	p_position		The position
		 *\param[in]	p_size			The size
		 *\param[in]	p_style			The style
		 *\param[in]	p_visible		Initial visibility status
		 */
		ButtonCtrl( castor::String const & p_name
			, castor3d::Engine & engine
			, ControlRPtr p_parent
			, uint32_t p_id
			, castor::String const & p_caption
			, castor::Position const & p_position
			, castor::Size const & p_size
			, uint32_t p_style = 0
			, bool p_visible = true );

		/** Destructor
		*/
		virtual ~ButtonCtrl();

		/** sets the text material.
		 *\param[in]	p_material	The new value.
		 */
		void setTextMaterial( castor3d::MaterialRPtr p_material );

		/** sets the highlighted button background material.
		*\param[in]	p_material	The new value.
		*/
		void setHighlightedBackgroundMaterial( castor3d::MaterialRPtr p_material );

		/** sets the highlighted button foreground material.
		*\param[in]	p_material	The new value.
		*/
		void setHighlightedForegroundMaterial( castor3d::MaterialRPtr p_material );

		/** sets the highlighted button text material.
		*\param[in]	p_material	The new value.
		*/
		void setHighlightedTextMaterial( castor3d::MaterialRPtr p_material );

		/** sets the pushed button background material.
		*\param[in]	p_material	The new value.
		*/
		void setPushedBackgroundMaterial( castor3d::MaterialRPtr p_material );

		/** sets the pushed button foreground material.
		*\param[in]	p_material	The new value.
		*/
		void setPushedForegroundMaterial( castor3d::MaterialRPtr p_material );

		/** sets the pushed button text material.
		*\param[in]	p_material	The new value.
		*/
		void setPushedTextMaterial( castor3d::MaterialRPtr p_material );

		/** sets the caption font.
		*\param[in]	p_font	The new value.
		*/
		void setFont( castor::String const & p_font );

		/** sets the horizontal alignment for the text.
		*\param[in]	p_align	The new value.
		*/
		void setHAlign( castor3d::HAlign p_align );

		/** sets the vertical alignment for the text.
		*\param[in]	p_align	The new value.
		*/
		void setVAlign( castor3d::VAlign p_align );

		/** Retrieves the caption
		 *\return		The valu
		 */
		inline castor::String const & getCaption()const
		{
			return m_caption;
		}

		/**
		*\return	The text material
		*/
		inline castor3d::MaterialRPtr getTextMaterial()const
		{
			return m_textMaterial;
		}

		/** Connects a function to a button event
		 *\param[in]	p_event			The event type
		 *\param[in]	p_function		The function
		 *\return		The internal function index, to be able to disconnect it
		 */
		inline OnButtonEventConnection connect( ButtonEvent p_event, OnButtonEventFunction p_function )
		{
			return m_signals[size_t( p_event )].connect( p_function );
		}

	private:
		/** @copydoc CastorGui::Control::doCreate
		*/
		void doCreate()override;

		/** @copydoc CastorGui::Control::doDestroy
		*/
		void doDestroy()override;

		/** @copydoc CastorGui::Control::doSetPosition
		*/
		void doSetPosition( castor::Position const & p_value )override;

		/** @copydoc CastorGui::Control::doSetSize
		*/
		void doSetSize( castor::Size const & p_value )override;

		/** @copydoc CastorGui::Control::doSetBackgroundMaterial
		*/
		void doSetBackgroundMaterial( castor3d::MaterialRPtr p_material )override;

		/** @copydoc CastorGui::Control::doSetForegroundMaterial
		*/
		void doSetForegroundMaterial( castor3d::MaterialRPtr p_material )override;

		/** @copydoc CastorGui::Control::doSetCaption
		*/
		void doSetCaption( castor::String const & p_caption )override;

		/** @copydoc CastorGui::Control::doSetVisible
		*/
		void doSetVisible( bool p_visible )override;

		/** Event when mouse enters the control
		 *\param[in]	p_event		The mouse event
		 */
		void onMouseEnter( castor3d::MouseEvent const & p_event );

		/** Event when mouse leaves the control
		 *\param[in]	p_event		The mouse event
		 */
		void onMouseLeave( castor3d::MouseEvent const & p_event );

		/** Event when mouse left button is pressed.
		 *\param[in]	p_event		The mouse event.
		 */
		void onMouseButtonDown( castor3d::MouseEvent const & p_event );

		/** Event when mouse left button is released.
		 *\param[in]	p_event		The mouse event.
		 */
		void onMouseButtonUp( castor3d::MouseEvent const & p_event );

		/** Creates a material with an ambient colour equal to p_material->ambient + p_offset
		 *\param[in]	p_material	The material.
		 *\param[in]	p_offset	The colour offset.
		 *\return		The created material.
		 */
		castor3d::MaterialRPtr doCreateMaterial( castor3d::MaterialRPtr p_material, float p_offset );

	private:
		//! The caption
		castor::String m_caption;
		//! The text overlay used to display the caption
		castor3d::TextOverlayWPtr m_text;
		//! The text material.
		castor3d::MaterialRPtr m_textMaterial;
		//! The highlighted button text material.
		castor3d::MaterialRPtr m_highlightedTextMaterial;
		//! The highlighted button background material.
		castor3d::MaterialRPtr m_highlightedBackgroundMaterial;
		//! The highlighted button foreground material.
		castor3d::MaterialRPtr m_highlightedForegroundMaterial;
		//! The pushed button text material.
		castor3d::MaterialRPtr m_pushedTextMaterial;
		//! The pushed button background material.
		castor3d::MaterialRPtr m_pushedBackgroundMaterial;
		//! The pushed button foreground material.
		castor3d::MaterialRPtr m_pushedForegroundMaterial;
		//! The button events signals
		OnButtonEvent m_signals[size_t( ButtonEvent::eCount )];
	};
}

#endif
