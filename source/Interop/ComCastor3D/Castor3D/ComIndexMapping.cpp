#include "ComCastor3D/Castor3D/ComIndexMapping.hpp"

namespace CastorCom
{
	CIndexMapping::CIndexMapping()
	{
	}

	CIndexMapping::~CIndexMapping()
	{
	}

	STDMETHODIMP CIndexMapping::get_Type( /* [retval][out] */ eINDEX_MAPPING_TYPE * value )
	{
		HRESULT hr = E_POINTER;

		if ( m_internal )
		{
			if ( value )
			{
				if ( m_internal->getType() == castor3d::TriFaceMapping::Name )
				{
					*value = eINDEX_MAPPING_TYPE_FACE;
					hr = S_OK;
				}
				else if ( m_internal->getType() == castor3d::TriFaceMapping::Name )
				{
					*value = eINDEX_MAPPING_TYPE_LINE;
					hr = S_OK;
				}
				else
				{
					hr = CComError::dispatchError( E_FAIL
						, LIBID_Castor3D
						, _T( "Unsupported index mapping type" )
						, ERROR_UNSUPPORTED_C3D_TYPE.c_str()
						, 0
						, nullptr );
				}
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
}
