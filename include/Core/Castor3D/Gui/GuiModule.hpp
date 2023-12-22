/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GuiModule_H___
#define ___C3D_GuiModule_H___

#include "Castor3D/Material/Pass/PassModule.hpp"
#include "Castor3D/Material/Texture/TextureModule.hpp"
#include "Castor3D/Overlay/OverlayModule.hpp"

#include <CastorUtils/Design/FlagCombination.hpp>
#include <CastorUtils/Design/Signal.hpp>
#include <CastorUtils/Graphics/Colour.hpp>

namespace castor3d
{
	using ControlID = size_t;
	using ControlFlagType = uint64_t;
	/**
	*\brief	Enumeration of supported control types.
	*/
	enum class ControlType
		: uint8_t
	{
		eNone,
		//! PanelCtrl
		ePanel,
		//! StaticCtrl
		eStatic,
		//! EditCtrl
		eEdit,
		//! SliderCtrl
		eSlider,
		//! ComboBoxCtrl
		eComboBox,
		//! ListBoxCtrl
		eListBox,
		//! ButtonCtrl
		eButton,
		//! ExpandablePanelCtrl
		eExpandablePanel,
		//! FrameCtrl
		eFrame,
		//! ScrollBarCtrl
		eScrollBar,
		//! ProgressCtrl
		eProgress,
	};
	/**
	*\brief	Enumeration of supported control events.
	*/
	enum class ControlEvent
		: uint8_t
	{
		//! Control dragged.
		eDrag,
		//! Control dropped.
		eDrop,
		//! The control events count.
		eCount,
	};
	/**
	*\brief	Enumeration of supported combobox events.
	*/
	enum class ComboBoxEvent
		: uint8_t
	{
		//! Item selected.
		eSelected,
		//! The combo box events count.
		eCount,
	};
	/**
	*\brief	Enumeration of supported edit events.
	*/
	enum class EditEvent
		: uint8_t
	{
		//! The edit text has changed.
		eUpdated,
		//! The return key has been hit (the edit must have EditStyle::eProcessEnter style).
		eReturn,
		//! The edit events count.
		eCount,
	};
	/**
	*\brief	Enumeration of supported expandable panel events.
	*/
	enum class ExpandablePanelEvent
		: uint8_t
	{
		//! Expand panel.
		eExpand,
		//! Retract panel, leaving only header visible.
		eRetract,
		//! The expandable panel events count.
		eCount,
	};
	/**
	*\brief	Enumeration of supported listbox events.
	*/
	enum class ListBoxEvent
		: uint8_t
	{
		//! Item selected.
		eSelected,
		//! The combobox events count.
		eCount,
	};
	/**
	*\brief	Enumeration of supported button events.
	*/
	enum class ButtonEvent
		: uint8_t
	{
		//! Button clicked.
		eClicked,
		//! The button events count.
		eCount,
	};
	/**
	*\brief	Enumeration of supported slider events.
	*/
	enum class SliderEvent
		: uint8_t
	{
		//! Slider thumb release.
		eThumbRelease,
		//! Slider thumb track.
		eThumbTrack,
		//! The slider events count.
		eCount,
	};
	/**
	*\brief	Enumeration of supported combobox events.
	*/
	enum class ScrollBarEvent
		: uint8_t
	{
		//! The user has dropped the scroll thumb.
		eThumbRelease,
		//! The user is dragging the scroll thumb.
		eThumbTrack,
		//! The scrollbar events count.
		eCount,
	};
	/**
	*\brief	Listbox control supported flags.
	*/
	enum class ControlFlag
		: ControlFlagType
	{
		//! Control is detached and appears on top of every other one.
		eAlwaysOnTop = 0x0000000000000001,
		//! Control can be moved around.
		eMovable = 0x0000000000000002,
		//! Control can be resized.
		eResizable = 0x0000000000000004,
	};
	CU_ImplementFlags( ControlFlag )
	/**
	*\brief	Combo control supported flags.
	*/
	enum class ComboBoxFlag
		: ControlFlagType
	{
		//! read only combo box
		eReadOnly = 0x0000000000000100,
	};
	CU_ImplementFlags( ComboBoxFlag )
	/**
	*\brief	Edit control supported flags.
	*/
	enum class EditFlag
		: ControlFlagType
	{
		//! The edit control process 'return' hit as an event and not as a newline.
		eProcessEnter = 0x0000000000001000,
		//! The edit control is a multiline edit control.
		eMultiline = 0x0000000000002000,
		//! The edit control process 'tab' hit as the tab character and not as an event.
		eProcessTab = 0x0000000000004000,
		//! Words are not cut.
		eWordWrap = 0x0000000000008000,
	};
	CU_ImplementFlags( EditFlag )
	/**
	*\brief	Slider control supported flags.
	*/
	enum class SliderFlag
		: ControlFlagType
	{
		// The slider is displayed horizontally (default mode).
		eHorizontal = 0x0000000000100000,
		// The slider is displayed vertically.
		eVertical = 0x0000000000200000,
	};
	CU_ImplementFlags( SliderFlag )
	/**
	*\brief	Static control supported flags.
	*/
	enum class StaticFlag
		: ControlFlagType
	{
		//! The static's text is left aligned.
		eHAlignLeft = 0x0000000001000000,
		//! The static's text is horizontally centered.
		eHAlignCenter = 0x0000000002000000,
		//! The static's text is right aligned.
		eHAlignRight = 0x0000000004000000,
		//! The static's text is aligned on top.
		eVAlignTop = 0x0000000010000000,
		//! The static's text is vertically centered.
		eVAlignCenter = 0x0000000020000000,
		//! The static's text is aligned on bottom.
		eVAlignBottom = 0x0000000040000000,
	};
	CU_ImplementFlags( StaticFlag )
	/**
	*\brief	ScrollBar control supported flags.
	*/
	enum class ScrollBarFlag
		: ControlFlagType
	{
		// The scrollbar is displayed horizontally (default mode).
		eHorizontal = 0x0000000100000000,
		// The scrollbar is displayed vertically.
		eVertical = 0x0000000200000000,
	};
	CU_ImplementFlags( ScrollBarFlag )
	/**
	*\brief	Progress control supported flags.
	*/
	enum class ProgressFlag
		: ControlFlagType
	{
		// The progress is shown from left to right (default mode).
		eLeftRight = 0x0000001000000000,
		// The progress is shown from right to left.
		eRightLeft = 0x0000002000000000,
		// The progress is shown from top to bottom.
		eTopBottom = 0x0000004000000000,
		// The progress is shown from bottom to top.
		eBottomTop = 0x0000008000000000,
		// The progress has a title.
		eHasTitle = 0x0000010000000000,
	};
	CU_ImplementFlags( ProgressFlag )
	/**
	*\brief	Enumeration of supported GUI events in ControlInfos.
	*/
	enum class GuiEvent
		: uint8_t
	{
		//! Event to ask the main window to show a message box.
		eShowMessageBox,
		//! Event to ask the main window to resize to the given size.
		eResizeParent,
	};
	/**
	*\brief	The flags for a specific control in a layout.
	*/
	struct LayoutItemFlags;
	/**
	*\brief		GUI parsing context.
	*/
	struct GuiContext;
	/**
	*\brief		Class used to to manage the controls: events and all GUI related stuff.
	*/
	class ControlsManager;
	/**
	*\brief		Description of a control.
	*/
	class Control;
	/**
	*\brief		Control with a layout.
	*/
	class LayoutControl;
	/**
	*\brief		Button control.
	*/
	class ButtonCtrl;
	/**
	*\brief		ComboBox control.
	*/
	class ComboBoxCtrl;
	/**
	*\brief		Edit control.
	*/
	class EditCtrl;
	/**
	*\brief		A panel with a header, expandable and retractable.
	*/
	class ExpandablePanelCtrl;
	/**
	*\brief		A movable/resizable panel with an optional title bar.
	*/
	class FrameCtrl;
	/**
	*\brief		ListBox control.
	*/
	class ListBoxCtrl;
	/**
	*\brief		Panel control.
	*/
	class PanelCtrl;
	/**
	*\brief		Progress control.
	*/
	class ProgressCtrl;
	/**
	*\brief		Holds together a control and its scrollbars.
	*/
	class ScrollableCtrl;
	/**
	*\brief		Scrollbar control.
	*/
	class ScrollBarCtrl;
	/**
	*\brief		Slider control.
	*/
	class SliderCtrl;
	/**
	*\brief		Static control.
	*/
	class StaticCtrl;
	/**
	*\brief		Base class to order controls.
	*/
	class Layout;
	/**
	*\brief		Simple box layout, to push controls in a direction.
	*\remarks	The direction can be horizontal or vertical, vertical being the default.
	*/
	class LayoutBox;
	/**
	*\brief		A space in a layout.
	*/
	class Spacer;
	/**
	 *\brief	Creates and holds styles.
	*/
	class StylesHolder;
	/**
	 *\brief	Regroups styles for all control types.
	*/
	class Theme;
	/**
	*\brief		Description of the style of a control.
	*/
	class ControlStyle;
	/**
	*\brief		Button style.
	*/
	class ButtonStyle;
	/**
	*\brief		ComboBox style.
	*/
	class ComboBoxStyle;
	/**
	*\brief		Edit style.
	*/
	class EditStyle;
	/**
	*\brief		ExpandablePanel style.
	*/
	class ExpandablePanelStyle;
	/**
	*\brief		Frame style.
	*/
	class FrameStyle;
	/**
	*\brief		ListBox style.
	*/
	class ListBoxStyle;
	/**
	*\brief		Panel style.
	*/
	class PanelStyle;
	/**
	*\brief		Progress style.
	*/
	class ProgressStyle;
	/**
	*\brief		Holds scrollbars styles.
	*/
	class ScrollableStyle;
	/**
	*\brief		Scrollbar style.
	*/
	class ScrollBarStyle;
	/**
	*\brief		Slider style.
	*/
	class SliderStyle;
	/**
	*\brief		Static style.
	*/
	class StaticStyle;

