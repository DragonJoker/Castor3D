/* See LICENSE file in root folder */
#ifndef __COMC3D_COM_LOGGER_H__
#define __COMC3D_COM_LOGGER_H__

#include "ComCastor3D/ComCastor3DPrerequisites.hpp"

#include "ComCastor3D/ComAtlObject.hpp"

#include <CastorUtils/Log/Logger.hpp>

namespace CastorCom
{
	class CLogger;
	template<>
	struct ComTypeTraitsT< castor::Logger >
	{
		static constexpr bool hasIType = true;
		static constexpr bool hasType = true;
		static constexpr bool hasInternalType = false;

		using IType = ILogger;
		using CType = CLogger;

		static inline const CLSID clsid = CLSID_Logger;
		static inline const CLSID iid = IID_ILogger;
		static inline const UINT rid = IDR_Logger;
	};
	/*!
	\~english
	\brief		This class defines a CLogger object accessible from COM.
	\~french
	\brief		Cette classe définit un CLogger accessible depuis COM.
	*/
	class CLogger
		: public CComAtlObjectBaseT< Logger, castor::Logger >
	{
	public:
		STDMETHOD( Initialise )( /* [in] */ eLOG_TYPE level );
		STDMETHOD( SetFileName )( /* [in] */ BSTR name, eLOG_TYPE target );
		STDMETHOD( Cleanup )();
		STDMETHOD( LogTrace )( /* [in] */ BSTR msg );
		STDMETHOD( LogDebug )( /* [in] */ BSTR msg );
		STDMETHOD( LogInfo )( /* [in] */ BSTR msg );
		STDMETHOD( LogWarning )( /* [in] */ BSTR msg );
		STDMETHOD( LogError )( /* [in] */ BSTR msg );
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object
	//!\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( Logger ), CLogger );
}

#endif
