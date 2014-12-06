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
		Castor::Logger::Initialise( Castor::eLOG_TYPE( level ) );
		return S_OK;
	}

	STDMETHODIMP CLogger::SetFileName( /* [in] */ BSTR name, eLOG_TYPE target )
	{
		Castor::Logger::SetFileName( FromBstr( name ), Castor::eLOG_TYPE( target ) );
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

	STDMETHODIMP CLogger::LogMessage( /* [in] */ BSTR msg )
	{
		Castor::Logger::LogMessage( FromBstr( msg ) );
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
