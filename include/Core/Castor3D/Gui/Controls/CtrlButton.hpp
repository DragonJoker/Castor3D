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
		 *\param[in]	scene	The parent scene (nullptr for global).
		 *\param[in]	name	The control name
		 *\param[in]	style	The control style
		 *\param[in]	parent	The parent control, if any
		 */
		C3D_API ButtonCtrl( SceneRPtr scene
			, castor::String const & name
			, ButtonStyleRPtr style
			, ControlRPtr parent );

		/** Constructor
		 *\param[in]	scene		The parent scene (nullptr for global).
		 *\param[in]	name		The control name
		 *\param[in]	style		The control style
		 *\param[in]	parent		The parent control, if any
		 *\param[in]	caption		The caption
		 *\param[in]	position	The position
		 *\param[in]	size		The size
		 *\param[in]	flags		The configuration flags
		 *\param[in]	visible		Initial visibility status
		 */
		C3D_API ButtonCtrl( SceneRPtr scene
			, castor::String const & name
			, ButtonStyleRPtr style
			, ControlRPtr parent
			, castor::U32String const & caption
			, castor::Position const & position
			, castor::Size const & size
			, ControlFlagType flags = 0
			, bool visible = true );
		C3D_API ~ButtonCtrl()noexcept override;

		/** Sets the horizontal alignment for the text.
		*\param[in]	align	The new value.
		*/
		C3D_API void setHAlign( HAlign align );

		/** Sets the vertical alignment for the text.
		*\param[in]	align	The new value.
		*/
		C3D_API void setVAlign( VAlign align );

		/** \return	The horizontal alignment for the text.
		*/
		C3D_API HAlign getHAlign()const;

		/** \return	The vertical alignment for the text.
		*/
		C3D_API VAlign getVAlign()const;

		/** Retrieves the caption
		 *\return		The value
		 */
		castor::U32String const & getCaption()const
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
		*\return	The button style.
		*/
		ButtonStyle const & getStyle()const
		{
			return static_cast< ButtonStyle const & >( getBaseStyle() );
		}

		C3D_API static ControlType constexpr Type{ ControlType::eButton };

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

		/** @copydoc Control::doSetBorderSize
		*/
		void doSetBorderSize( castor::Point4ui const & value )override;

		/** @copydoc Control::doUpdateStyle
		*/
		void doUpdateStyle()override;

		/** @copydoc Control::doSetCaption
		*/
		void doSetCaption( castor::U32String const & caption )override;

		/** @copydoc Control::doSetVisible
		*/
		void doSetVisible( bool visible )override;

		/** @copydoc Control::doUpdateFlags
		*/
		void doUpdateFlags()override;

		/** @copydoc Control::doOnMouseEnter
		*/
		void doOnMouseEnter( MouseEvent const & event )override;

		/** @copydoc Control::doOnMouseButtonDown
		*/
		void doOnMouseButtonDown( MouseEvent const & event )override;

		/** @copydoc Control::doOnMouseButtonUp
		*/
		void doOnMouseButtonUp( MouseEvent const & event )override;

		/** @copydoc Control::doOnMouseLeave
		*/
		void doOnMouseLeave( MouseEvent const & event )override;

		/** @copydoc Control::doUpdateZIndex
		*/
		void doUpdateZIndex( uint32_t & index )override;

		/** @copydoc Control::doAdjustZIndex
		*/
		void doAdjustZIndex( uint32_t offset )override;

		/** Creates a material with an ambient colour equal to material->colour + offset
		 *\param[in]	material	The material.
		 *\param[in]	offset		The colour offset.
		 *\return		The created material.
		 */
		MaterialObs doCreateMaterial( MaterialObs material
			, float offset );

	private:
		castor::U32String m_caption;
		TextOverlayRPtr m_text{};
		OnButtonEvent m_signals[size_t( ButtonEvent::eCount )];
		OnEnableConnection m_onEnable;
	};
}

#endif
