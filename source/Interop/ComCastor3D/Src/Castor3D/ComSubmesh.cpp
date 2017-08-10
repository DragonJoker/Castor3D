#include "ComSubmesh.hpp"
#include "ComVector3D.hpp"

namespace CastorCom
{
	static const castor::String ERROR_UNINITIALISED = cuT( "The submesh must be initialised" );

	CSubmesh::CSubmesh()
	{
	}

	CSubmesh::~CSubmesh()
	{
	}

	STDMETHODIMP CSubmesh::addPoint( /* [in] */ IVector3D * val )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->addPoint( *static_cast< CVector3D * >( val ) );
			hr = S_OK;
		}
		else
		{
			hr = CComError::DispatchError(
					 E_FAIL,						// This represents the error
					 IID_ISubmesh,					// This is the GUID of PixelComponents throwing error
					 cuT( "addPoint" ),				// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 NULL );
		}

		return hr;
	}

	STDMETHODIMP CSubmesh::addFace( /* [in] */ unsigned int x, /* [in] */ unsigned int y, /* [in] */ unsigned int z )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->addFace( x, y, z );
			hr = S_OK;
		}
		else
		{
			hr = CComError::DispatchError(
					 E_FAIL,						// This represents the error
					 IID_ISubmesh,					// This is the GUID of PixelComponents throwing error
					 cuT( "addFace" ),				// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 NULL );
		}

		return hr;
	}
}
