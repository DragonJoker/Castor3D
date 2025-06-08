#include "ComCastor3D/Castor3D/ComMaterial.hpp"
#include "ComCastor3D/Castor3D/ComPass.hpp"

namespace CastorCom
{
	STDMETHODIMP CMaterial::CreatePass( /*[out, retval]*/ IPass ** pVal )noexcept
	{
		if ( !pVal )
			return E_POINTER;
		if ( !m_internal )
			return dispatchUninitialised( _T( "AddLight" ) );
		if ( CPass::CreateInstance( pVal ) != S_OK )
			return E_FAIL;

		return convert( c3dMaterial_createPass( m_internal
			, &static_cast< CPass * >( *pVal )->getInternal() ) );
	}

	STDMETHODIMP CMaterial::GetPass( /*[in]*/ UINT val, /*[out, retval]*/ IPass ** pVal )noexcept
	{
		if ( !pVal )
			return E_POINTER;
		if ( !m_internal )
			return dispatchUninitialised( _T( "GetPass" ) );
		if ( CPass::CreateInstance( pVal ) != S_OK )
			return E_FAIL;

		return convert( c3dMaterial_getPass( m_internal
			, val
			, &static_cast< CPass * >( *pVal )->getInternal() ) );
	}

	STDMETHODIMP CMaterial::RemovePass( /*[in]*/ IPass * val )noexcept
	{
		if ( !val )
			return E_POINTER;
		if ( !m_internal )
			return dispatchUninitialised( _T( "RemovePass" ) );

		return convert( c3dMaterial_removePass( m_internal, static_cast< CPass * >( val )->getInternal() ) );
	}
}
