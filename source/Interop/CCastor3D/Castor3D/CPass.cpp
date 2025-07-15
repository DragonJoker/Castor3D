#include "CCastor3D/Castor3D.h"
#include "CCastor3D/Castor3DCommon.h"

#include <Castor3D/Material/Pass/Pass.hpp>
#include <Castor3D/Material/Pass/Component/Base/TwoSidedComponent.hpp>
#include <Castor3D/Material/Pass/Component/Lighting/EmissiveComponent.hpp>
#include <Castor3D/Material/Pass/Component/Lighting/MetalnessComponent.hpp>
#include <Castor3D/Material/Pass/Component/Lighting/RoughnessComponent.hpp>
#include <Castor3D/Material/Pass/Component/Lighting/SpecularComponent.hpp>
#include <Castor3D/Material/Pass/Component/Lighting/SpecularFactorComponent.hpp>
#include <Castor3D/Material/Pass/Component/Other/AlphaTestComponent.hpp>
#include <Castor3D/Material/Pass/Component/Other/ColourComponent.hpp>
#include <Castor3D/Material/Pass/Component/Other/OpacityComponent.hpp>
#include <Castor3D/Material/Pass/Component/Other/RefractionComponent.hpp>

#ifdef __cplusplus
extern "C"
{
#endif

	static const C3DString ERROR_UNINITIALISED_PASS = cuT( "The pass must be initialised" );
	static const C3DString ERROR_COMPONENT_NOT_FOUND = cuT( "The required component was not found" );

	C3D_CAPIMETHODIMP c3dPass_delete( C3DPass * object )
	{
		if ( !object )
			return C3D_POINTER;

		delete object;

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dPass_getTwoSided( C3DPass const * object, bool * result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_PASS );

		auto component = object->internal->getComponent< c3d::TwoSidedComponent >();
		if ( !component )
			return cc3d::reportError( C3D_FAILURE, ERROR_COMPONENT_NOT_FOUND );

		*result = component->isTwoSided();

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dPass_setTwoSided( C3DPass * object, bool val )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_PASS );

		if ( auto component = object->internal->createComponent< c3d::TwoSidedComponent >() )
		{
			component->setTwoSided( val );
		}

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dPass_getOpacity( C3DPass const * object, float * result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_PASS );

		auto component = object->internal->getComponent< c3d::OpacityComponent >();
		if ( !component )
			return cc3d::reportError( C3D_FAILURE, ERROR_COMPONENT_NOT_FOUND );

		*result = component->getOpacity();

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dPass_setOpacity( C3DPass * object, float val )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_PASS );

		if ( auto component = object->internal->createComponent< c3d::OpacityComponent >() )
		{
			component->setOpacity( val );
		}

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dPass_getEmissiveColour( C3DPass const * object, C3DRgbColour * result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_PASS );

		auto component = object->internal->getComponent< c3d::EmissiveComponent >();
		if ( !component )
			return cc3d::reportError( C3D_FAILURE, ERROR_COMPONENT_NOT_FOUND );

		*result = cc3d::convert( component->getEmissiveColour() );

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dPass_setEmissiveColour( C3DPass * object, C3DRgbColour const * val )
	{
		if ( !object || !val )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_PASS );

		if ( auto component = object->internal->createComponent< c3d::EmissiveComponent >() )
		{
			component->setEmissive( cc3d::convert( *val ) );
		}

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dPass_getEmissiveFactor( C3DPass const * object, float* result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_PASS );

		auto component = object->internal->getComponent< c3d::EmissiveComponent >();
		if ( !component )
			return cc3d::reportError( C3D_FAILURE, ERROR_COMPONENT_NOT_FOUND );

		*result = component->getEmissiveFactor();

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dPass_setEmissiveFactor( C3DPass * object, float val )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_PASS );

		if ( auto component = object->internal->createComponent< c3d::EmissiveComponent >() )
		{
			component->setEmissiveFactor( val );
		}

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dPass_getRefractionRatio( C3DPass const * object, float * result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_PASS );

		auto component = object->internal->getComponent< c3d::RefractionComponent >();
		if ( !component )
			return cc3d::reportError( C3D_FAILURE, ERROR_COMPONENT_NOT_FOUND );

		*result = component->getRefractionRatio();

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dPass_setRefractionRatio( C3DPass * object, float val )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_PASS );

		if ( auto component = object->internal->createComponent< c3d::RefractionComponent >() )
		{
			component->setRefractionRatio( val );
		}

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dPass_getAlphaFunc( C3DPass const * object, C3D_COMPARISON_FUNC * result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_PASS );

		auto component = object->internal->getComponent< c3d::AlphaTestComponent >();
		if ( !component )
			return cc3d::reportError( C3D_FAILURE, ERROR_COMPONENT_NOT_FOUND );

		*result = C3D_COMPARISON_FUNC( component->getAlphaFunc() );

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dPass_setAlphaFunc( C3DPass * object, C3D_COMPARISON_FUNC val )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_PASS );

		if ( auto component = object->internal->createComponent< c3d::AlphaTestComponent >() )
		{
			component->setAlphaFunc( c3d::ComparisonFunc( val ) );
		}

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dPass_getBlendAlphaFunc( C3DPass const * object, C3D_COMPARISON_FUNC * result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_PASS );

		auto component = object->internal->getComponent< c3d::AlphaTestComponent >();
		if ( !component )
			return cc3d::reportError( C3D_FAILURE, ERROR_COMPONENT_NOT_FOUND );

		*result = C3D_COMPARISON_FUNC( component->getBlendAlphaFunc() );

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dPass_setBlendAlphaFunc( C3DPass * object, C3D_COMPARISON_FUNC val )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_PASS );

		if ( auto component = object->internal->createComponent< c3d::AlphaTestComponent >() )
		{
			component->setBlendAlphaFunc( c3d::ComparisonFunc( val ) );
		}

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dPass_getAlphaRefValue( C3DPass const * object, float * result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_PASS );

		auto component = object->internal->getComponent< c3d::AlphaTestComponent >();
		if ( !component )
			return cc3d::reportError( C3D_FAILURE, ERROR_COMPONENT_NOT_FOUND );

		*result = component->getAlphaRefValue();

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dPass_setAlphaRefValue( C3DPass * object, float val )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_PASS );

		if ( auto component = object->internal->createComponent< c3d::AlphaTestComponent >() )
		{
			component->setAlphaRefValue( val );
		}

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dPass_getDiffuse( C3DPass const * object, C3DRgbColour * result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_PASS );

		auto component = object->internal->getComponent< c3d::ColourComponent >();
		if ( !component )
			return cc3d::reportError( C3D_FAILURE, ERROR_COMPONENT_NOT_FOUND );

		*result = cc3d::convert( c3d::RgbColour{ component->getColour(), 2.2f } );

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dPass_setDiffuse( C3DPass * object, C3DRgbColour const * val )
	{
		if ( !object || !val )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_PASS );

		if ( auto component = object->internal->createComponent< c3d::ColourComponent >() )
		{
			component->setColour( cc3d::convert( *val ) );
		}

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dPass_getAlbedo( C3DPass const * object, C3DHdrRgbColour * result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_PASS );

		auto component = object->internal->getComponent< c3d::ColourComponent >();
		if ( !component )
			return cc3d::reportError( C3D_FAILURE, ERROR_COMPONENT_NOT_FOUND );

		*result = cc3d::convert( component->getColour() );

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dPass_setAlbedo( C3DPass * object, C3DHdrRgbColour const * val )
	{
		if ( !object || !val )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_PASS );

		if ( auto component = object->internal->createComponent< c3d::ColourComponent >() )
		{
			component->setColour( cc3d::convert( *val ) );
		}

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dPass_getSpecular( C3DPass const * object, C3DRgbColour * result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_PASS );

		auto component = object->internal->getComponent< c3d::SpecularComponent >();
		if ( !component )
			return cc3d::reportError( C3D_FAILURE, ERROR_COMPONENT_NOT_FOUND );

		*result = cc3d::convert( component->getSpecular() );

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dPass_setSpecular( C3DPass * object, C3DRgbColour const * val )
	{
		if ( !object || !val )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_PASS );

		if ( auto component = object->internal->createComponent< c3d::SpecularComponent >() )
		{
			component->setSpecular( cc3d::convert( *val ) );
		}

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dPass_getSpecularFactor( C3DPass const * object, float * result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_PASS );

		auto component = object->internal->getComponent< c3d::SpecularFactorComponent >();
		if ( !component )
			return cc3d::reportError( C3D_FAILURE, ERROR_COMPONENT_NOT_FOUND );

		*result = component->getFactor();

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dPass_setSpecularFactor( C3DPass * object, float val )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_PASS );

		if ( auto component = object->internal->createComponent< c3d::SpecularFactorComponent >() )
		{
			component->setFactor( val );
		}

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dPass_getMetalness( C3DPass const * object, float * result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_PASS );

		auto component = object->internal->getComponent< c3d::MetalnessComponent >();
		if ( !component )
			return cc3d::reportError( C3D_FAILURE, ERROR_COMPONENT_NOT_FOUND );

		*result = component->getMetalness();

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dPass_setMetalness( C3DPass * object, float val )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_PASS );

		if ( auto component = object->internal->createComponent< c3d::MetalnessComponent >() )
		{
			component->setMetalness( val );
		}

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dPass_getRoughness( C3DPass const * object, float * result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_PASS );

		auto component = object->internal->getComponent< c3d::RoughnessComponent >();
		if ( !component )
			return cc3d::reportError( C3D_FAILURE, ERROR_COMPONENT_NOT_FOUND );

		*result = component->getRoughness();

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dPass_setRoughness( C3DPass * object, float val )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_PASS );

		if ( auto component = object->internal->createComponent< c3d::RoughnessComponent >() )
		{
			component->setRoughness( val );
		}

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dPass_getGlossiness( C3DPass const * object, float * result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_PASS );

		auto component = object->internal->getComponent< c3d::RoughnessComponent >();
		if ( !component )
			return cc3d::reportError( C3D_FAILURE, ERROR_COMPONENT_NOT_FOUND );

		*result = component->getGlossiness();

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dPass_setGlossiness( C3DPass * object, float val )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_PASS );

		if ( auto component = object->internal->createComponent< c3d::RoughnessComponent >() )
		{
			component->setGlossiness( val );
		}

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dPass_getShininess( C3DPass const * object, float * result )
	{
		if ( !object || !result )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_PASS );

		auto component = object->internal->getComponent< c3d::RoughnessComponent >();
		if ( !component )
			return cc3d::reportError( C3D_FAILURE, ERROR_COMPONENT_NOT_FOUND );

		*result = component->getShininess();

		return C3D_OK;
	}

	C3D_CAPIMETHODIMP c3dPass_setShininess( C3DPass * object, float val )
	{
		if ( !object )
			return C3D_POINTER;
		if ( !object->internal )
			return cc3d::reportError( C3D_FAILURE, ERROR_UNINITIALISED_PASS );

		if ( auto component = object->internal->createComponent< c3d::RoughnessComponent >() )
		{
			component->setShininess( val );
		}

		return C3D_OK;
	}

#ifdef __cplusplus
}
#endif
