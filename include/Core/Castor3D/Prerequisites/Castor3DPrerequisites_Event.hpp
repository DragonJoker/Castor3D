/*
See LICENSE file in root folder
*/
#ifndef ___C3D_CASTOR_GUI_PREREQUISITES_H___
#define ___C3D_CASTOR_GUI_PREREQUISITES_H___

#include <CastorUtils/Graphics/Colour.hpp>

namespace castor3d
{
	/*!
	\~english
	\brief		Frame Event Type enumeration
	\~french
	\brief		Enumération des types d'évènement de frame
	*/
	enum class EventType
		: uint8_t
	{
		//!\~english	This kind of event happens before any render, device context is active (so be fast !!).
		//!\~french		Ce type d'évènement est traité avant le rendu, le contexte de rendu est actif (donc soyez rapide !!)
		ePreRender,
		//!\~english	This kind of event happens after the render, before buffers' swap.
		//!\~french		Ce type d'évènement est traité après le rendu, avant l'échange des tampons.
		eQueueRender,
		//!\~english	This kind of event happens after the buffer' swap.
		//!\~french		Ce type d'évènement est traité après l'échange des tampons.
		ePostRender,
		CU_ScopedEnumBounds( ePreRender )
	};
	castor::String getName( EventType value );
	/*!
	*\author	Sylvain DOREMUS
	*\date		17/04/2016
	*\version	0.9.0
	*\~english
	*\brief		Supported events types enumeration.
	*\~french
	*\brief		Enumération des types évènements supportàs.
	*/
	enum class UserInputEventType
	{
		//!\~english	Mouse event type.
		//!\~french		Type d'evènement de souris.
		eMouse,
		//!\~english	Keyboard event type.
		//!\~french		Type d'évènement de clavier.
		eKeyboard,
		//!\~english	Handle event type.
		//!\~french		Type d'évènement de handler.
		eHandler,
		CU_ScopedEnumBounds( eMouse )
	};
	castor::String getName( UserInputEventType value );
	/*!
	*\author	Sylvain DOREMUS
	*\date		17/04/2016
	*\version	0.9.0
	*\~english
	*\brief		Enumeration of supported mouse events.
	*\~english
	*\brief		Enumération des évènement de souris supportàs.
	*/
	enum class MouseEventType
		: uint8_t
	{
		//!\~english	The mouse moves.
		//!\~french		La souris se déplace.
		eMove,
		//!\~english	The mouse enters an handler's zone.
		//!\~french		La souris entre dans la zone d'action d'un gestionnaire.
		eEnter,
		//!\~english	The mouse remains still in an handler's zone during hover time.
		//!\~french		La souris reste immobile pendant un temps déterminé, dans la zone d'action d'un gestionnaire.
		eHover,
		//!\~english	The mouse leaves an handler's zone.
		//!\~french		La souris quitte la zone d'action d'un gestionnaire.
		eLeave,
		//!\~english	One mouse button is pushed.
		//!\~french		Un bouton appuyé.
		ePushed,
		//!\~english	One mouse button is released.
		//!\~french		Un bouton relâché.
		eReleased,
		//!\~english	One mouse wheel is rolled.
		//!\~french		Une molette de la souris est tournée.
		eWheel,
		CU_ScopedEnumBounds( eMove )
	};
	castor::String getName( MouseEventType value );
	/*!
	*\author	Sylvain DOREMUS
	*\date		17/04/2016
	*\version	0.9.0
	*\~english
	*\brief		Enumeration of supported keyboard events.
	*\~english
	*\brief		Enumération des évènement clavier supportàs.
	*/
	enum class KeyboardEventType
		: uint8_t
	{
		//!\~english	A key is pushed.
		//!\~french		Une touche est appuyée.
		ePushed,
		//!\~english	A key is released.
		//!\~french		Une touche est relâchée.
		eReleased,
		//!\~english	A displayable char key is stroked.
		//!\~french		Une touche de caractère imprimable est tapée.
		eChar,
		CU_ScopedEnumBounds( ePushed )
	};
	castor::String getName( KeyboardEventType value );
	/*!
	*\author	Sylvain DOREMUS
	*\date		17/04/2016
	*\version	0.9.0
	*\~english
	*\brief		Enumeration of supported handler events.
	*\~english
	*\brief		Enumération des évènements de gestionnaire supportées.
	*/
	enum class HandlerEventType
		: uint8_t
	{
		//!\~english	Handler activated.
		//!\~french		Gestionnaire activé.
		eActivate,
		//!\~english	Handler deactivated.
		//!\~french		Gestionnaire désactivé.
		eDeactivate,
		CU_ScopedEnumBounds( eActivate )
	};
	castor::String getName( HandlerEventType value );
	/*!
	*\author	Sylvain DOREMUS
	*\date		17/04/2016
	*\version	0.9.0
	*\~english
	*\brief		Mouse buttons enumeration.
	*\~french
	*\brief		Enumération des boutons de la souris.
	*/
	enum class MouseButton
	{
		//!\~english	Left mouse button.
		//!\~french		Le bouton gauche.
		eLeft,
		//!\~english	Middle mouse button.
		//!\~french		Le bouton du milieu.
		eMiddle,
		//!\~english	Right mouse button.
		//!\~french		Le bouton droit.
		eRight,
		CU_ScopedEnumBounds( eLeft )
	};
	castor::String getName( MouseButton value );
	/*!
	*\author	Sylvain DOREMUS
	*\date		17/04/2016
	*\version	0.9.0
	*\~english
	*\brief		Keyboard keys enumeration.
	*\~french
	*\brief		Enumération des touches du clavier.
	*/
	enum class KeyboardKey
	{
		eNone,
		eBackspace = 0x08,
		eTab = 0x0A,
		eReturn = 0x0D,
		eEscape = 0x1B,
		eSpace = 0x20,

