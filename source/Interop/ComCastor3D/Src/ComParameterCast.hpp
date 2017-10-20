/* See LICENSE file in root folder */
#ifndef ___C3DCOM_COM_PARAMETER_CAST_H___
#define ___C3DCOM_COM_PARAMETER_CAST_H___

#include "ComCastor3DPrerequisites.hpp"
#include "ComUtils.hpp"

#include <Graphics/ColourComponent.hpp>

namespace CastorCom
{
	template< typename Value, typename _Value >
	inline Value parameter_cast( _Value const & value )
	{
		return Value( value );
	}
	template<>
	inline bool parameter_cast< bool, boolean >( boolean const & value )
	{
		return value != 0;
	}
	template<>
	inline boolean parameter_cast< boolean, bool >( bool const & value )
	{
		return value ? 1 : 0;
	}
	template<>
	inline BSTR parameter_cast< BSTR, castor::String >( castor::String const & value )
	{
		return ToBstr( value );
	}
	template<>
	inline castor::String parameter_cast< castor::String, BSTR >( BSTR const & value )
	{
		return FromBstr( value );
	}
	template<>
	inline BSTR parameter_cast< BSTR, castor::Path >( castor::Path const & value )
	{
		return ToBstr( value );
	}
	template<>
	inline castor::Path parameter_cast< castor::Path, BSTR >( BSTR const & value )
	{
		return castor::Path{ FromBstr( value ) };
	}
	template<>
	inline FLOAT parameter_cast< FLOAT, castor::ColourComponent >( castor::ColourComponent const & value )
	{
		return ( FLOAT )value;
	}
	template<>
	inline IScene * parameter_cast< IScene *, castor3d::SceneSPtr >( castor3d::SceneSPtr const & value )
	{
		return reinterpret_cast< IScene *>( value.get() );
	}
}

#endif
