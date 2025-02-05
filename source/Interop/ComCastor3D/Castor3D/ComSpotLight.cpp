#include "ComCastor3D/Castor3D/ComSpotLight.hpp"

namespace CastorCom
{
	STDMETHODIMP CSpotLight::get_Intensity( /* [retval][out] */ float * pVal )noexcept
	{
		return VariableGetterT< ComTypeGetInternalT< float > >( [this]()
			{
				return m_internal->getIntensity().candela();
			} )( pVal );
	}

	STDMETHODIMP CSpotLight::put_Intensity( /* [in] */ float val )noexcept
	{
		return VariablePutterT< float >( [this]( float value )
			{
				m_internal->setIntensity( castor::LuminousIntensity{ value } );
			} )( val );
	}
}
