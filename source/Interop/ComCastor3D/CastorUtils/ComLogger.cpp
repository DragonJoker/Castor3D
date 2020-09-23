#include "ComCastor3D/CastorUtils/ComLogger.hpp"
#include "ComCastor3D/ComUtils.hpp"

namespace CastorCom
{
	CLogger::CLogger()
	{
	}

	CLogger::~CLogger()
	{
	}

	STDMETHODIMP CLogger::Initialise( /* [in] */ eLOG_TYPE level )
	{
		castor::Logger::initialise( castor::LogType( level ) );
		return S_OK;
	}

	STDMETHODIMP CLogger::SetFileName( /* [in] */ BSTR name, eLOG_TYPE target )
	{
		castor::Logger::setFileName( castor::Path{ fromBstr( name ) }, castor::LogType( target ) );
		return S_OK;
	}

	STDMETHODIMP CLogger::Cleanup()
	{
		castor::Logger::cleanup();
		return S_OK;
	}

	STDMETHODIMP CLogger::LogTrace( /* [in] */ BSTR msg )
	{
		castor::Logger::logTrace( fromBstr( msg ) );
		return S_OK;
	}

	STDMETHODIMP CLogger::LogDebug( /* [in] */ BSTR msg )
	{
		castor::Logger::logDebug( fromBstr( msg ) );
		return S_OK;
	}

	STDMETHODIMP CLogger::LogInfo( /* [in] */ BSTR msg )
	{
		castor::Logger::logInfo( fromBstr( msg ) );
		return S_OK;
	}

	STDMETHODIMP CLogger::LogWarning( /* [in] */ BSTR msg )
	{
		castor::Logger::logWarning( fromBstr( msg ) );
		return S_OK;
	}

	STDMETHODIMP CLogger::LogError( /* [in] */ BSTR msg )
	{
		castor::Logger::logError( fromBstr( msg ) );
		return S_OK;
	}

}
