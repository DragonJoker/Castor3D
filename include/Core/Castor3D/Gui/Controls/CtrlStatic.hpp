/*
See LICENSE file in root folder
*/
#ifndef ___C3D_StaticCtrl_H___
#define ___C3D_StaticCtrl_H___

#include "Castor3D/Gui/Controls/CtrlControl.hpp"
#include "Castor3D/Gui/Theme/StyleStatic.hpp"

namespace castor3d
{
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
		StaticCtrl( SceneRPtr scene
			, castor::String const & name
			, StaticStyle * style
			, ControlRPtr parent );

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
		StaticCtrl( SceneRPtr scene
			, castor::String const & name
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
		void setHAlign( HAlign value );

		/** Sets the vertical alignment for the text.
		*\param[in]	value	The new value.
		*/
		void setVAlign( VAlign value );

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
		void doSetCaption( castor::String const & value )override;

		/** @copydoc Control::doSetVisible
		*/
		void doSetVisible( bool visible )override;

		/** @copydoc Control::doUpdateFlags
		*/
		void doUpdateFlags()override;

	private:
		castor::String m_caption;
		TextOverlayWPtr m_text;
	};
}

#endif