		// values from 0x21 to 0x7E are reserved for the standard ASCII characters

		eDelete = 0x7F,

		// values from 0x80 to 0xFF are reserved for ASCII extended characters

		eStart = 0x100,
		eLButton,
		eRButton,
		eCancel,
		eMButton,
		eClear,
		eShift,
		eAlt,
		eControl,
		eMenu,
		ePause,
		eCapital,
		eEnd,
		eHome,
		eLeft,
		eUp,
		eRight,
		edown,
		eSelect,
		ePrint,
		eExecute,
		eSnapshot,
		eInsert,
		eHelp,
		eNumpad0,
		eNumpad1,
		eNumpad2,
		eNumpad3,
		eNumpad4,
		eNumpad5,
		eNumpad6,
		eNumpad7,
		eNumpad8,
		eNumpad9,
		eMultiply,
		eAdd,
		eSeparator,
		eSubtract,
		eDecimal,
		eDivide,
		eF1,
		eF2,
		eF3,
		eF4,
		eF5,
		eF6,
		eF7,
		eF8,
		eF9,
		eF10,
		eF11,
		eF12,
		eF13,
		eF14,
		eF15,
		eF16,
		eF17,
		eF18,
		eF19,
		eF20,
		eF21,
		eF22,
		eF23,
		eF24,
		eNumlock,
		eScroll,
		ePageUp,
		ePageDown,
		eNumpadSpace,
		eNumpadTab,
		eNumpadEnter,
		eNumpadF1,
		eNumpadF2,
		eNumpadF3,
		eNumpadF4,
		eNumpadHome,
		eNumpadLeft,
		eNumpadUp,
		eNumpadRight,
		eNumpadDown,
		eNumpadPageUp,
		eNumpadPageDown,
		eNumpadEnd,
		eNumpadBegin,
		eNumpadInsert,
		eNumpadDelete,
		eNumpadEqual,
		eNumpadMultiply,
		eNumpadAdd,
		eNumpadSeparator,
		eNumpadSubtract,
		eNumpadDecimal,
		eNumpadDivide,
	};

	class EventHandler;
	class UserInputEvent;
	class KeyboardEvent;
	class MouseEvent;
	class HandlerEvent;
	class UserInputListener;

	CU_DeclareSmartPtr( EventHandler );
	CU_DeclareSmartPtr( UserInputEvent );
	CU_DeclareSmartPtr( KeyboardEvent );
	CU_DeclareSmartPtr( MouseEvent );
	CU_DeclareSmartPtr( HandlerEvent );
	CU_DeclareSmartPtr( UserInputListener );

	template< class Derived > class NonClientEventHandler;
}

#endif
