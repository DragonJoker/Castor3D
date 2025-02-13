#include "CCastor3D/Castor3D.h"

#include "CCastor3D/Castor3DCommon.h"

#include <CastorUtils/Data/Path.hpp>
#include <CastorUtils/Log/Logger.hpp>

#ifdef __cplusplus
extern "C"
{
#endif

	static C3DChar const * const ERROR_UNINITIALISED_LOGGER = cuT( "The C3DLogger must be initialised" );
	static C3DChar const * const ERROR_INITIALISED_LOGGER = cuT( "The C3DLogger has already been initialised" );

	C3D_CAPIMETHODIMP c3dLogger_create( C3D_LOG_TYPE level, C3DLogger ** result )
	{
		if ( !result )
			return C3D_POINTER;
		if ( *result && ( *result )->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_INITIALISED_LOGGER );

		try
		{
			auto logger = castor::Logger::initialise( castor::LogType( level ) );
			C3D_SafeAlloc( *result, C3DLogger );
			( *result )->internal = logger;
		}
		C3D_CatchCommonExceptions()

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dLogger_delete( C3DLogger * object )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			cc3d::reportWarning( ERROR_UNINITIALISED_LOGGER );

		castor::Logger::cleanup();
		delete object;

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dLogger_setFileName( C3DLogger * object, C3DString name, C3D_LOG_TYPE target )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_LOGGER );

		try
		{
			object->internal->setFileName( castor::Path{ castor::makeString( name ) }, castor::LogType( target ) );
		}
		C3D_CatchCommonExceptions()

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dLogger_logTrace( C3DLogger * object, C3DString msg )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_LOGGER );

		try
		{
			object->internal->logTrace( castor::makeString( msg ) );
		}
		C3D_CatchCommonExceptions()
		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dLogger_logDebug( C3DLogger * object, C3DString msg )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_LOGGER );

		try
		{
			object->internal->logDebug( castor::makeString( msg ) );
		}
		C3D_CatchCommonExceptions()

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dLogger_logInfo( C3DLogger * object, C3DString msg )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_LOGGER );

		try
		{
			object->internal->logInfo( castor::makeString( msg ) );
		}
		C3D_CatchCommonExceptions()

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dLogger_logWarning( C3DLogger * object, C3DString msg )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_LOGGER );

		try
		{
			object->internal->logWarning( castor::makeString( msg ) );
		}
		C3D_CatchCommonExceptions()

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dLogger_logError( C3DLogger * object, C3DString msg )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_LOGGER );

		try
		{
			object->internal->logError( castor::makeString( msg ) );
		}
		C3D_CatchCommonExceptions()

		return C3D_OK;
	}

#ifdef __cplusplus
}
#endif
