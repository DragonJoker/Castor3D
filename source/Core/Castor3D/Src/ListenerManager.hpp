/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___C3D_LISTENER_MANAGER_H___
#define ___C3D_LISTENER_MANAGER_H___

#include "Manager/ResourceManager.hpp"

#include "Event/Frame/FrameListener.hpp"

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		04/02/2016
	\version	0.8.0
	\~english
	\brief		Helper structure to get an object type name.
	\~french
	\brief		Structure permettant de récupérer le nom du type d'un objet.
	*/
	template<> struct ManagedObjectNamer< FrameListener >
	{
		C3D_API static const Castor::String Name;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		13/10/2015
	\version	0.8.0
	\~english
	\brief		Helper structure to enable cleanup if a type supports it.
	\remarks	Specialisation for types that support cleanup.
	\~french
	\brief		Structure permettant de nettoyer les éléments qui le supportent.
	\remarks	Spécialisation pour les types qui supportent le cleanup.
	*/
	template<>
	struct ElementCleaner< FrameListener, FrameListener >
	{
		static void Cleanup( Engine & p_engine, FrameListener & p_element )
		{
			p_element.Flush();
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		15/10/2015
	\version	0.8.0
	\~english
	\brief		Frame listener manager.
	\~french
	\brief		Gestionnaire de frame listener.
	*/
	class ListenerManager
		: public ResourceManager< Castor::String, FrameListener >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\~french
		 *\brief		Constructeur.
		 */
		C3D_API ListenerManager( Engine & p_engine );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~ListenerManager();
		/**
		 *\~english
		 *\brief		Posts a frame event to the default frame listener.
		 *\param[in]	p_event		The event to add.
		 *\~french
		 *\brief		Ajoute un évènement de frame au frame listener par défaut.
		 *\param[in]	p_event		L'évènement.
		 */
		C3D_API void PostEvent( FrameEventSPtr p_event );
		/**
		 *\~english
		 *\brief		Fires all events of given type for all listeners.
		 *\param[in]	p_type	The event type.
		 *\~french
		 *\brief		Ajoute un évènement de frame au frame listener par défaut
		 *\param[in]	p_type	Le type d'évènement.
		 */
		C3D_API void FireEvents( eEVENT_TYPE p_type );

	private:
		//!\~english The default frame listener.	\~french Le frame listener par défaut.
		FrameListenerWPtr m_defaultListener;
	};
}

#undef DECLARE_MANAGED_MEMBER

#endif
