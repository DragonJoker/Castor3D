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
#ifndef ___C3D_PREREQUISITES_LIGHT_H___
#define ___C3D_PREREQUISITES_LIGHT_H___

namespace Castor3D
{
	/**@name Light */
	//@{

	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		Light types enumeration
	\~french
	\brief		Enumération des types de lumières
	*/
	typedef enum eLIGHT_TYPE
	CASTOR_TYPE( uint8_t )
	{
		eLIGHT_TYPE_DIRECTIONAL,	//!< Directional light type
		eLIGHT_TYPE_POINT,			//!< Point light type
		eLIGHT_TYPE_SPOT,			//!< Spot light type
		eLIGHT_TYPE_COUNT,
	}	eLIGHT_TYPE;

	class Light;
	class LightCategory;
	class DirectionalLight;
	class PointLight;
	class SpotLight;

	DECLARE_SMART_PTR( Light );
	DECLARE_SMART_PTR( LightCategory );
	DECLARE_SMART_PTR( DirectionalLight );
	DECLARE_SMART_PTR( PointLight );
	DECLARE_SMART_PTR( SpotLight );

	class LightFactory;
	DECLARE_SMART_PTR( LightFactory );

	//! Array of lights
	DECLARE_VECTOR( LightSPtr, LightPtr );
	//! Map of lights, sorted by name
	DECLARE_MAP( Castor::String, LightSPtr, LightPtrStr );
	//! Map of lights, sorted by index
	DECLARE_MAP( int, LightSPtr, LightPtrInt );

	//@}
}

#endif
