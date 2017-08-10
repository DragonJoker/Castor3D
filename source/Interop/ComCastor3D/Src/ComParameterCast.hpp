/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
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
