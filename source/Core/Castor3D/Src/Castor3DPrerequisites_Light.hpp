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
		: uint8_t
	{
		//!\~english Directional light type	\~french Lumière directionnelle
		eLIGHT_TYPE_DIRECTIONAL,
		//!\~english Point light type	\~french Lumière ponctuelle
		eLIGHT_TYPE_POINT,
		//!\~english Spot light type	\~french Lumière projecteur
		eLIGHT_TYPE_SPOT,
		CASTOR_ENUM_BOUNDS( eLIGHT_TYPE, eLIGHT_TYPE_DIRECTIONAL )
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
