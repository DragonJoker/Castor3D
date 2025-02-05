#include "ComCastor3D/Castor3D/ComPointLight.hpp"

namespace CastorCom
{
	STDMETHODIMP CPointLight::get_Intensity( /* [retval][out] */ float * pVal )noexcept
	{
		return VariableGetterT< ComTypeGetInternalT< float > >( [this]()
			{
				return m_internal->getIntensity().candela();
			} )( pVal );
	}

	STDMETHODIMP CPointLight::put_Intensity( /* [in] */ float val )noexcept
	{
		return VariablePutterT< float >( [this]( float value )
			{
				m_internal->setIntensity( castor::LuminousIntensity{ value } );
			} )( val );
	}
}
