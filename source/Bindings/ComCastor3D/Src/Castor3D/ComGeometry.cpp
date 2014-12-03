#include "ComGeometry.hpp"
#include "ComSceneNode.hpp"
#include "ComMaterial.hpp"
#include "ComSubmesh.hpp"

namespace CastorCom
{
	static const Castor::String ERROR_UNINITIALISED = cuT( "The geometry must be initialised" );

	CGeometry::CGeometry()
	{
	}

	CGeometry::~CGeometry()
	{
	}

	STDMETHODIMP CGeometry::AttachTo( /* [in] */ ISceneNode * val )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->AttachTo( static_cast< CSceneNode * >( val )->GetInternal().get() );
			hr = S_OK;
		}
		else
		{
			hr = CComError::DispatchError(
					 E_FAIL,						// This represents the error
					 IID_IGeometry,					// This is the GUID of component throwing error
					 cuT( "AttachTo" ),				// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 NULL );
		}

		return hr;
	}

	STDMETHODIMP CGeometry::Detach()
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->Detach();
			hr = S_OK;
		}
		else
		{
			hr = CComError::DispatchError(
					 E_FAIL,						// This represents the error
					 IID_IGeometry,					// This is the GUID of component throwing error
					 cuT( "Detach" ),				// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 NULL );
		}

		return hr;
	}

	STDMETHODIMP CGeometry::GetMaterial( /* [in] */ ISubmesh * submesh, /* [out, retval] */ IMaterial ** pVal )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			if ( pVal )
			{
				hr = CMaterial::CreateInstance( pVal );

				if ( hr == S_OK )
				{
					static_cast< CMaterial * >( *pVal )->SetInternal( m_internal->GetMaterial( static_cast< CSubmesh * >( submesh )->GetInternal() ) );
				}
			}
		}
		else
		{
			hr = CComError::DispatchError(
					 E_FAIL,						// This represents the error
					 IID_IGeometry,					// This is the GUID of component throwing error
					 cuT( "GetMaterial" ),			// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 NULL );
		}

		return hr;
	}

	STDMETHODIMP CGeometry::SetMaterial( /* [in] */ ISubmesh * submesh, /* [in] */ IMaterial * val )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->SetMaterial( static_cast< CSubmesh * >( submesh )->GetInternal(), static_cast< CMaterial * >( val )->GetInternal() );
			hr = S_OK;
		}
		else
		{
			hr = CComError::DispatchError(
					 E_FAIL,						// This represents the error
					 IID_IGeometry,					// This is the GUID of component throwing error
					 cuT( "SetMaterial" ),			// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 NULL );
		}

		return hr;
	}
}
