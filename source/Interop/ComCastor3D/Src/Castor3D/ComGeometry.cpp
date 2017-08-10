#include "ComGeometry.hpp"
#include "ComSceneNode.hpp"
#include "ComMaterial.hpp"
#include "ComSubmesh.hpp"

namespace CastorCom
{
	static const castor::String ERROR_UNINITIALISED = cuT( "The geometry must be initialised" );

	CGeometry::CGeometry()
	{
	}

	CGeometry::~CGeometry()
	{
	}

	STDMETHODIMP CGeometry::attachTo( /* [in] */ ISceneNode * val )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->attachTo( static_cast< CSceneNode * >( val )->getInternal() );
			hr = S_OK;
		}
		else
		{
			hr = CComError::DispatchError(
					 E_FAIL,						// This represents the error
					 IID_IGeometry,					// This is the GUID of PixelComponents throwing error
					 cuT( "attachTo" ),				// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 NULL );
		}

		return hr;
	}

	STDMETHODIMP CGeometry::detach()
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->detach();
			hr = S_OK;
		}
		else
		{
			hr = CComError::DispatchError(
					 E_FAIL,						// This represents the error
					 IID_IGeometry,					// This is the GUID of PixelComponents throwing error
					 cuT( "Detach" ),				// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 NULL );
		}

		return hr;
	}

	STDMETHODIMP CGeometry::getMaterial( /* [in] */ ISubmesh * submesh, /* [out, retval] */ IMaterial ** pVal )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			if ( pVal )
			{
				hr = CMaterial::CreateInstance( pVal );

				if ( hr == S_OK )
				{
					static_cast< CMaterial * >( *pVal )->setInternal( m_internal->getMaterial( *static_cast< CSubmesh * >( submesh )->getInternal() ) );
				}
			}
		}
		else
		{
			hr = CComError::DispatchError(
					 E_FAIL,						// This represents the error
					 IID_IGeometry,					// This is the GUID of PixelComponents throwing error
					 cuT( "getMaterial" ),			// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 NULL );
		}

		return hr;
	}

	STDMETHODIMP CGeometry::setMaterial( /* [in] */ ISubmesh * submesh, /* [in] */ IMaterial * val )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->setMaterial( *static_cast< CSubmesh * >( submesh )->getInternal(), static_cast< CMaterial * >( val )->getInternal() );
			hr = S_OK;
		}
		else
		{
			hr = CComError::DispatchError(
					 E_FAIL,						// This represents the error
					 IID_IGeometry,					// This is the GUID of PixelComponents throwing error
					 cuT( "setMaterial" ),			// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 NULL );
		}

		return hr;
	}
}