	using OnControlEventFunction = std::function< void() >;
	using OnControlEvent = castor::SignalT< OnControlEventFunction >;
	using OnControlEventConnection = OnControlEvent::connection;

	using OnButtonEventFunction = std::function< void() >;
	using OnButtonEvent = castor::SignalT< OnButtonEventFunction >;
	using OnButtonEventConnection = OnButtonEvent::connection;

	using OnComboEventFunction = std::function< void( int32_t ) >;
	using OnComboEvent = castor::SignalT< OnComboEventFunction >;
	using OnComboEventConnection = OnComboEvent::connection;

	using OnEditEventFunction = std::function< void( castor::U32String const & ) >;
	using OnEditEvent = castor::SignalT< OnEditEventFunction >;
	using OnEditEventConnection = OnEditEvent::connection;

	using OnExpandablePanelEventFunction = std::function< void() >;
	using OnExpandablePanelEvent = castor::SignalT< OnExpandablePanelEventFunction >;
	using OnExpandablePanelEventConnection = OnExpandablePanelEvent::connection;

	using OnListEventFunction = std::function< void( int32_t ) >;
	using OnListEvent = castor::SignalT< OnListEventFunction >;
	using OnListEventConnection = OnListEvent::connection;

	using OnSliderEventFunction = std::function< void( int32_t ) >;
	using OnSliderEvent = castor::SignalT< OnSliderEventFunction >;
	using OnSliderEventConnection = OnSliderEvent::connection;

