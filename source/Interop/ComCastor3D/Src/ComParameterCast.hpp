/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___C3DCOM_COM_PARAMETER_CAST_H___
#define ___C3DCOM_COM_PARAMETER_CAST_H___

#include "ComCastor3DPrerequisites.hpp"
#include "ComUtils.hpp"

#include <ColourComponent.hpp>

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
	inline BSTR parameter_cast< BSTR, Castor::String >( Castor::String const & value )
	{
		return ToBstr( value );
	}
	template<>
	inline Castor::String parameter_cast< Castor::String, BSTR >( BSTR const & value )
	{
		return FromBstr( value );
	}
	template<>
	inline BSTR parameter_cast< BSTR, Castor::Path >( Castor::Path const & value )
	{
		return ToBstr( value );
	}
	template<>
	inline Castor::Path parameter_cast< Castor::Path, BSTR >( BSTR const & value )
	{
		return FromBstr( value );
	}
	template<>
	inline FLOAT parameter_cast< FLOAT, Castor::ColourComponent >( Castor::ColourComponent const & value )
	{
		return ( FLOAT )value;
	}
	template<>
	inline IScene * parameter_cast< IScene *, Castor3D::SceneSPtr >( Castor3D::SceneSPtr const & value )
	{
		return reinterpret_cast< IScene *>( value.get() );
	}
}

#endif
