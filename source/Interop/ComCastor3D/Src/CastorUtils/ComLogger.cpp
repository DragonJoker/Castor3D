#include "ComLogger.hpp"
#include "ComUtils.hpp"

namespace CastorCom
{
	CLogger::CLogger()
	{
	}

	CLogger::~CLogger()
	{
	}

	STDMETHODIMP CLogger::initialise( /* [in] */ eLOG_TYPE level )
	{
		castor::Logger::initialise( castor::LogType( level ) );
		return S_OK;
	}

	STDMETHODIMP CLogger::setFileName( /* [in] */ BSTR name, eLOG_TYPE target )
	{
		castor::Logger::setFileName( FromBstr( name ), castor::LogType( target ) );
		return S_OK;
	}

	STDMETHODIMP CLogger::cleanup()
	{
		castor::Logger::cleanup();
		return S_OK;
	}

	STDMETHODIMP CLogger::logDebug( /* [in] */ BSTR msg )
	{
		castor::Logger::logDebug( FromBstr( msg ) );
		return S_OK;
	}

	STDMETHODIMP CLogger::logInfo( /* [in] */ BSTR msg )
	{
		castor::Logger::logInfo( FromBstr( msg ) );
		return S_OK;
	}

	STDMETHODIMP CLogger::logWarning( /* [in] */ BSTR msg )
	{
		castor::Logger::logWarning( FromBstr( msg ) );
		return S_OK;
	}

	STDMETHODIMP CLogger::logError( /* [in] */ BSTR msg )
	{
		castor::Logger::logError( FromBstr( msg ) );
		return S_OK;
	}

}
