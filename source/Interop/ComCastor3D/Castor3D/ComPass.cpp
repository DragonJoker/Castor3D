#include "ComCastor3D/Castor3D/ComPass.hpp"
#include "ComCastor3D/CastorUtils/ComLogger.hpp"
#include "ComCastor3D/CastorUtils/ComSize.hpp"

#include <Castor3D/Material/Pass/Component/Base/TwoSidedComponent.hpp>
#include <Castor3D/Material/Pass/Component/Lighting/EmissiveComponent.hpp>
#include <Castor3D/Material/Pass/Component/Lighting/MetalnessComponent.hpp>
#include <Castor3D/Material/Pass/Component/Lighting/RoughnessComponent.hpp>
#include <Castor3D/Material/Pass/Component/Lighting/SpecularComponent.hpp>
#include <Castor3D/Material/Pass/Component/Other/AlphaTestComponent.hpp>
#include <Castor3D/Material/Pass/Component/Other/ColourComponent.hpp>
#include <Castor3D/Material/Pass/Component/Other/OpacityComponent.hpp>
#include <Castor3D/Material/Pass/Component/Other/RefractionComponent.hpp>

namespace CastorCom
{
	namespace pass
	{
		static const tstring ERROR_UNINITIALISED = _T( "The pass must be initialised" );
	}

	castor::RgbColour CPass::getEmissiveColour( castor3d::Pass const * pass )
	{
		castor::RgbColour result{ 1.0f, 1.0f, 1.0f };

		if ( auto component = pass->getComponent< castor3d::EmissiveComponent >() )
		{
			result = component->getEmissiveColour();
		}

		return result;
	}

	void CPass::setEmissiveColour( castor3d::Pass * pass, castor::RgbColour v )
	{
		if ( auto component = pass->createComponent< castor3d::EmissiveComponent >() )
		{
			component->setEmissive( v );
		}
	}

	float CPass::getEmissiveFactor( castor3d::Pass const * pass )
	{
		if ( auto component = pass->getComponent< castor3d::EmissiveComponent >() )
		{
			return component->getEmissiveFactor();
		}

		return 0.0f;
	}

	void CPass::setEmissiveFactor( castor3d::Pass * pass, float v )
	{
		if ( auto component = pass->createComponent< castor3d::EmissiveComponent >() )
		{
			component->setEmissiveFactor( v );
		}
	}

	float CPass::getRefractionRatio( castor3d::Pass const * pass )
	{
		if ( auto component = pass->getComponent< castor3d::RefractionComponent >() )
		{
			return component->getRefractionRatio();
		}

		return 0.0f;
	}

	void CPass::setRefractionRatio( castor3d::Pass * pass, float v )
	{
		if ( auto component = pass->createComponent< castor3d::RefractionComponent >() )
		{
			component->setRefractionRatio( v );
		}
	}

	eCOMPARISON_FUNC CPass::getAlphaFunc( castor3d::Pass const * pass )
	{
		if ( auto component = pass->getComponent< castor3d::AlphaTestComponent >() )
		{
			return eCOMPARISON_FUNC( component->getAlphaFunc() );
		}

		return eCOMPARISON_FUNC_ALWAYS;
	}

	void CPass::setAlphaFunc( castor3d::Pass * pass, eCOMPARISON_FUNC v )
	{
		if ( auto component = pass->createComponent< castor3d::AlphaTestComponent >() )
		{
			component->setAlphaFunc( VkCompareOp( v ) );
		}
	}

	eCOMPARISON_FUNC CPass::getBlendAlphaFunc( castor3d::Pass const * pass )
	{
		if ( auto component = pass->getComponent< castor3d::AlphaTestComponent >() )
		{
			return eCOMPARISON_FUNC( component->getBlendAlphaFunc() );
		}

		return eCOMPARISON_FUNC_ALWAYS;
	}

	void CPass::setBlendAlphaFunc( castor3d::Pass * pass, eCOMPARISON_FUNC v )
	{
		if ( auto component = pass->createComponent< castor3d::AlphaTestComponent >() )
		{
			component->setBlendAlphaFunc( VkCompareOp( v ) );
		}
	}

	float CPass::getAlphaValue( castor3d::Pass const * pass )
	{
		if ( auto component = pass->getComponent< castor3d::AlphaTestComponent >() )
		{
			return component->getAlphaRefValue();
		}

		return 0.0f;
	}

	void CPass::setAlphaValue( castor3d::Pass * pass, float v )
	{
		if ( auto component = pass->createComponent< castor3d::AlphaTestComponent >() )
		{
			component->setAlphaRefValue( v );
		}
	}

	bool CPass::isTwoSided( castor3d::Pass const * pass )
	{
		if ( auto component = pass->getComponent< castor3d::TwoSidedComponent >() )
		{
			return component->isTwoSided();
		}

		return false;
	}

	void CPass::setTwoSided( castor3d::Pass * pass, bool v )
	{
		if ( auto component = pass->createComponent< castor3d::TwoSidedComponent >() )
		{
			component->setTwoSided( v );
		}
	}

	float CPass::getOpacity( castor3d::Pass const * pass )
	{
		if ( auto component = pass->getComponent< castor3d::OpacityComponent >() )
		{
			return component->getOpacity();
		}

		return 1.0f;
	}

