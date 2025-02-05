#include "ComCastor3D/Castor3D/ComShadow.hpp"

namespace CastorCom
{
	STDMETHODIMP CShadow::get_Enabled( /*[out, retval]*/ boolean * pVal )noexcept
	{
		return VariableGetterT< boolean >( [this]()
			{
				return boolean( m_internal.enabled );
			} )( pVal );
	}
	STDMETHODIMP CShadow::put_Enabled( /*[in]*/ boolean val )noexcept
	{
		return VariablePutterT< boolean >( [this]( boolean value )
			{
				m_internal.enabled = ( value != 0 );
			} )( val );
	}
	STDMETHODIMP CShadow::get_Filter( /*[out, retval]*/ eSHADOW_TYPE * pVal )noexcept
	{
		return VariableGetterT< eSHADOW_TYPE >( [this]()
			{
				return eSHADOW_TYPE( m_internal.filterType );
			} )( pVal );
	}
	STDMETHODIMP CShadow::put_Filter( /*[in]*/ eSHADOW_TYPE val )noexcept
	{
		return VariablePutterT< eSHADOW_TYPE >( [this]( eSHADOW_TYPE value )
			{
				m_internal.filterType = castor3d::ShadowType( value );
			} )( val );
	}
	STDMETHODIMP CShadow::get_PcfFilterSize( /*[out, retval]*/ UINT * pVal )noexcept
	{
		return VariableGetterT< UINT >( [this]()
			{
				return m_internal.pcfFilterSize.value();
			} )( pVal );
	}
	STDMETHODIMP CShadow::put_PcfFilterSize( /*[in]*/ UINT val )noexcept
	{
		return VariablePutterT< UINT >( [this]( UINT value )
			{
				m_internal.pcfFilterSize = value;
			} )( val );
	}
	STDMETHODIMP CShadow::get_PcfSampleCount( /*[out, retval]*/ UINT * pVal )noexcept
	{
		return VariableGetterT< UINT >( [this]()
			{
				return m_internal.pcfSampleCount.value();
			} )( pVal );
	}
	STDMETHODIMP CShadow::put_PcfSampleCount( /*[in]*/ UINT val )noexcept
	{
		return VariablePutterT< UINT >( [this]( UINT value )
			{
				m_internal.pcfSampleCount = value;
			} )( val );
	}
}
