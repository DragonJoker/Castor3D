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
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	owner	The owner object.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	owner	L'objet propriétaire.
		 */
		inline explicit OwnedBy( Owner & owner )
			: m_owner( owner )
		{
		}
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		inline ~OwnedBy()
		{
		}
		/**
		 *\~english
		 *\return		The owner object.
		 *\~french
		 *\brief		L'objet propriétaire.
		 */
		inline Owner * getOwner()const
		{
			return &m_owner;
		}

	private:
		Owner & m_owner;
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
		private:\
			OwnedBy & operator=( OwnedBy< Owner > const & rhs ) = delete;\
			OwnedBy & operator=( OwnedBy< Owner > && rhs ) = delete;\
		public:\
			explicit OwnedBy( Owner & owner );\
			OwnedBy( OwnedBy< Owner > const & rhs ) = default;\
			OwnedBy( OwnedBy< Owner > && rhs ) = default;\
			~OwnedBy() = default;\
			Owner * get##Name()const;\
		private:\
			Owner & m_owner;\
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
			: m_owner( owner )\
		{\
		}\
		Owner * OwnedBy< Owner >::get##Name()const\
		{\
			return &m_owner;\
		}\
	}
}

#endif
