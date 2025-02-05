#include "ComCastor3D/Castor3D/ComDirectionalLight.hpp"

namespace CastorCom
{
	STDMETHODIMP CDirectionalLight::get_Illumination( /* [retval][out] */ float * pVal )noexcept
	{
		return VariableGetterT< ComTypeGetInternalT< float > >( [this]()
			{
				return m_internal->getIllumination().lux();
			} )( pVal );
	}

	STDMETHODIMP CDirectionalLight::put_Illumination( /* [in] */ float val )noexcept
	{
		return VariablePutterT< float >( [this]( float value )
			{
				m_internal->setIllumination( castor::Illumination{ value } );
			} )( val );
	}
}
