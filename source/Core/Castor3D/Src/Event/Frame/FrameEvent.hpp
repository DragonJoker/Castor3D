/*
See LICENSE file in root folder
*/
#ifndef ___C3D_FRAME_EVENT_H___
#define ___C3D_FRAME_EVENT_H___

#include "Castor3DPrerequisites.hpp"

#include <Miscellaneous/Debug.hpp>

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\date		03/03/2010
	\version	0.1
	\~english
	\brief		The interface which represents a frame event.
	\remark		Basically a frame event has a EventType to know when it must be applied.
				<br />It can be applied, so the function must be implemented by children classes
	\~french
	\brief		Interface représentant un évènement de frame
	\remark		Un évènement a un EventType pour savoir quand il doit être traité.
				<br />La fonction de traitement doit être implémentée par les classes filles.
	*/
	class FrameEvent
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_type	The event type.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_type	Le type d'évènement.
		 */
		C3D_API explicit FrameEvent( EventType p_type );
		/**
		 *\~english
		 *\brief		Copy constructor.
		 *\param[in]	p_object	The object to copy.
		 *\~french
		 *\brief		Constructeur par copie.
		 *\param[in]	p_object	L'objet à copier.
		 */
		C3D_API FrameEvent( FrameEvent const & p_object ) = default;
		/**
		 *\~english
		 *\brief		Move constructor.
		 *\param[in]	p_object	The object to move.
		 *\~french
		 *\brief		Constructeur par déplacement.
		 *\param[in]	p_object	L'objet à déplacer.
		 */
		C3D_API FrameEvent( FrameEvent && p_object ) = default;
		/**
		 *\~english
		 *\brief		Copy assignment operator.
		 *\param[in]	p_object	The object to copy.
		 *\return		A reference to this object.
		 *\~french
		 *\brief		Opérateur d'affectation par copie.
		 *\param[in]	p_object	L'objet à copier.
		 *\return		Une référence sur cet objet.
		 */
		C3D_API FrameEvent & operator=( FrameEvent const & p_object ) = default;
		/**
		 *\~english
		 *\brief		Move assignment operator.
		 *\param[in]	p_object	The object to move.
		 *\return		A reference to this object.
		 *\~french
		 *\brief		Opérateur d'affectation par déplacement.
		 *\param[in]	p_object	L'objet à déplacer.
		 *\return		Une référence sur cet objet.
		 */
		C3D_API FrameEvent & operator=( FrameEvent && p_object ) = default;
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API virtual ~FrameEvent();
		/**
		 *\~english
		 *\brief		Applies the event.
		 *\remarks		Must be implemented by children classes.
		 *\return		\p true if the event was applied successfully.
		 *\~french
		 *\brief		Traite l'évènement.
		 *\remarks		doit être implémentée dans les classes filles.
		 *\return		\p true si l'évènement a été traité avec succès.
		 */
		C3D_API virtual bool apply() = 0;
		/**
		 *\~english
		 *\brief		Retrieves the event type.
		 *\return		The event type.
		 *\~french
		 *\brief		Récupère le type de l'évènement.
		 *\return		Le type de l'évènement.
		 */
		inline EventType getType()
		{
			return m_type;
		}

	protected:
		//!\~english	The event type.
		//!\~french		Le type d'évènement.
		EventType m_type;

#if !defined( NDEBUG )

		//!\~english	The event creation stack trace.
		//!\~french		La pile d'appels lors de la création de l'évènement.
		castor::String m_stackTrace;

#endif
	};
}

#endif
