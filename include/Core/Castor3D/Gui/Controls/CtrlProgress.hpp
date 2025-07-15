/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ProgressCtrl_H___
#define ___C3D_ProgressCtrl_H___

#include "Castor3D/Gui/Controls/CtrlControl.hpp"
#include "Castor3D/Gui/Theme/StyleProgress.hpp"

namespace c3d
{
	class ProgressCtrl
		: public Control
	{
	public:
		/** Constructor
		 *\param[in]	scene	The parent scene (nullptr for global).
		 *\param[in]	name	The control name
		 *\param[in]	style	The control style
		 *\param[in]	parent	The parent control, if any
		*/
		C3D_API ProgressCtrl( SceneRPtr scene
			, String const & name
			, ProgressStyle * style
			, ControlRPtr parent );
		C3D_API ~ProgressCtrl()noexcept override;

		C3D_API void setTitle( U32String value );
		C3D_API void setRange( Range< int32_t > const & value );
		C3D_API void setProgress( int32_t value );
		C3D_API void setContainerBorderSize( Point4ui const & value );
		C3D_API void setBarBorderSize( Point4ui const & value );
		C3D_API void setLeftToRight()noexcept;
		C3D_API void setRightToLeft()noexcept;
		C3D_API void setTopToBottom()noexcept;
		C3D_API void setBottomToTop()noexcept;
		C3D_API void showTitle( bool show )noexcept;

		Range< int32_t > const & getRange()const noexcept
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
			return checkFlag( getFlags(), ProgressFlag::eLeftRight );
		}

		bool isRightToLeft()const noexcept
		{
			return checkFlag( getFlags(), ProgressFlag::eRightLeft );
		}

		bool isTopToBottom()const noexcept
		{
			return checkFlag( getFlags(), ProgressFlag::eTopBottom );
		}

		bool isBottomToTop()const noexcept
		{
			return checkFlag( getFlags(), ProgressFlag::eBottomTop );
		}

		bool hasTitle()const noexcept
		{
			return checkFlag( getFlags(), ProgressFlag::eHasTitle );
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
		void doSetBorderSize( Point4ui const & value )override;

		/** Sets the position
		*\param[in]	value		The new value
		*/
		void doSetPosition( Position const & value )override;

		/** Sets the size
		*\param[in]	value	The new value
		*/
		void doSetSize( Size const & value )override;

		/** Sets the caption.
		*\param[in]	caption	The new value
		*/
		void doSetCaption( U32String const & caption )override;

		/** Sets the visibility
		 *\remarks		Used for derived control specific behavious
		 *\param[in]	visible		The new value
		 */
		void doSetVisible( bool visible )override;

	private:
		void doUpdatePosSize();
		void doUpdateProgress();

	private:
		TextOverlayRPtr m_title{};
		PanelCtrlRPtr m_container{};
		PanelCtrlRPtr m_progress{};
		StaticCtrlRPtr m_text{};
		RangedValue< int32_t > m_value{ makeRangedValue( 0, 0, 100 ) };
	};
}

#endif
