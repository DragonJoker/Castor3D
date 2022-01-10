/*
See LICENSE file in root folder
*/
#ifndef ___CI_CASTOR_GUI_PREREQUISITES_H___
#define ___CI_CASTOR_GUI_PREREQUISITES_H___

#include <Castor3D/Material/Pass/PassModule.hpp>
#include <Castor3D/Material/Texture/TextureModule.hpp>
#include <Castor3D/Overlay/OverlayModule.hpp>

#include <Castor3D/Plugin/GenericPlugin.hpp>

#include <CastorUtils/Design/FlagCombination.hpp>
#include <CastorUtils/Design/Signal.hpp>
#include <CastorUtils/Graphics/Colour.hpp>

#ifndef CU_PlatformWindows
#	define C3D_CGui_API
#else
#	ifdef CastorGui_EXPORTS
#		define C3D_CGui_API __declspec( dllexport )
#	else
#		define C3D_CGui_API __declspec( dllimport )
#	endif
#endif

namespace CastorGui
{
	const castor::String PLUGIN_NAME = cuT( "CastorGui" );
	static const uint32_t DEFAULT_HEIGHT = 25;
	/**
	*\brief	Enumeration of supported controls in a OptionsPanel
	*/
	enum class ControlType
		: uint8_t
	{
		eNone,
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
	};
	/**
	*\brief	Enumeration of supported combobox events
	*/
	enum class ComboBoxEvent
		: uint8_t
	{
		//! Item selected
		eSelected,
		//! The combo box events count
		eCount,
	};
	/**
	*\brief	Enumeration of supported listbox events
	*/
	enum class ListBoxEvent
		: uint8_t
	{
		//! Item selected
		eSelected,
		//! The combobox events count
		eCount,
	};
	/**
	*\brief	Enumeration of supported button events
	*/
	enum class ButtonEvent
		: uint8_t
	{
		//! Button clicked
		eClicked,
		//! The button events count
		eCount,
	};
	/**
	*\brief	Enumeration of supported slider events
	*/
	enum class SliderEvent
		: uint8_t
	{
		//! Slider thumb release
		eThumbRelease,
		//! Slider thumb track
		eThumbTrack,
		//! The slider events count
		eCount,
	};
	/**
	*\brief	Enumeration of supported edit events
	*/
	enum class EditEvent
		: uint8_t
	{
		//! The edit text has changed
		eUpdated,
		//! The return key has been hit (the edit must have EditStyle::eProcessEnter style)
		eReturn,
		//! The edit events count
		eCount,
	};
	/**
	*\brief	Combo control supported flags.
	*/
	enum class ComboBoxFlag
		: uint32_t
	{
		//! read only combo box
		eReadOnly = 0x00000001,
	};
	CU_ImplementFlags( ComboBoxFlag )
	/**
	*\brief	Edit control supported flags.
	*/
	enum class EditFlag
		: uint32_t
	{
		//! The edit control process 'return' hit as an event and not as a newline
		eProcessEnter = 0x00000100,
		//! The edit control is a multiline edit control
		eMultiline = 0x00000200,
		//! The edit control process 'tab' hit as the tab character and not as an event
		eProcessTab = 0x00000400,
		//! Words are not cut
		eWordWrap = 0x00000800,
	};
	CU_ImplementFlags( EditFlag )
	/**
	*\brief	Slider control supported flags.
	*/
	enum class SliderFlag
		: uint32_t
	{
		// The slider is displaye horizontally (default mode)
		eHorizontal = 0x00010000,
		// The slider is displaye vertically
		eVertical = 0x000200000,
	};
	CU_ImplementFlags( SliderFlag )
	/**
	*\brief	Static control supported flags.
	*/
	enum class StaticFlag
		: uint32_t
	{
		//! The static's text is left aligned
		eHAlignLeft = 0x01000000,
		//! The static's text is horizontally centered
		eHAlignCenter = 0x02000000,
		//! The static's text is right aligned
		eHAlignRight = 0x04000000,
		//! The static's text is aligned on top
		eVAlignTop = 0x10000000,
		//! The static's text is vertically centered
		eVAlignCenter = 0x20000000,
		//! The static's text is aligned on bottom
		eVAlignBottom = 0x40000000,
	};
	CU_ImplementFlags( StaticFlag )
	/**
	*\brief	Enumeration of supported GUI events in ControlInfos
	*/
	enum class GuiEvent
		: uint8_t
	{
		//! Event to ask the main window to show a message box
		eShowMessageBox,
		//! Event to ask the main window to resize to the given size
		eResizeParent,
	};
	/**
	*\brief	Mouse cursors enumeration
	*/
	enum class MouseCursor
	{
		//! The usual arrow cursor
		eArrow,
		//! The hand cursor
		eHand,
		//! The text cursor
		eText,
		//! The cursors count
		eCount,
	};

