/*
See LICENSE file in root folder
*/
#ifndef ___C3D_UserInputEventModule_H___
#define ___C3D_UserInputEventModule_H___

#include "Castor3D/Castor3DModule.hpp"

#include <CastorUtils/Graphics/Position.hpp>

#include <array>

namespace castor3d
{
	/**@name Event */
	//@{
	/**@name UserInput */
	//@{

	/**
	*\~english
	*\brief
	*	Supported events types enumeration.
	*\~french
	*\brief
	*	Enumération des types évènements supportés.
	*/
	enum class UserInputEventType
	{
		//!\~english	Mouse event type.
		//!\~french		Type d'evènement de souris.
		eMouse = 0,
		//!\~english	Keyboard event type.
		//!\~french		Type d'évènement de clavier.
		eKeyboard = 1,
		//!\~english	Handle event type.
		//!\~french		Type d'évènement de handler.
		eHandler = 2,
		CU_ScopedEnumBounds( eMouse, eHandler )
	};
	C3D_API castor::String getName( UserInputEventType value );
	/**
	*\~english
	*\brief
	*	Enumeration of supported mouse events.
	*\~english
	*\brief
	*	Enumération des évènement de souris supportés.
	*/
	enum class MouseEventType
		: uint8_t
	{
		//!\~english	The mouse moves.
		//!\~french		La souris se déplace.
		eMove = 0,
		//!\~english	The mouse enters an handler's zone.
		//!\~french		La souris entre dans la zone d'action d'un gestionnaire.
		eEnter = 1,
		//!\~english	The mouse remains still in an handler's zone during hover time.
		//!\~french		La souris reste immobile pendant un temps déterminé, dans la zone d'action d'un gestionnaire.
		eHover = 2,
		//!\~english	The mouse leaves an handler's zone.
		//!\~french		La souris quitte la zone d'action d'un gestionnaire.
		eLeave = 3,
		//!\~english	One mouse button is pushed.
		//!\~french		Un bouton appuyé.
		ePushed = 4,
		//!\~english	One mouse button is released.
		//!\~french		Un bouton relâché.
		eReleased = 5,
		//!\~english	One mouse wheel is rolled.
		//!\~french		Une molette de la souris est tournée.
		eWheel = 6,
		CU_ScopedEnumBounds( eMove, eWheel )
	};
	C3D_API castor::String getName( MouseEventType value );
	/**
	*\~english
	*\brief
	*	Enumeration of supported keyboard events.
	*\~english
	*\brief
	*	Enumération des évènement clavier supportés.
	*/
	enum class KeyboardEventType
		: uint8_t
	{
		//!\~english	A key is pushed.
		//!\~french		Une touche est appuyée.
		ePushed = 0,
		//!\~english	A key is released.
		//!\~french		Une touche est relâchée.
		eReleased = 1,
		//!\~english	A displayable char key is stroked.
		//!\~french		Une touche de caractère imprimable est tapée.
		eChar = 2,
		CU_ScopedEnumBounds( ePushed, eChar )
	};
	C3D_API castor::String getName( KeyboardEventType value );
	/**
	*\~english
	*\brief
	*	Enumeration of supported handler events.
	*\~english
	*\brief
	*	Enumération des évènements de gestionnaire supportées.
	*/
	enum class HandlerEventType
		: uint8_t
	{
		//!\~english	Handler activated.
		//!\~french		Gestionnaire activé.
		eActivate = 0,
		//!\~english	Handler deactivated.
		//!\~french		Gestionnaire désactivé.
		eDeactivate = 1,
		CU_ScopedEnumBounds( eActivate, eDeactivate )
	};
	C3D_API castor::String getName( HandlerEventType value );
	/**
	*\~english
	*\brief
	*	Mouse buttons enumeration.
	*\~french
	*\brief
	*	Enumération des boutons de la souris.
	*/
	enum class MouseButton
	{
		//!\~english	Left mouse button.
		//!\~french		Le bouton gauche.
		eLeft = 0,
		//!\~english	Middle mouse button.
		//!\~french		Le bouton du milieu.
		eMiddle = 1,
		//!\~english	Right mouse button.
		//!\~french		Le bouton droit.
		eRight = 2,
		CU_ScopedEnumBounds( eLeft, eRight )
	};
	C3D_API castor::String getName( MouseButton value );
	/**
	*\~english
	*\brief
	*	Mouse cursors enumeration.
	*\~french
	*\brief
	*	Enumération des curseurs de souris.
	*/
	enum class MouseCursor
	{
		//!\~english	Usual arrow cursor.
		//!\~french		Pointeur en flèche habituel.
		eArrow = 0,
		//!\~english	Hand cursor.
		//!\~french		Curseur en forme de main.
		eHand = 1,
		//!\~english	Text cursor.
		//!\~french		Curseur pour texte.
		eText = 2,
		//!\~english	Arrows pointing left and right cursor.
		//!\~french		Flèches gauche-droite.
		eSizeWE = 3,
		//!\~english	Arrows pointing up and down cursor.
		//!\~french		Flèches haut-bas.
		eSizeNS = 4,
		//!\~english	Diagonal arrows pointing top-left and bottom-right cursor.
		//!\~french		Flèches en diagonale haut-gauche vers bas-droite.
		eSizeNWSE = 5,
		//!\~english	Diagonal arrows pointing top-right and bottom-left cursor.
		//!\~french		Flèches en diagonale haut-droite vers bas-gauche.
		eSizeNESW = 6,
		CU_ScopedEnumBounds( eArrow, eSizeNESW )
	};
	/**
	*\~english
	*\brief
	*	Keyboard keys enumeration.
	*\~french
	*\brief
	*	Enumération des touches du clavier.
	*/
	enum class KeyboardKey
	{
		eNone = 0x00,
		eBackspace = 0x08,
		eTab = 0x0A,
		eReturn = 0x0D,
		eEscape = 0x1B,
		eSpace = 0x20,
		eAsciiBegin = eSpace,

