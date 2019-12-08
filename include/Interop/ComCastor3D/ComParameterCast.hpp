/* See LICENSE file in root folder */
#ifndef ___C3DCOM_COM_PARAMETER_CAST_H___
#define ___C3DCOM_COM_PARAMETER_CAST_H___

#include "ComCastor3D/ComCastor3DPrerequisites.hpp"
#include "ComCastor3D/ComUtils.hpp"

#include <CastorUtils/Graphics/ColourComponent.hpp>

namespace CastorCom
{
	template< typename Value, typename _Value >
	inline Value parameterCast( _Value const & value )
	{
		return Value( value );
	}
	template<>
	inline bool parameterCast< bool, boolean >( boolean const & value )
	{
		return value != 0;
	}
	template<>
	inline boolean parameterCast< boolean, bool >( bool const & value )
	{
		return value ? 1 : 0;
	}
	template<>
	inline BSTR parameterCast< BSTR, castor::String >( castor::String const & value )
	{
		return toBstr( value );
	}
	template<>
	inline castor::String parameterCast< castor::String, BSTR >( BSTR const & value )
	{
		return fromBstr( value );
	}
	template<>
	inline BSTR parameterCast< BSTR, castor::Path >( castor::Path const & value )
	{
		return toBstr( value );
	}
	template<>
	inline castor::Path parameterCast< castor::Path, BSTR >( BSTR const & value )
	{
		return castor::Path{ fromBstr( value ) };
	}
	template<>
	inline FLOAT parameterCast< FLOAT, castor::ColourComponent >( castor::ColourComponent const & value )
	{
		return ( FLOAT )value;
	}
	template<>
	inline IScene * parameterCast< IScene *, castor3d::SceneSPtr >( castor3d::SceneSPtr const & value )
	{
		return reinterpret_cast< IScene *>( value.get() );
	}
	template<>
	inline IPixelBuffer * parameterCast< IPixelBuffer *, castor::PxBufferBaseSPtr >( castor::PxBufferBaseSPtr const & value )
	{
		return reinterpret_cast< IPixelBuffer *>( value.get() );
	}
}

#endif
