#include "CCastor3D/Castor3D.h"
#include "CCastor3D/Castor3DCommon.h"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Material/Texture/Sampler.hpp>

#ifdef __cplusplus
extern "C"
{
#endif

	static const C3DString ERROR_UNINITIALISED_SAMPLER = cuT( "The sampler must be initialised" );

	C3D_CAPIMETHODIMP c3dSampler_delete( C3DSampler * object )
	{
		if ( !object )
			return C3D_POINTER;
		if ( object->getInternal() )
			cc3d::reportWarning( ERROR_UNINITIALISED_SAMPLER );

		object->releaseInternal();
		delete object;

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dSampler_getMinFilter( C3DSampler const * object, C3D_FILTER_MODE * result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_SAMPLER );

		*result = C3D_FILTER_MODE( object->getInternal()->getMinFilter() );

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dSampler_setMinFilter( C3DSampler * object, C3D_FILTER_MODE val )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_SAMPLER );

		object->getInternal()->setMinFilter( VkFilter( val ) );

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dSampler_getMagFilter( C3DSampler const * object, C3D_FILTER_MODE * result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_SAMPLER );

		*result = C3D_FILTER_MODE( object->getInternal()->getMagFilter() );

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dSampler_setMagFilter( C3DSampler * object, C3D_FILTER_MODE val )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_SAMPLER );

		object->getInternal()->setMagFilter( VkFilter( val ) );

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dSampler_getMipmapMode( C3DSampler const * object, C3D_MIPMAP_MODE * result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_SAMPLER );

		*result = C3D_MIPMAP_MODE( object->getInternal()->getMipFilter() );

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dSampler_setMipmapMode( C3DSampler * object, C3D_MIPMAP_MODE val )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_SAMPLER );

		object->getInternal()->setMipFilter( VkSamplerMipmapMode( val ) );

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dSampler_getWrapModeU( C3DSampler const * object, C3D_WRAP_MODE * result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_SAMPLER );

		*result = C3D_WRAP_MODE( object->getInternal()->getWrapS() );

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dSampler_setWrapModeU( C3DSampler * object, C3D_WRAP_MODE val )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_SAMPLER );

		object->getInternal()->setWrapS( VkSamplerAddressMode( val ) );

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dSampler_getWrapModeV( C3DSampler const * object, C3D_WRAP_MODE * result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_SAMPLER );

		*result = C3D_WRAP_MODE( object->getInternal()->getWrapT() );

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dSampler_setWrapModeV( C3DSampler * object, C3D_WRAP_MODE val )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_SAMPLER );

		object->getInternal()->setWrapT( VkSamplerAddressMode( val ) );

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dSampler_getWrapModeW( C3DSampler const * object, C3D_WRAP_MODE * result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_SAMPLER );

		*result = C3D_WRAP_MODE( object->getInternal()->getWrapR() );

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dSampler_setWrapModeW( C3DSampler * object, C3D_WRAP_MODE val )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_SAMPLER );

		object->getInternal()->setWrapR( VkSamplerAddressMode( val ) );

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dSampler_getMaxAnisotropy( C3DSampler const * object, float * result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_SAMPLER );

		*result = object->getInternal()->getMaxAnisotropy();

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dSampler_setMaxAnisotropy( C3DSampler * object, float val )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_SAMPLER );

		object->getInternal()->setMaxAnisotropy( val );

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dSampler_getMinLod( C3DSampler const * object, float * result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_SAMPLER );

		*result = object->getInternal()->getMinLod();

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dSampler_setMinLod( C3DSampler * object, float val )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_SAMPLER );

		object->getInternal()->setMinLod( val );

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dSampler_getMaxLod( C3DSampler const * object, float * result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_SAMPLER );

		*result = object->getInternal()->getMaxLod();

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dSampler_setMaxLod( C3DSampler * object, float val )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_SAMPLER );

		object->getInternal()->setMaxLod( val );

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dSampler_getLodBias( C3DSampler const * object, float * result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_SAMPLER );

		*result = object->getInternal()->getLodBias();

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dSampler_setLodBias( C3DSampler * object, float val )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_SAMPLER );

		object->getInternal()->setLodBias( val );

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dSampler_getBorderColour( C3DSampler const * object, C3D_BORDER_COLOUR * result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_SAMPLER );

		*result = C3D_BORDER_COLOUR( object->getInternal()->getBorderColour() );

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dSampler_setBorderColour( C3DSampler * object, C3D_BORDER_COLOUR val )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->getInternal() )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_SAMPLER );

		object->getInternal()->setBorderColour( VkBorderColor( val ) );

		return C3D_OK;
	}

#ifdef __cplusplus
}
#endif
