/* See LICENSE file in root folder */
#ifndef __COMC3D_COM_LOGGER_H__
#define __COMC3D_COM_LOGGER_H__

#include "ComCastor3DPrerequisites.hpp"

#include "ComAtlObject.hpp"

#include <Log/Logger.hpp>

namespace CastorCom
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0
	\date		10/09/2014
	\~english
	\brief		This class defines a CLogger object accessible from COM.
	\~french
	\brief		Cette classe définit un CLogger accessible depuis COM.
	*/
	class ATL_NO_VTABLE CLogger
		:	COM_ATL_OBJECT( Logger )
	{
	public:
		/**
		 *\~english
		 *\brief		Default constructor.
		 *\~french
		 *\brief		Constructeur par défaut.
		 */
		CLogger();
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		virtual ~CLogger();

		STDMETHOD( Initialise )( /* [in] */ eLOG_TYPE level );
		STDMETHOD( SetFileName )( /* [in] */ BSTR name, eLOG_TYPE target );
		STDMETHOD( Cleanup )();
		STDMETHOD( LogTrace )( /* [in] */ BSTR msg );
		STDMETHOD( LogDebug )( /* [in] */ BSTR msg );
		STDMETHOD( LogInfo )( /* [in] */ BSTR msg );
		STDMETHOD( LogWarning )( /* [in] */ BSTR msg );
		STDMETHOD( LogError )( /* [in] */ BSTR msg );
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object	\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( Logger ), CLogger )
}

#endif
