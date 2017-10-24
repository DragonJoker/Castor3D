/*
See LICENSE file in root folder
*/
#ifndef ___C3D_FRAME_LISTENER_H___
#define ___C3D_FRAME_LISTENER_H___

#include "Castor3DPrerequisites.hpp"

#include <Design/Named.hpp>

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.1
	\date		03/03/2010
	\~english
	\brief		User event synchronisation class.
	\remark		The handler of the frame events. It can add frame events and applies them at the wanted times.
	\~french
	\brief		Classe de synchronisation des évènements.
	\remark		Le gestionnaire des évènements de frame, on peut y ajouter des évènements à traiter, qui le seront au moment voulu (en fonction de leur EventType).
	*/
	class FrameListener
		: public castor::Named
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_name	The listener's name.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_name	Le nom du listener.
		 */
		C3D_API explicit FrameListener( castor::String const & p_name );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~FrameListener();
		/**
		 *\~english
		 *\brief		Empties the event lists.
		 *\~french
		 *\brief		Vide les listes d'évènements.
		 */
		C3D_API void flush();
		/**
		 *\~english
		 *\brief		Puts an event in the corresponding array.
		 *\param[in]	p_event	The event to put.
		 *\~french
		 *\brief		Ajoute un évènement à la liste d'évènements correspondant à sont type.
		 *\param[in]	p_event	L'évènement à ajouter.
		 */
		C3D_API void postEvent( FrameEventUPtr && p_event );
		/**
		 *\~english
		 *\brief		Applies all events of a given type, then discards them.
		 *\param[in]	p_type	The type of events to fire.
		 *\return		\p true si tous les évènements se sont exécutés sans erreur.
		 *\~french
		 *\brief		Traite tous les évènements d'un type donné.
		 *\param[in]	p_type	Le type des évènements à traiter.
		 *\return		\p true if all events were processed successfully.
		 */
		C3D_API bool fireEvents( EventType p_type );
		/**
		 *\~english
		 *\brief		Discards all events of a given type.
		 *\param[in]	p_type	The type of events to fire.
		 *\~french
		 *\brief		Supprime tous les évènements d'un type donné.
		 *\param[in]	p_type	Le type des évènements à traiter.
		 */
		C3D_API void flushEvents( EventType p_type );

	protected:
		/**
		 *\~english
		 *\brief		Empties the event lists.
		 *\~french
		 *\brief		Vide les listes d'évènements.
		 */
		C3D_API virtual void doFlush() {}

	protected:
		//!\~english	The events arrays.
		//!\~french		Les tableaux d'évènements.
		std::array< FrameEventPtrArray,	size_t( EventType::eCount ) > m_events;
		//!\~english	Mutex to make this class thread safe.
		//!\~french		Mutex pour rendre cette classe thread safe.
		std::recursive_mutex m_mutex;
	};
}

#endif
