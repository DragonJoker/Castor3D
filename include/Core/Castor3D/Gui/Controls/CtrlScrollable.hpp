/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ScrollableCtrl_H___
#define ___C3D_ScrollableCtrl_H___

#include "Castor3D/Gui/Theme/StyleScrollBar.hpp"

namespace castor3d
{
	C3D_API bool isScrollableControl( ControlType type );
	C3D_API bool isScrollableControl( Control const & control );

	class ScrollableCtrl
	{
	public:
		C3D_API castor::Position getScrollPosition()const;

		ScrollableStyle & getStyle()const noexcept
		{
			return *m_scrollableStyle;
		}

	protected:
		/** Constructor
		 *\param[in]	target	The target control.
		*/
		C3D_API ScrollableCtrl( Control & target
			, ScrollableStyleRPtr style );

		/** @copydoc Control::create
		*/
		C3D_API void createScrollBars();

		/** @copydoc Control::destroy
		*/
		C3D_API void destroyScrollBars();

		/** Updates the scrollbars' style.
		*/
		C3D_API void updateScrollBarsStyle();

		/** Enables/Disables scrollbars, depending on control flags.
		*/
		C3D_API void checkScrollBarFlags();

		/** Updates scrollbars size and position.
		*/
		C3D_API void updateScrollBars();

		/** Updates the given client rect regarding scrollbars dimensions and visibility.
		*/
		C3D_API castor::Point4ui updateScrollableClientRect( castor::Point4ui const & clientRect );

		/** Updates scrollbars thumb from external event.
		*/
		C3D_API void updateScrollBarsThumb( castor::Position const & pos );

		/** Updates scrollbars visibility.
		*/
		C3D_API void setScrollBarsVisible( bool visible );

		/** Updates total scrollable content dimensions.
		*/
		C3D_API void registerControl( Control & control );

		/** Updates total scrollable content dimensions.
		*/
		C3D_API void updateTotalSize( castor::Size const & size );

		/** @copydoc Control::doUpdateZIndex
		*/
		C3D_API void updateScrollZIndex( uint32_t & index );

		/** @copydoc Control::doAdjustZIndex
		*/
		C3D_API void adjustScrollZIndex( uint32_t offset );

		bool hasVerticalScrollBar()const noexcept
		{
			return m_verticalScrollBar != nullptr;
		}

		bool hasHorizontalScrollBar()const noexcept
		{
			return m_horizontalScrollBar != nullptr;
		}

	protected:
		struct ScrolledControl
		{
			OnControlChangedConnection connection;
			castor::Position originalPosition;
		};

		using OnScrollContentFunction = std::function< void( castor::Position const & ) >;
		using OnScrollContent = castor::SignalT< OnScrollContentFunction >;
		using OnScrollContentConnection = OnScrollContent::connection;

		OnScrollContent onScrollContent;

	private:
		void doUpdateControlPosition( Control & ctrl );
		void doUpdateScrollableContent();
		void doScrollContent();

	private:
	private:
		Control & m_target;
		ScrollableStyleRPtr m_scrollableStyle;
		ScrollBarCtrlSPtr m_verticalScrollBar;
		ScrollBarCtrlSPtr m_horizontalScrollBar;
		PanelOverlayWPtr m_corner;
		std::map< ControlRPtr, ScrolledControl > m_controls;
		OnScrollBarEventConnection m_onVerticalThumbRelease;
		OnScrollBarEventConnection m_onHorizontalThumbRelease;
		OnScrollBarEventConnection m_onVerticalThumbTrack;
		OnScrollBarEventConnection m_onHorizontalThumbTrack;
		bool m_updating{};
	};
}

#endif
