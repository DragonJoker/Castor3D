/*
See LICENSE file in root folder
*/
#ifndef ___CASTOR_OWNED_BY_H___
#define ___CASTOR_OWNED_BY_H___

#include "CastorUtils/Design/DesignModule.hpp"

namespace castor
{
	template< class Owner >
	class OwnedBy
	{
	protected:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	owner	The owner object.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	owner	L'objet propriétaire.
		 */
		explicit OwnedBy( Owner & owner )
			: m_owner( &owner )
		{
		}

	public:
		/**
		 *\~english
		 *\return		The owner object.
		 *\~french
		 *\brief		L'objet propriétaire.
		 */
		Owner * getOwner()const
		{
			return m_owner;
		}

	private:
		Owner * m_owner;
	};

	/**
	 *\~english
	 *\brief		Helper macro to declare an exported OwnedBy specialisation.
	 *\remarks		Must be used in global namespace.
	 *\~french
	 *\brief		Macro pour déclarer une spécialisation exportée de OwnedBy.
	 *\remarks		doit être utilisée dans le namespace global.
	 */
#	define CU_DeclareExportedOwnedBy( Export, Owner, Name )\
	namespace castor\
	{\
		template<>\
		class Export OwnedBy< Owner >\
		{\
		protected:\
			explicit OwnedBy( Owner & owner );\
		public:\
			Owner * get##Name()const;\
			Owner * getOwner()const;\
		private:\
			Owner * m_owner;\
		};\
	}

	/**
	 *\~english
	 *\brief		Helper macro to implement an OwnedBy specialisation.
	 *\remarks		Must be used in global namespace.
	 *\~french
	 *\brief		Macro pour implémenter une spécialisation de OwnedBy.
	 *\remarks		doit être utilisée dans le namespace global.
	 */
#	define CU_ImplementExportedOwnedBy( Owner, Name )\
	namespace castor\
	{\
		OwnedBy< Owner >::OwnedBy( Owner & owner )\
			: m_owner( &owner )\
		{\
		}\
		Owner * OwnedBy< Owner >::get##Name()const\
		{\
			return m_owner;\
		}\
		Owner * OwnedBy< Owner >::getOwner()const\
		{\
			return m_owner;\
		}\
	}
}

#endif