	template< class Derived > class EventHandler;
	class ControlsManager;

	class Control;
	class ButtonCtrl;
	class ComboBoxCtrl;
	class EditCtrl;
	class ListBoxCtrl;
	class SliderCtrl;
	class StaticCtrl;

	class Theme;
	class ControlStyle;
	class ButtonStyle;
	class ComboBoxStyle;
	class EditStyle;
	class ListBoxStyle;
	class SliderStyle;
	class StaticStyle;

	using OnButtonEventFunction = std::function< void() >;
	using OnButtonEvent = castor::SignalT< OnButtonEventFunction >;
	using OnButtonEventConnection = OnButtonEvent::connection;

	using OnComboEventFunction = std::function< void( int ) >;
	using OnComboEvent = castor::SignalT< OnComboEventFunction >;
	using OnComboEventConnection = OnComboEvent::connection;

	using OnEditEventFunction = std::function< void( castor::String const & ) >;
	using OnEditEvent = castor::SignalT< OnEditEventFunction >;
	using OnEditEventConnection = OnEditEvent::connection;

	using OnListEventFunction = std::function< void( int ) >;
	using OnListEvent = castor::SignalT< OnListEventFunction >;
	using OnListEventConnection = OnListEvent::connection;

	using OnSliderEventFunction = std::function< void( int ) >;
	using OnSliderEvent = castor::SignalT< OnSliderEventFunction >;
	using OnSliderEventConnection = OnSliderEvent::connection;

	CU_DeclareSmartPtr( ControlsManager );
	CU_DeclareSmartPtr( Control );
	CU_DeclareSmartPtr( ButtonCtrl );
	CU_DeclareSmartPtr( ComboBoxCtrl );
	CU_DeclareSmartPtr( EditCtrl );
	CU_DeclareSmartPtr( ListBoxCtrl );
	CU_DeclareSmartPtr( SliderCtrl );
	CU_DeclareSmartPtr( StaticCtrl );
	CU_DeclareSmartPtr( Theme );
	CU_DeclareSmartPtr( ControlStyle );
	CU_DeclareSmartPtr( ButtonStyle );
	CU_DeclareSmartPtr( ComboBoxStyle );
	CU_DeclareSmartPtr( EditStyle );
	CU_DeclareSmartPtr( ListBoxStyle );
	CU_DeclareSmartPtr( SliderStyle );
	CU_DeclareSmartPtr( StaticStyle );

	typedef std::vector< ControlSPtr > ControlArray;

	/** Sets the pass colour.
	 *\param[in]	pass	The pass.
	 *\param[in]	colour	The material colour.
	 */
	void setMaterialColour( castor3d::Pass & pass, castor::RgbColour const & colour );

	/** Retrieves the pass colour.
	 *\param[in]	pass	The pass.
	 *\return		The material colour.
	 */
	castor::RgbColour const & getMaterialColour( castor3d::Pass const & pass );
	/** Creates a colour material.
	 *\param[in]	engine	The engine.
	 *\param[in]	p_name		The material name.
	 *\param[in]	p_colour	The material colour.
	 *\return		The created material.
	*/
	castor3d::MaterialRPtr createMaterial( castor3d::Engine & engine, castor::String const & p_name, castor::RgbColour const & p_colour );

	/** Creates a texture material.
	 *\param[in]	engine	The engine.
	 *\param[in]	p_name		The material name.
	 *\param[in]	p_texture	The material texture.
	 *\return		The created material.
	*/
	castor3d::MaterialRPtr createMaterial( castor3d::Engine & engine, castor::String const & p_name, castor3d::TextureLayoutSPtr p_texture );
}

#endif
