#include "ComCastor3D/CastorUtils/ComLogger.hpp"
#include "ComCastor3D/ComUtils.hpp"

namespace CastorCom
{
	STDMETHODIMP CLogger::Create( /*[in]*/ eLOG_TYPE level )noexcept
	{
		if ( m_internal )
			return dispatchInitialised( _T( "Create" ) );

		return convert( c3dLogger_create( C3D_LOG_TYPE( level ), &m_internal ) );
	}

	STDMETHODIMP CLogger::SetFileName( /*[in]*/ BSTR name, eLOG_TYPE target )noexcept
	{
		if ( !m_internal )
			return dispatchUninitialised( _T( "SetFileName" ) );

		return convert( c3dLogger_setFileName( m_internal, bstrToString( name ).c_str(), C3D_LOG_TYPE( target ) ) );
	}

	STDMETHODIMP CLogger::LogTrace( /*[in]*/ BSTR msg )noexcept
	{
		if ( !m_internal )
			return dispatchUninitialised( _T( "LogTrace" ) );

		return convert( c3dLogger_logTrace( m_internal, bstrToString( msg ).c_str() ) );
	}

	STDMETHODIMP CLogger::LogDebug( /*[in]*/ BSTR msg )noexcept
	{
		if ( !m_internal )
			return dispatchUninitialised( _T( "LogDebug" ) );

		return convert( c3dLogger_logDebug( m_internal, bstrToString( msg ).c_str() ) );
	}

	STDMETHODIMP CLogger::LogInfo( /*[in]*/ BSTR msg )noexcept
	{
		if ( !m_internal )
			return dispatchUninitialised( _T( "LogInfo" ) );

		return convert( c3dLogger_logInfo( m_internal, bstrToString( msg ).c_str() ) );
	}

	STDMETHODIMP CLogger::LogWarning( /*[in]*/ BSTR msg )noexcept
	{
		if ( !m_internal )
			return dispatchUninitialised( _T( "LogWarning" ) );

		return convert( c3dLogger_logWarning( m_internal, bstrToString( msg ).c_str() ) );
	}

	STDMETHODIMP CLogger::LogError( /*[in]*/ BSTR msg )noexcept
	{
		if ( !m_internal )
			return dispatchUninitialised( _T( "LogError" ) );

		return convert( c3dLogger_logError( m_internal, bstrToString( msg ).c_str() ) );
	}

}
