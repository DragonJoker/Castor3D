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
			auto logger = c3d::Logger::initialise( c3d::LogType( level ) );
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

		c3d::Logger::cleanup();
		delete object;

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dLogger_setFileName( C3DLogger const * object, C3DString name, C3D_LOG_TYPE target )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_LOGGER );

		try
		{
			object->internal->setFileName( c3d::Path{ c3d::makeString( name ) }, c3d::LogType( target ) );
		}
		C3D_CatchCommonExceptions()

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dLogger_logTrace( C3DLogger const * object, C3DString msg )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_LOGGER );

		try
		{
			object->internal->logTrace( c3d::makeString( msg ) );
		}
		C3D_CatchCommonExceptions()
		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dLogger_logDebug( C3DLogger const * object, C3DString msg )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_LOGGER );

		try
		{
			object->internal->logDebug( c3d::makeString( msg ) );
		}
		C3D_CatchCommonExceptions()

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dLogger_logInfo( C3DLogger const * object, C3DString msg )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_LOGGER );

		try
		{
			object->internal->logInfo( c3d::makeString( msg ) );
		}
		C3D_CatchCommonExceptions()

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dLogger_logWarning( C3DLogger const * object, C3DString msg )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_LOGGER );

		try
		{
			object->internal->logWarning( c3d::makeString( msg ) );
		}
		C3D_CatchCommonExceptions()

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dLogger_logError( C3DLogger const * object, C3DString msg )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_LOGGER );

		try
		{
			object->internal->logError( c3d::makeString( msg ) );
		}
		C3D_CatchCommonExceptions()

		return C3D_OK;
	}

#ifdef __cplusplus
}
#endif
