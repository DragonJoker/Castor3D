#include "ComCastor3D/Castor3D/ComSampler.hpp"

#include "ComCastor3D/Castor3D/ComEngine.hpp"

namespace CastorCom
{
	STDMETHODIMP CSampler::Create( /*[in]*/ IEngine * engine, /*[in]*/ BSTR name )noexcept
	{
		if ( !engine || !name )
			return E_POINTER;
		if ( m_internal )
			return dispatchInitialised( _T( "Create" ) );

		return convert( c3dSampler_create( static_cast< CEngine * >( engine )->getInternal()
			, bstrToString( name ).c_str()
			, &m_internal ) );
	}
}
