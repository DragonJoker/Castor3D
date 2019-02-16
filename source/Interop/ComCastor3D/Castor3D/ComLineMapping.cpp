#include "ComCastor3D/Castor3D/ComLineMapping.hpp"
#include "ComCastor3D/CastorUtils/ComVector3D.hpp"

namespace CastorCom
{
	static const castor::String ERROR_UNINITIALISED = cuT( "The mapping must be initialised" );

	CLinesMapping::CLinesMapping()
	{
	}

	CLinesMapping::~CLinesMapping()
	{
	}

	STDMETHODIMP CLinesMapping::get_Type( /* [retval][out] */ eINDEX_MAPPING_TYPE * value )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			if ( value )
			{
				*value = eINDEX_MAPPING_TYPE_LINE;
				hr = S_OK;
			}
		}
		else
		{
			hr = CComError::dispatchError( E_FAIL
				, LIBID_Castor3D
				, cuT( "Null instance" )
				, ERROR_UNINITIALISED_INSTANCE.c_str()
				, 0
				, nullptr );
		}

		return hr;
	}

	STDMETHODIMP CLinesMapping::AddLine( /* [in] */ unsigned int x, /* [in] */ unsigned int y )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->addLine( x, y );
			hr = S_OK;
		}
		else
		{
			hr = CComError::dispatchError(
					 E_FAIL,						// This represents the error
					 IID_ILinesMapping,				// This is the GUID of PixelComponents throwing error
					 cuT( "addFace" ),				// This is generally displayed as the title
					 ERROR_UNINITIALISED.c_str(),	// This is the description
					 0,								// This is the context in the help file
					 nullptr );
		}

		return hr;
	}
}
