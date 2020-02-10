/*
See LICENSE file in root folder
*/
#ifndef ___C3D_FrameEventModule_H___
#define ___C3D_FrameEventModule_H___

#include "Castor3D/Castor3DModule.hpp"

namespace castor3d
{
	/**@name Event */
	//@{
	/**@name Frame */
	//@{

	/**
	*\~english
	*\brief
	*	Frame Event Type enumeration
	*\~french
	*\brief
	*	Enumération des types d'évènement de frame
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
	/**
	*\~english
	*\brief
	*	Cleaner event
	*\remarks
	*	Cleans up the member given when constructed.
	*\~french
	*\brief
	*	Evènement de nettoyage
	*\remarks
	*	Nettoie le membre donné lors de la construction.
	*/
	template< class T >
	class CleanupEvent;
	/**
	*\~english
	*\brief
	*	The interface which represents a frame event.
	*\remarks
	*	Basically a frame event has a EventType to know when it must be applied.
	*	<br />It can be applied, so the function must be implemented by children classes
	*\~french
	*\brief
	*	Interface représentant un évènement de frame
	*\remarks
	*	Un évènement a un EventType pour savoir quand il doit être traité.
	*	<br />La fonction de traitement doit être implémentée par les classes filles.
	*/
	class FrameEvent;
	/**
	*\~english
	*\brief
	*	User event synchronisation class.
	*\remarks
	*	The handler of the frame events. It can add frame events and applies them at the wanted times.
	*\~french
	*\brief
	*	Classe de synchronisation des évènements.
	*\remarks
	*	Le gestionnaire des évènements de frame, on peut y ajouter des évènements à traiter, qui le seront au moment voulu (en fonction de leur EventType).
	*/
	class FrameListener;
	/**
	*\~english
	*\brief
	*	Functor event
	*\remarks
	*	Executes a function when processed
	*\~french
	*\brief
	*	Evènement foncteur
	*\remarks
	*	Excécute une fonction lorsqu'il est traité
	*/
	template< class Functor >
	class FunctorEvent;
	/**
	*\~english
	*\brief
	*	Initialiser event
	*\remarks
	*	Initialises the member given when constructed.
	*\~french
	*\brief
	*	Evènement d'initialisation
	*\remarks
	*	Initialise le membre donné lors de la construction.
	*/
	template< class T >
	class InitialiseEvent;

	CU_DeclareSmartPtr( FrameEvent );
	CU_DeclareSmartPtr( FrameListener );

	//! FrameEvent pointer array
	CU_DeclareVector( FrameEventUPtr, FrameEventPtr );
	//! FrameListener pointer map, sorted by name
	CU_DeclareMap( castor::String, FrameListenerSPtr, FrameListenerPtrStr );

	//@}
	//@}
}

#endif
