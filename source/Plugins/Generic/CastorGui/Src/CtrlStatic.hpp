﻿/*
See LICENSE file in root folder
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
		 *\param[in]	engine	The engine
		 *\param[in]	p_parent	The parent control, if any
		 *\param[in]	p_id		The control ID.
		*/
		StaticCtrl( castor::String const & p_name
			, castor3d::Engine & engine
			, ControlRPtr p_parent
			, uint32_t p_id );

		/** Constructor
		 *\param[in]	engine	The engine
		 *\param[in]	p_parent	The parent control, if any
		 *\param[in]	p_caption	The static caption
		 *\param[in]	p_position	The control position
		 *\param[in]	p_size		The control siz
		 *\param[in]	p_style		The control styly
		 *\param[in]	p_visible	Initial visibility status
		 */
		StaticCtrl( castor::String const & p_name
			, castor3d::Engine & engine
			, ControlRPtr p_parent
			, castor::String const & p_caption
			, castor::Position const & p_position
			, castor::Size const & p_size
			, uint32_t p_style = 0
			, bool p_visible = true );

		/** Destructor
		*/
		virtual ~StaticCtrl();

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

		/** Retrieve the static caption
		 *\return		The value
		*/
		inline castor::String const & getCaption()const
		{
			return m_caption;
		}

	private:
		/** @copydoc CastorGui::Control::doCreate
		*/
		virtual void doCreate()override;

		/** @copydoc CastorGui::Control::doDestroy
		*/
		virtual void doDestroy()override;

		/** @copydoc CastorGui::Control::doSetPosition
		*/
		virtual void doSetPosition( castor::Position const & p_value )override;

		/** @copydoc CastorGui::Control::doSetSize
		*/
		virtual void doSetSize( castor::Size const & p_value )override;

		/** @copydoc CastorGui::Control::doSetBackgroundMaterial
		*/
		virtual void doSetBackgroundMaterial( castor3d::MaterialSPtr p_material )override;

		/** @copydoc CastorGui::Control::doSetForegroundMaterial
		*/
		virtual void doSetForegroundMaterial( castor3d::MaterialSPtr p_material )override;

		/** @copydoc CastorGui::Control::doSetCaption
		*/
		void doSetCaption( castor::String const & p_value )override;

		/** @copydoc CastorGui::Control::doSetVisible
		*/
		virtual void doSetVisible( bool p_visible )override;

		/** @copydoc CastorGui::Control::doUpdateStyle
		*/
		virtual void doUpdateStyle()override;

	private:
		//! The static caption
		castor::String m_caption;

	protected:
		//! The text overlay used to display the caption
		castor3d::TextOverlayWPtr m_text;
		// The statics count
		static int m_count;
	};
}

#endif
