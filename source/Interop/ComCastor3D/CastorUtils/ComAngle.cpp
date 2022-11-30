#include "ComCastor3D/CastorUtils/ComAngle.hpp"

namespace CastorCom
{
	STDMETHODIMP CAngle::Cos( /* [out, retval] */ float * pVal )noexcept
	{
		return retrieveValue( pVal, [this]()
		{
			return float( m_internal.cos() );
		} );
	}

	STDMETHODIMP CAngle::Sin( /* [out, retval] */ float * pVal )noexcept
	{
		return retrieveValue( pVal, [this]()
		{
			return float( m_internal.sin() );
		} );
	}

	STDMETHODIMP CAngle::Tan( /* [out, retval] */ float * pVal )noexcept
	{
		return retrieveValue( pVal, [this]()
		{
			return float( m_internal.tan() );
		} );
	}

	STDMETHODIMP CAngle::Cosh( /* [out, retval] */ float * pVal )noexcept
	{
		return retrieveValue( pVal, [this]()
		{
			return float( m_internal.cosh() );
		} );
	}

	STDMETHODIMP CAngle::Sinh( /* [out, retval] */ float * pVal )noexcept
	{
		return retrieveValue( pVal, [this]()
		{
			return float( m_internal.sinh() );
		} );
	}

	STDMETHODIMP CAngle::Tanh( /* [out, retval] */ float * pVal )noexcept
	{
		return retrieveValue( pVal, [this]()
		{
			return float( m_internal.tanh() );
		} );
	}

	STDMETHODIMP CAngle::ACos( /* [in] */ float val )noexcept
	{
		m_internal = castor::acosf( val );
		return S_OK;
	}

	STDMETHODIMP CAngle::ASin( /* [in] */ float val )noexcept
	{
		m_internal = castor::asinf( val );
		return S_OK;
	}

	STDMETHODIMP CAngle::ATan( /* [in] */ float val )noexcept
	{
		m_internal = castor::atanf( val );
		return S_OK;
	}
}
