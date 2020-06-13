/*
See LICENSE file in root folder
*/
#ifndef ___CU_LogType_H___
#define ___CU_LogType_H___

namespace castor
{
	/**
	\~english
	\brief		Defines the various log types
	\~french
	\brief		Définit les différents types de log
	*/
	enum class LogType
		: uint8_t
	{
		//!\~english	Trace type log.
		//!\~french		Log de type trace.
		eTrace,
		//!\~english	Debug type log.
		//!\~french		Log de type debug.
		eDebug,
		//!\~english	Information type log.
		//!\~french		Log de type information.
		eInfo,
		//!\~english	Warning type log.
		//!\~french		Log de type avertissement.
		eWarning,
		//!\~english	Error type log.
		//!\~french		Log de type erreur.
		eError,
		//!\~english	Log types count.
		//!\~french		Compte des logs.
		eCount,
	};
}

#endif