		// values from 0x21 to 0x7E are reserved for the standard ASCII characters

		eDelete = 0x7F,

		// values from 0x80 to 0xFF are reserved for ASCII extended characters

		eAsciiEnd = 0xFF,
		eStart = 0x100,
		eLButton = 0x101,
		eRButton = 0x102,
		eCancel = 0x103,
		eMButton = 0x104,
		eClear = 0x105,
		eShift = 0x106,
		eAlt = 0x107,
		eControl = 0x108,
		eMenu = 0x109,
		ePause = 0x10A,
		eCapital = 0x10B,
		eEnd = 0x10C,
		eHome = 0x10D,
		eLeft = 0x10E,
		eUp = 0x10F,
		eRight = 0x110,
		eDown = 0x111,
		eSelect = 0x112,
		ePrint = 0x113,
		eExecute = 0x114,
		eSnapshot = 0x115,
		eInsert = 0x116,
		eHelp = 0x117,
		eNumpad0 = 0x118,
		eNumpad1 = 0x119,
		eNumpad2 = 0x11A,
		eNumpad3 = 0x11B,
		eNumpad4 = 0x11C,
		eNumpad5 = 0x11D,
		eNumpad6 = 0x11E,
		eNumpad7 = 0x11F,
		eNumpad8 = 0x120,
		eNumpad9 = 0x121,
		eMultiply = 0x122,
		eAdd = 0x123,
		eSeparator = 0x124,
		eSubtract = 0x125,
		eDecimal = 0x126,
		eDivide = 0x127,
		eF1 = 0x128,
		eF2 = 0x129,
		eF3 = 0x12A,
		eF4 = 0x12B,
		eF5 = 0x12C,
		eF6 = 0x12D,
		eF7 = 0x12E,
		eF8 = 0x12F,
		eF9 = 0x130,
		eF10 = 0x131,
		eF11 = 0x132,
		eF12 = 0x133,
		eF13 = 0x134,
		eF14 = 0x135,
		eF15 = 0x136,
		eF16 = 0x137,
		eF17 = 0x138,
		eF18 = 0x139,
		eF19 = 0x13A,
		eF20 = 0x13B,
		eF21 = 0x13C,
		eF22 = 0x13D,
		eF23 = 0x13E,
		eF24 = 0x13F,
		eNumlock = 0x140,
		eScroll = 0x141,
		ePageUp = 0x142,
		ePageDown = 0x143,
		eNumpadSpace = 0x144,
		eNumpadTab = 0x145,
		eNumpadEnter = 0x146,
		eNumpadF1 = 0x147,
		eNumpadF2 = 0x148,
		eNumpadF3 = 0x149,
		eNumpadF4 = 0x14A,
		eNumpadHome = 0x14B,
		eNumpadLeft = 0x14C,
		eNumpadUp = 0x14D,
		eNumpadRight = 0x14E,
		eNumpadDown = 0x14F,
		eNumpadPageUp = 0x150,
		eNumpadPageDown = 0x151,
		eNumpadEnd = 0x152,
		eNumpadBegin = 0x153,
		eNumpadInsert = 0x154,
		eNumpadDelete = 0x155,
		eNumpadEqual = 0x156,
		eNumpadMultiply = 0x157,
		eNumpadAdd = 0x158,
		eNumpadSeparator = 0x159,
		eNumpadSubtract = 0x15A,
		eNumpadDecimal = 0x15B,
		eNumpadDivide = 0x15C,
	};
	/**
	*\~english
	*\brief
	*	Keyboard state.
	*\~french
	*\brief
	*	Etat du clavier.
	*/
	struct KeyboardState
	{
		//!\~english	The control key state.
		//!\~french		L'état de la touche Ctrl.
		bool ctrl;
		//!\~english	The alt key state.
		//!\~french		L'état de la touche Alt.
		bool alt;
		//!\~english	The shift key state.
		//!\~french		L'état de la touche Shift.
		bool shift;
	};
	/**
	*\~english
	*\brief
	*	Mouse state.
	*\~french
	*\brief
	*	Etat de la souris.
	*/
	struct MouseState
	{
		//!\~english	The current position.
		//!\~french		La position actuelle.
		castor::Position position;
		//!\~english	The wheel position.
		//!\~french		La position de la molette.
		castor::Position wheel;
		//!\~english	The buttons state (true = down).
		//!\~french		L'état des boutons (true = enfoncé).
		std::array< bool, size_t( MouseButton::eCount ) > buttons;
		//!\~english	The button which had the last change.
		//!\~french		Le bouton ayant reçu le dernier changement.
		MouseButton changed;
	};
	/**
	*\~english
	*\brief
	*	Description of an event handler, class that can receive event.
	*\~french
	*\brief
	*	Description d'un gestionnaire d'évènements, classe pouvant recevoir des évènements.
	*/
	class EventHandler;
	/**
	*\~english
	*\brief
	*	Description of a handler event.
	*\~french
	*\brief
	*	Description d'un évènement de gestionnaire.
	*/
	class HandlerEvent;
	/**
	*\~english
	*\brief
	*	Description of a keyboard event.
	*\~french
	*\brief
	*	Description d'un évènement clavier.
	*/
	class KeyboardEvent;
	/**
	*\~english
	*\brief
	*	Description of a mouse event.
	*\~french
	*\brief
	*	Description d'un évènement de souris.
	*/
	class MouseEvent;
	/**
	*\~english
	*\brief
	*	Description of a user input event.
	*\remarks
	*	You may use this one, but prefer using MouseEvent or KeyboardEvent.
	*\~french
	*\brief
	*	Description d'un évènement de tpye entrée utilisateur.
	*\remarks
	*	Vous pouvez utiliser cette classe, mais il est conseillé d'utiliser les  classes MouseEvent ou KeyboardEvent.
	*/
	class UserInputEvent;
	/**
	*\~english
	*\brief
	*	Class used to to manage the user input events.
	*\~french
	*\brief
	*	Classe utilisée pour gérer les évènements utilisateur.
	*/
	class UserInputListener;

	CU_DeclareSmartPtr( castor3d, EventHandler, C3D_API );
	CU_DeclareSmartPtr( castor3d, UserInputListener, C3D_API );

	using UserInputEventSPtr = std::shared_ptr< UserInputEvent >;
	using KeyboardEventSPtr = std::shared_ptr< KeyboardEvent >;
	using MouseEventSPtr = std::shared_ptr< MouseEvent >;
	using HandlerEventSPtr = std::shared_ptr< HandlerEvent >;

	template< class Derived >
	class NonClientEventHandler;

	//@}
	//@}
}

#endif
