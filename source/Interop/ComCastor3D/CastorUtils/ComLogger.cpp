#include "ComCastor3D/CastorUtils/ComLogger.hpp"
#include "ComCastor3D/ComUtils.hpp"

namespace CastorCom
{
	STDMETHODIMP CLogger::Initialise( /* [in] */ eLOG_TYPE level )noexcept
	{
		castor::Logger::initialise( castor::LogType( level ) );
		return S_OK;
	}

	STDMETHODIMP CLogger::SetFileName( /* [in] */ BSTR name, eLOG_TYPE target )noexcept
	{
		castor::Logger::setFileName( castor::Path{ fromBstr( name ) }, castor::LogType( target ) );
		return S_OK;
	}

	STDMETHODIMP CLogger::Cleanup()noexcept
	{
		castor::Logger::cleanup();
		return S_OK;
	}

	STDMETHODIMP CLogger::LogTrace( /* [in] */ BSTR msg )noexcept
	{
		castor::Logger::logTrace( fromBstr( msg ) );
		return S_OK;
	}

	STDMETHODIMP CLogger::LogDebug( /* [in] */ BSTR msg )noexcept
	{
		castor::Logger::logDebug( fromBstr( msg ) );
		return S_OK;
	}

	STDMETHODIMP CLogger::LogInfo( /* [in] */ BSTR msg )noexcept
	{
		castor::Logger::logInfo( fromBstr( msg ) );
		return S_OK;
	}

	STDMETHODIMP CLogger::LogWarning( /* [in] */ BSTR msg )noexcept
	{
		castor::Logger::logWarning( fromBstr( msg ) );
		return S_OK;
	}

	STDMETHODIMP CLogger::LogError( /* [in] */ BSTR msg )noexcept
	{
		castor::Logger::logError( fromBstr( msg ) );
		return S_OK;
	}

}
