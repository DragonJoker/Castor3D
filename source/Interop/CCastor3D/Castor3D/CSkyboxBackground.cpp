#include "CCastor3D/Castor3D.h"
#include "CCastor3D/Castor3DCommon.h"

#include <Castor3D/Scene/Background/Skybox.hpp>

#ifdef __cplusplus
extern "C"
{
#endif

	static const C3DString ERROR_UNINITIALISED_SKYBOX = cuT( "The skybox must be initialised" );

	C3D_CAPIMETHODIMP c3dSkybox_delete( C3DSkybox * object )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			cc3d::reportWarning( ERROR_UNINITIALISED_SKYBOX );

		object->internal.reset();
		delete object;

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dSkybox_setLeftImage( C3DSkybox * object, C3DString filePath )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_SKYBOX );

		auto path = c3d::Path{ c3d::makeString( filePath ) };
		object->internal->setLeftImage( path.getPath(), path.getFileName( true ) );

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dSkybox_setRightImage( C3DSkybox * object, C3DString filePath )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_SKYBOX );

		auto path = c3d::Path{ c3d::makeString( filePath ) };
		object->internal->setRightImage( path.getPath(), path.getFileName( true ) );

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dSkybox_setTopImage( C3DSkybox * object, C3DString filePath )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_SKYBOX );

		auto path = c3d::Path{ c3d::makeString( filePath ) };
		object->internal->setTopImage( path.getPath(), path.getFileName( true ) );

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dSkybox_setBottomImage( C3DSkybox * object, C3DString filePath )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_SKYBOX );

		auto path = c3d::Path{ c3d::makeString( filePath ) };
		object->internal->setBottomImage( path.getPath(), path.getFileName( true ) );

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dSkybox_setFrontImage( C3DSkybox * object, C3DString filePath )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_SKYBOX );

		auto path = c3d::Path{ c3d::makeString( filePath ) };
		object->internal->setFrontImage( path.getPath(), path.getFileName( true ) );

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dSkybox_setBackImage( C3DSkybox * object, C3DString filePath )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_SKYBOX );

		auto path = c3d::Path{ c3d::makeString( filePath ) };
		object->internal->setBackImage( path.getPath(), path.getFileName( true ) );

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dSkybox_setCrossImage( C3DSkybox * object, C3DString filePath )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_SKYBOX );

		auto path = c3d::Path{ c3d::makeString( filePath ) };
		object->internal->setCrossTexture( path.getPath(), path.getFileName( true ) );

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dSkybox_setEquirectangularImage( C3DSkybox * object, C3DString filePath, uint32_t size )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_SKYBOX );

		auto path = c3d::Path{ c3d::makeString( filePath ) };
		object->internal->setEquiTexture( path.getPath(), path.getFileName( true ), size );

		return C3D_OK;
	}

#ifdef __cplusplus
}
#endif