	void CPass::setOpacity( castor3d::Pass * pass, float v )
	{
		if ( auto component = pass->createComponent< castor3d::OpacityComponent >() )
		{
			component->setOpacity( v );
		}
	}

	castor::HdrRgbColour CPass::getAlbedo( castor3d::Pass const * pass )
	{
		castor::HdrRgbColour result{ 1.0f, 1.0f, 1.0f };

		if ( auto component = pass->getComponent< castor3d::ColourComponent >() )
		{
			result = component->getColour();
		}

		return result;
	}

	void CPass::setAlbedo( castor3d::Pass * pass, castor::HdrRgbColour v )
	{
		if ( auto component = pass->createComponent< castor3d::ColourComponent >() )
		{
			component->setColour( v );
		}
	}

	castor::RgbColour CPass::getDiffuse( castor3d::Pass const * pass )
	{
		castor::RgbColour result{ 1.0f, 1.0f, 1.0f };

		if ( auto component = pass->getComponent< castor3d::ColourComponent >() )
		{
			result = castor::RgbColour{ component->getColour(), 2.2f };
		}

		return result;
	}

	void CPass::setDiffuse( castor3d::Pass * pass, castor::RgbColour v )
	{
		if ( auto component = pass->createComponent< castor3d::ColourComponent >() )
		{
			component->setColour( v );
		}
	}

	castor::RgbColour CPass::getSpecular( castor3d::Pass const * pass )
	{
		castor::RgbColour result{ 1.0f, 1.0f, 1.0f };

		if ( auto component = pass->getComponent< castor3d::SpecularComponent >() )
		{
			result = component->getSpecular();
		}

		return result;
	}

	void CPass::setSpecular( castor3d::Pass * pass, castor::RgbColour v )
	{
		if ( auto component = pass->createComponent< castor3d::SpecularComponent >() )
		{
			component->setSpecular( v );
		}
	}

	float CPass::getMetalness( castor3d::Pass const * pass )
	{
		if ( auto component = pass->getComponent< castor3d::MetalnessComponent >() )
		{
			return component->getMetalness();
		}

		return 0.0f;
	}

	void CPass::setMetalness( castor3d::Pass * pass, float v )
	{
		if ( auto component = pass->createComponent< castor3d::MetalnessComponent >() )
		{
			component->setMetalness( v );
		}
	}

	float CPass::getRoughness( castor3d::Pass const * pass )
	{
		if ( auto component = pass->getComponent< castor3d::RoughnessComponent >() )
		{
			return component->getRoughness();
		}

		return 0.0f;
	}

	void CPass::setRoughness( castor3d::Pass * pass, float v )
	{
		if ( auto component = pass->createComponent< castor3d::RoughnessComponent >() )
		{
			component->setRoughness( v );
		}
	}

	float CPass::getGlossiness( castor3d::Pass const * pass )
	{
		if ( auto component = pass->getComponent< castor3d::RoughnessComponent >() )
		{
			return component->getGlossiness();
		}

		return 1.0f;
	}

	void CPass::setGlossiness( castor3d::Pass * pass, float v )
	{
		if ( auto component = pass->createComponent< castor3d::RoughnessComponent >() )
		{
			component->setGlossiness( v );
		}
	}

	float CPass::getShininess( castor3d::Pass const * pass )
	{
		if ( auto component = pass->getComponent< castor3d::RoughnessComponent >() )
		{
			return component->getShininess();
		}

		return castor3d::MaxPhongShininess;
	}

	void CPass::setShininess( castor3d::Pass * pass, float v )
	{
		if ( auto component = pass->createComponent< castor3d::RoughnessComponent >() )
		{
			component->setShininess( v );
		}
	}

	STDMETHODIMP CPass::CreateTextureUnit( /* [out, retval] */ ITextureUnit ** pVal )noexcept
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
		}
		else
		{
			hr = CComError::dispatchError(
					 E_FAIL,						// This represents the error
					 IID_IPass,						// This is the GUID of PixelComponents throwing error
					 _T( "CreateTextureUnit" ),		// This is generally displayed as the title
					pass::ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 nullptr );
		}

		return hr;
	}

	STDMETHODIMP CPass::DestroyTextureUnit( /* [in] */ ITextureUnit * val )noexcept
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
		}
		else
		{
			hr = CComError::dispatchError(
					 E_FAIL,						// This represents the error
					 IID_IPass,						// This is the GUID of PixelComponents throwing error
					 _T( "DestroyTextureUnit" ),	// This is generally displayed as the title
					 pass::ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 nullptr );
		}

		return hr;
	}

	STDMETHODIMP CPass::GetTextureUnitByIndex( /* [in] */ unsigned int index, /* [out, retval] */ ITextureUnit ** pVal )noexcept
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
		}
		else
		{
			hr = CComError::dispatchError(
					 E_FAIL,						// This represents the error
					 IID_IPass,						// This is the GUID of PixelComponents throwing error
					 _T( "GetTextureUnitByIndex" ),	// This is generally displayed as the title
					 pass::ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 nullptr );
		}

		return hr;
	}
}