	using OnScrollBarEventFunction = std::function< void( int32_t ) >;
	using OnScrollBarEvent = castor::SignalT< OnScrollBarEventFunction >;
	using OnScrollBarEventConnection = OnScrollBarEvent::connection;

	using OnControlChangedFunction = std::function< void( Control const & ) >;
	using OnControlChanged = castor::SignalT< OnControlChangedFunction >;
	using OnControlChangedConnection = OnControlChanged::connection;

	CU_DeclareSmartPtr( castor3d, Layout, C3D_API );
	CU_DeclareSmartPtr( castor3d, Spacer, C3D_API );
	CU_DeclareSmartPtr( castor3d, GuiContext, C3D_API );

	CU_DeclareSmartPtr( castor3d, ControlsManager, C3D_API );
	CU_DeclareSmartPtr( castor3d, Control, C3D_API );
	CU_DeclareSmartPtr( castor3d, LayoutControl, C3D_API );
	CU_DeclareSmartPtr( castor3d, ButtonCtrl, C3D_API );
	CU_DeclareSmartPtr( castor3d, ComboBoxCtrl, C3D_API );
	CU_DeclareSmartPtr( castor3d, EditCtrl, C3D_API );
	CU_DeclareSmartPtr( castor3d, ExpandablePanelCtrl, C3D_API );
	CU_DeclareSmartPtr( castor3d, FrameCtrl, C3D_API );
	CU_DeclareSmartPtr( castor3d, ListBoxCtrl, C3D_API );
	CU_DeclareSmartPtr( castor3d, PanelCtrl, C3D_API );
	CU_DeclareSmartPtr( castor3d, ProgressCtrl, C3D_API );
	CU_DeclareSmartPtr( castor3d, ScrollableCtrl, C3D_API );
	CU_DeclareSmartPtr( castor3d, ScrollBarCtrl, C3D_API );
	CU_DeclareSmartPtr( castor3d, SliderCtrl, C3D_API );
	CU_DeclareSmartPtr( castor3d, StaticCtrl, C3D_API );
	CU_DeclareSmartPtr( castor3d, StylesHolder, C3D_API );
	CU_DeclareSmartPtr( castor3d, Theme, C3D_API );
	CU_DeclareSmartPtr( castor3d, ControlStyle, C3D_API );
	CU_DeclareSmartPtr( castor3d, ButtonStyle, C3D_API );
	CU_DeclareSmartPtr( castor3d, ComboBoxStyle, C3D_API );
	CU_DeclareSmartPtr( castor3d, EditStyle, C3D_API );
	CU_DeclareSmartPtr( castor3d, ExpandablePanelStyle, C3D_API );
	CU_DeclareSmartPtr( castor3d, FrameStyle, C3D_API );
	CU_DeclareSmartPtr( castor3d, ListBoxStyle, C3D_API );
	CU_DeclareSmartPtr( castor3d, PanelStyle, C3D_API );
	CU_DeclareSmartPtr( castor3d, ProgressStyle, C3D_API );
	CU_DeclareSmartPtr( castor3d, ScrollableStyle, C3D_API );
	CU_DeclareSmartPtr( castor3d, ScrollBarStyle, C3D_API );
	CU_DeclareSmartPtr( castor3d, SliderStyle, C3D_API );
	CU_DeclareSmartPtr( castor3d, StaticStyle, C3D_API );

	using ControlArray = std::vector< ControlRPtr >;

	template< typename TypeT >
	concept ControlFlagTypeT = ( std::is_same_v< TypeT, ControlFlagType >
		|| std::is_same_v< std::underlying_type_t< TypeT >, ControlFlagType > );

	/** Sets the pass colour.
	 *\param[in]	pass	The pass.
	 *\param[in]	colour	The material colour.
	 */
	void setMaterialColour( Pass & pass, castor::HdrRgbColour const & colour );

	/** Retrieves the pass colour.
	 *\param[in]	pass	The pass.
	 *\return		The material colour.
	 */
	castor::HdrRgbColour const & getMaterialColour( Pass const & pass );
	/** Creates a colour material.
	 *\param[in]	engine	The engine.
	 *\param[in]	name	The material name.
	 *\param[in]	colour	The material colour.
	 *\return		The created material.
	*/
	MaterialObs createMaterial( Engine & engine, castor::String const & name, castor::HdrRgbColour const & colour );
}

#endif
