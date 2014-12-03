#include "ComSubmesh.hpp"
#include "ComVector3D.hpp"

namespace CastorCom
{
	static const Castor::String ERROR_UNINITIALISED = cuT( "The submesh must be initialised" );

	CSubmesh::CSubmesh()
	{
	}

	CSubmesh::~CSubmesh()
	{
	}

	STDMETHODIMP CSubmesh::AddPoint( /* [in] */ IVector3D * val )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->AddPoint( *static_cast< CVector3D * >( val ) );
			hr = S_OK;
		}
		else
		{
			hr = CComError::DispatchError(
					 E_FAIL,						// This represents the error
					 IID_ISubmesh,					// This is the GUID of component throwing error
					 cuT( "AddPoint" ),				// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 NULL );
		}

		return hr;
	}

	STDMETHODIMP CSubmesh::AddFace( /* [in] */ unsigned int x, /* [in] */ unsigned int y, /* [in] */ unsigned int z )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->AddFace( x, y, z );
			hr = S_OK;
		}
		else
		{
			hr = CComError::DispatchError(
					 E_FAIL,						// This represents the error
					 IID_ISubmesh,					// This is the GUID of component throwing error
					 cuT( "AddFace" ),				// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 NULL );
		}

		return hr;
	}
}
