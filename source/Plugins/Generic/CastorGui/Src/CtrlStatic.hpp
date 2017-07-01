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
#ifndef ___CI_CTRL_STATIC_H___
#define ___CI_CTRL_STATIC_H___

#include "CtrlControl.hpp"

namespace CastorGui
{
	/*!
	 *\author		Sylvain DOREMUS
	 *\version		0.1.0
	 *\brief		Description of a control, in order to be able to create it completely
	*/
	class StaticCtrl
		: public Control
	{
	public:
		/** Constructor
		 *\param[in]	p_engine	The engine
		 *\param[in]	p_parent	The parent control, if any
		 *\param[in]	p_id		The control ID.
		*/
		StaticCtrl( Castor::String const & p_name
			, Castor3D::Engine & p_engine
			, ControlRPtr p_parent
			, uint32_t p_id );

		/** Constructor
		 *\param[in]	p_engine	The engine
		 *\param[in]	p_parent	The parent control, if any
		 *\param[in]	p_caption	The static caption
		 *\param[in]	p_position	The control position
		 *\param[in]	p_size		The control siz
		 *\param[in]	p_style		The control styly
		 *\param[in]	p_visible	Initial visibility status
		 */
		StaticCtrl( Castor::String const & p_name
			, Castor3D::Engine & p_engine
			, ControlRPtr p_parent
			, Castor::String const & p_caption
			, Castor::Position const & p_position
			, Castor::Size const & p_size
			, uint32_t p_style = 0
			, bool p_visible = true );

		/** Destructor
		*/
		virtual ~StaticCtrl();

		/** Sets the caption font.
		*\param[in]	p_font	The new value.
		*/
		void SetFont( Castor::String const & p_font );

		/** Sets the horizontal alignment for the text.
		*\param[in]	p_align	The new value.
		*/
		void SetHAlign( Castor3D::HAlign p_align );

		/** Sets the vertical alignment for the text.
		*\param[in]	p_align	The new value.
		*/
		void SetVAlign( Castor3D::VAlign p_align );

		/** Retrieve the static caption
		 *\return		The value
		*/
		inline Castor::String const & GetCaption()const
		{
			return m_caption;
		}

	private:
		/** @copydoc CastorGui::Control::DoCreate
		*/
		virtual void DoCreate()override;

		/** @copydoc CastorGui::Control::DoDestroy
		*/
		virtual void DoDestroy()override;

		/** @copydoc CastorGui::Control::DoSetPosition
		*/
		virtual void DoSetPosition( Castor::Position const & p_value )override;

		/** @copydoc CastorGui::Control::DoSetSize
		*/
		virtual void DoSetSize( Castor::Size const & p_value )override;

		/** @copydoc CastorGui::Control::DoSetBackgroundMaterial
		*/
		virtual void DoSetBackgroundMaterial( Castor3D::MaterialSPtr p_material )override;

		/** @copydoc CastorGui::Control::DoSetForegroundMaterial
		*/
		virtual void DoSetForegroundMaterial( Castor3D::MaterialSPtr p_material )override;

		/** @copydoc CastorGui::Control::DoSetCaption
		*/
		void DoSetCaption( Castor::String const & p_value )override;

		/** @copydoc CastorGui::Control::DoSetVisible
		*/
		virtual void DoSetVisible( bool p_visible )override;

		/** @copydoc CastorGui::Control::DoUpdateStyle
		*/
		virtual void DoUpdateStyle()override;

	private:
		//! The static caption
		Castor::String m_caption;

	protected:
		//! The text overlay used to display the caption
		Castor3D::TextOverlayWPtr m_text;
		// The statics count
		static int m_count;
	};
}

#endif
