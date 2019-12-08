#include "ComCastor3D/Castor3D/ComTriFaceMapping.hpp"

namespace CastorCom
{
	static const tstring ERROR_UNINITIALISED = _T( "The mapping must be initialised" );

	CTriFaceMapping::CTriFaceMapping()
	{
	}

	CTriFaceMapping::~CTriFaceMapping()
	{
	}

	STDMETHODIMP CTriFaceMapping::get_Type( /* [retval][out] */ eINDEX_MAPPING_TYPE * value )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			if ( value )
			{
				*value = eINDEX_MAPPING_TYPE_FACE;
				hr = S_OK;
			}
		}
		else
		{
			hr = CComError::dispatchError( E_FAIL
				, LIBID_Castor3D
				, _T( "Null instance" )
				, ERROR_UNINITIALISED_INSTANCE.c_str()
				, 0
				, nullptr );
		}

		return hr;
	}

	STDMETHODIMP CTriFaceMapping::AddFace( /* [in] */ unsigned int x, /* [in] */ unsigned int y, /* [in] */ unsigned int z )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			m_internal->addFace( x, y, z );
			hr = S_OK;
		}
		else
		{
			hr = CComError::dispatchError(
				E_FAIL,						// This represents the error
				IID_ITriFaceMapping,			// This is the GUID of PixelComponents throwing error
				_T( "addFace" ),				// This is generally displayed as the title
				ERROR_UNINITIALISED.c_str(),	// This is the description
				0,								// This is the context in the help file
				nullptr );
		}

		return hr;
	}
}
