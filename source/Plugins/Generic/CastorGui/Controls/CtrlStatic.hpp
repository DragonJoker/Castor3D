/*
See LICENSE file in root folder
*/
#ifndef ___CI_CTRL_STATIC_H___
#define ___CI_CTRL_STATIC_H___

#include "CastorGui/Controls/CtrlControl.hpp"
#include "CastorGui/Theme/StyleStatic.hpp"

namespace CastorGui
{
	/**
	 *\brief	Static control.
	*/
	class StaticCtrl
		: public Control
	{
	public:
		/** Constructor
		 *\param[in]	name	The control name
		 *\param[in]	style	The control style
		 *\param[in]	parent	The parent control, if any
		 *\param[in]	id		The control ID.
		*/
		StaticCtrl( castor::String const & name
			, StaticStyle * style
			, ControlRPtr parent
			, uint32_t id );

		/** Constructor
		 *\param[in]	name		The control name
		 *\param[in]	style		The control style
		 *\param[in]	parent		The parent control, if any
		 *\param[in]	caption		The static caption
		 *\param[in]	position	The control position
		 *\param[in]	size		The control size
		 *\param[in]	flags		The configuration flags
		 *\param[in]	visible		Initial visibility status
		 */
		StaticCtrl( castor::String const & name
			, StaticStyle * style
			, ControlRPtr parent
			, castor::String const & caption
			, castor::Position const & position
			, castor::Size const & size
			, uint32_t flags = 0
			, bool visible = true );

		/** Sets the horizontal alignment for the text.
		*\param[in]	value	The new value.
		*/
		void setHAlign( castor3d::HAlign value );

		/** Sets the vertical alignment for the text.
		*\param[in]	value	The new value.
		*/
		void setVAlign( castor3d::VAlign value );

		/**
		*\return	The static caption
		*/
		castor::String const & getCaption()const
		{
			return m_caption;
		}

		/**
		*\return	The static style
		*/
		StaticStyle const & getStyle()const
		{
			return static_cast< StaticStyle const & >( getBaseStyle() );
		}

		static ControlType constexpr Type{ ControlType::eStatic };

	private:
		StaticStyle & getStyle()
		{
			return static_cast< StaticStyle & >( getBaseStyle() );
		}
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

		/** @copydoc CastorGui::Control::doUpdateStyle
		*/
		void doUpdateStyle()override;

		/** @copydoc CastorGui::Control::doSetCaption
		*/
		void doSetCaption( castor::String const & p_value )override;

		/** @copydoc CastorGui::Control::doSetVisible
		*/
		void doSetVisible( bool p_visible )override;

		/** @copydoc CastorGui::Control::doUpdateFlags
		*/
		void doUpdateFlags()override;

	private:
		castor::String m_caption;
		castor3d::TextOverlayWPtr m_text;
		static uint32_t m_count;
	};
}

#endif
