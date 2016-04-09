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

	STDMETHODIMP CLogger::Initialise( /* [in] */ eLOG_TYPE level )
	{
		Castor::Logger::Initialise( Castor::ELogType( level ) );
		return S_OK;
	}

	STDMETHODIMP CLogger::SetFileName( /* [in] */ BSTR name, eLOG_TYPE target )
	{
		Castor::Logger::SetFileName( FromBstr( name ), Castor::ELogType( target ) );
		return S_OK;
	}

	STDMETHODIMP CLogger::Cleanup()
	{
		Castor::Logger::Cleanup();
		return S_OK;
	}

	STDMETHODIMP CLogger::LogDebug( /* [in] */ BSTR msg )
	{
		Castor::Logger::LogDebug( FromBstr( msg ) );
		return S_OK;
	}

	STDMETHODIMP CLogger::LogInfo( /* [in] */ BSTR msg )
	{
		Castor::Logger::LogInfo( FromBstr( msg ) );
		return S_OK;
	}

	STDMETHODIMP CLogger::LogWarning( /* [in] */ BSTR msg )
	{
		Castor::Logger::LogWarning( FromBstr( msg ) );
		return S_OK;
	}

	STDMETHODIMP CLogger::LogError( /* [in] */ BSTR msg )
	{
		Castor::Logger::LogError( FromBstr( msg ) );
		return S_OK;
	}

}
