/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ProgressCtrl_H___
#define ___C3D_ProgressCtrl_H___

#include "Castor3D/Gui/Controls/CtrlControl.hpp"
#include "Castor3D/Gui/Theme/StyleProgress.hpp"

namespace castor3d
{
	class ProgressCtrl
		: public Control
	{
	public:
		/** Constructor
		 *\param[in]	name	The control name
		 *\param[in]	style	The control style
		 *\param[in]	parent	The parent control, if any
		 *\param[in]	id		The control ID.
		*/
		C3D_API ProgressCtrl( SceneRPtr scene
			, castor::String const & name
			, ProgressStyle * style
			, ControlRPtr parent );
		C3D_API ~ProgressCtrl()noexcept;

		C3D_API void setTitle( castor::U32String value );
		C3D_API void setRange( castor::Range< int32_t > const & value );
		C3D_API void setProgress( int32_t value );
		C3D_API void setContainerBorderSize( castor::Point4ui value );
		C3D_API void setBarBorderSize( castor::Point4ui value );
		C3D_API void setLeftToRight()noexcept;
		C3D_API void setRightToLeft()noexcept;
		C3D_API void setTopToBottom()noexcept;
		C3D_API void setBottomToTop()noexcept;

		castor::Range< int32_t > const & getRange()const noexcept
		{
			return m_value.range();
		}

		int32_t getProgress()const noexcept
		{
			return m_value.value();
		}

		void incProgress()
		{
			setProgress( m_value.value() + 1 );
		}

		bool isLeftToRight()const noexcept
		{
			return castor::checkFlag( getFlags(), ProgressFlag::eLeftRight );
		}

		bool isRightToLeft()const noexcept
		{
			return castor::checkFlag( getFlags(), ProgressFlag::eRightLeft );
		}

		bool isTopToBottom()const noexcept
		{
			return castor::checkFlag( getFlags(), ProgressFlag::eTopBottom );
		}

		bool isBottomToTop()const noexcept
		{
			return castor::checkFlag( getFlags(), ProgressFlag::eBottomTop );
		}

		bool isVertical()const noexcept
		{
			return isBottomToTop() || isTopToBottom();
		}

		/**
		*\return	The progress style.
		*/
		ProgressStyle const & getStyle()const
		{
			return static_cast< ProgressStyle const & >( getBaseStyle() );
		}

		ProgressStyle & getStyle()
		{
			return static_cast< ProgressStyle & >( getBaseStyle() );
		}

		C3D_API static ControlType constexpr Type{ ControlType::eProgress };

	private:
		/** @copydoc Control::doCreate
		*/
		void doCreate()override;

		/** @copydoc Control::doDestroy
		*/
		void doDestroy()override;

		/** @copydoc Control::doUpdateStyle
		*/
		void doUpdateStyle()override;

		/** @copydoc Control::doUpdateFlags
		*/
		void doUpdateFlags()override;

		/** @copydoc Control::doUpdateZIndex
		*/
		void doUpdateZIndex( uint32_t & index )override;

		/** @copydoc Control::doAdjustZIndex
		*/
		void doAdjustZIndex( uint32_t offset )override;

		/** Sets the background borders size.
		 *\param[in]	value		The new value.
		 */
		void doSetBorderSize( castor::Point4ui const & value )override;

		/** Sets the position
		*\param[in]	value		The new value
		*/
		void doSetPosition( castor::Position const & value )override;

		/** Sets the size
		*\param[in]	value	The new value
		*/
		void doSetSize( castor::Size const & value )override;

		/** Sets the caption.
		*\param[in]	caption	The new value
		*/
		void doSetCaption( castor::U32String const & caption )override;

		/** Sets the visibility
		 *\remarks		Used for derived control specific behavious
		 *\param[in]	value		The new value
		 */
		void doSetVisible( bool visible )override;

	private:
		void doUpdatePosSize();
		void doUpdateProgress();

	private:
		TextOverlayRPtr m_title{};
		PanelCtrlRPtr m_container{};
		PanelCtrlRPtr m_progress{};
		TextOverlayRPtr m_text{};
		castor::RangedValue< int32_t > m_value;
	};
}

#endif
