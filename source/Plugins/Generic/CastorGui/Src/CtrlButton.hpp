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
#ifndef ___CI_CTRL_BUTTON_H___
#define ___CI_CTRL_BUTTON_H___

#include "CtrlControl.hpp"

namespace CastorGui
{
	/*!
	 *\author		Sylvain DOREMU
	 *\date		16/02/201
	 *\version		0.1.
	 *\brief		Button contro
	*/
	class ButtonCtrl
		: public Control
	{
	public:
		using OnEventFunction = std::function< void() >;
		using OnEvent = Castor::Signal< OnEventFunction >;

	public:
		/** Constructor
		 *\param[in]	p_engine	The engine
		 *\param[in]	p_parent	The parent control, if any
		 *\param[in]	p_id		The control ID
		 */
		ButtonCtrl( Castor3D::Engine * p_engine, ControlRPtr p_parent, uint32_t p_id );

		/** Constructor
		 *\param[in]	p_engine		The engine
		 *\param[in]	p_parent		The parent control, if any
		 *\param[in]	p_caption		The caption
		 *\param[in]	p_id			The control ID
		 *\param[in]	p_position		The position
		 *\param[in]	p_size			The size
		 *\param[in]	p_style			The style
		 *\param[in]	p_visible		Initial visibility status
		 */
		ButtonCtrl( Castor3D::Engine * p_engine, ControlRPtr p_parent, uint32_t p_id, Castor::String const & p_caption, Castor::Position const & p_position, Castor::Size const & p_size, uint32_t p_style = 0, bool p_visible = true );

		/** Destructor
		*/
		virtual ~ButtonCtrl();

		/** Sets the caption
		 *\param[in]	p_value		The new value
		 */
		void SetCaption( Castor::String const & p_value );

		/** Sets the text material.
		 *\param[in]	p_material	The new value.
		 */
		void SetTextMaterial( Castor3D::MaterialSPtr p_material );

		/** Sets the highlighted button background material.
		*\param[in]	p_material	The new value.
		*/
		void SetHighlightedBackgroundMaterial( Castor3D::MaterialSPtr p_material );

		/** Sets the highlighted button foreground material.
		*\param[in]	p_material	The new value.
		*/
		void SetHighlightedForegroundMaterial( Castor3D::MaterialSPtr p_material );

		/** Sets the highlighted button text material.
		*\param[in]	p_material	The new value.
		*/
		void SetHighlightedTextMaterial( Castor3D::MaterialSPtr p_material );

		/** Sets the pushed button background material.
		*\param[in]	p_material	The new value.
		*/
		void SetPushedBackgroundMaterial( Castor3D::MaterialSPtr p_material );

		/** Sets the pushed button foreground material.
		*\param[in]	p_material	The new value.
		*/
		void SetPushedForegroundMaterial( Castor3D::MaterialSPtr p_material );

		/** Sets the pushed button text material.
		*\param[in]	p_material	The new value.
		*/
		void SetPushedTextMaterial( Castor3D::MaterialSPtr p_material );

		/** Sets the caption font.
		*\param[in]	p_font	The new value.
		*/
		void SetFont( Castor::String const & p_font );

		/** Retrieves the caption
		 *\return		The valu
		 */
		inline Castor::String const & GetCaption()const
		{
			return m_caption;
		}

		/** Connects a function to a button event
		 *\param[in]	p_event			The event type
		 *\param[in]	p_function		The function
		 *\return		The internal function index, to be able to disconnect it
		 */
		inline OnEvent::Connection Connect( ButtonEvent p_event, OnEventFunction p_function )
		{
			return m_signals[size_t( p_event )].connect( p_function );
		}

	private:
		/** @copydoc CastorGui::Control::DoCreate
		*/
		virtual void DoCreate();

		/** @copydoc CastorGui::Control::DoDestroy
		*/
		virtual void DoDestroy();

		/** @copydoc CastorGui::Control::DoSetPosition
		*/
		virtual void DoSetPosition( Castor::Position const & p_value );

		/** @copydoc CastorGui::Control::DoSetSize
		*/
		virtual void DoSetSize( Castor::Size const & p_value );

		/** @copydoc CastorGui::Control::DoSetBackgroundMaterial
		*/
		virtual void DoSetBackgroundMaterial( Castor3D::MaterialSPtr p_material );

		/** @copydoc CastorGui::Control::DoSetForegroundMaterial
		*/
		virtual void DoSetForegroundMaterial( Castor3D::MaterialSPtr p_material );

		/** @copydoc CastorGui::Control::DoSetVisible
		*/
		virtual void DoSetVisible( bool p_visible );

		/** Event when mouse enters the control
		 *\param[in]	p_event		The mouse event
		 */
		void OnMouseEnter( Castor3D::MouseEvent const & p_event );

		/** Event when mouse leaves the control
		 *\param[in]	p_event		The mouse event
		 */
		void OnMouseLeave( Castor3D::MouseEvent const & p_event );

		/** Event when mouse left button is pressed.
		 *\param[in]	p_event		The mouse event.
		 */
		void OnMouseButtonDown( Castor3D::MouseEvent const & p_event );

		/** Event when mouse left button is released.
		 *\param[in]	p_event		The mouse event.
		 */
		void OnMouseButtonUp( Castor3D::MouseEvent const & p_event );

		/** Creates a material with an ambient colour equal to p_material->ambient + p_offset
		 *\param[in]	p_material	The material.
		 *\param[in]	p_offset	The colour offset.
		 *\return		The created material.
		 */
		Castor3D::MaterialSPtr DoCreateMaterial( Castor3D::MaterialSPtr p_material, float p_offset );

	private:
		//! The caption
		Castor::String m_caption;
		//! The text overlay used to display the caption
		Castor3D::TextOverlayWPtr m_text;
		//! The text material.
		Castor3D::MaterialWPtr m_textMaterial;
		//! The highlighted button text material.
		Castor3D::MaterialWPtr m_highlightedTextMaterial;
		//! The highlighted button background material.
		Castor3D::MaterialWPtr m_highlightedBackgroundMaterial;
		//! The highlighted button foreground material.
		Castor3D::MaterialWPtr m_highlightedForegroundMaterial;
		//! The pushed button text material.
		Castor3D::MaterialWPtr m_pushedTextMaterial;
		//! The pushed button background material.
		Castor3D::MaterialWPtr m_pushedBackgroundMaterial;
		//! The pushed button foreground material.
		Castor3D::MaterialWPtr m_pushedForegroundMaterial;
		//! The button events signals
		OnEvent m_signals[size_t( ButtonEvent::eCount )];
	};
}

#endif
