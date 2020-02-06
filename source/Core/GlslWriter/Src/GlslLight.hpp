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
#ifndef ___GLSL_LIGHT_H___
#define ___GLSL_LIGHT_H___

#include "GlslMat.hpp"

namespace GLSL
{
	static constexpr int BaseLightComponentsCount = 2;
	static constexpr int MaxLightComponentsCount = 14;
	static constexpr float LightComponentsOffset = MaxLightComponentsCount * 0.001f;

	struct Light
		: public Type
	{
		inline Light();
		inline Light( GlslWriter * p_writer, Castor::String const & p_name = Castor::String() );
		inline Light & operator=( Light const & p_rhs );
		template< typename T > inline Light & operator=( T const & p_rhs );
		inline Vec3 m_v3Colour()const;
		inline Vec3 m_v3Intensity()const;
	};

	struct DirectionalLight
		: public Type
	{
		inline DirectionalLight();
		inline DirectionalLight( GlslWriter * p_writer, Castor::String const & p_name = Castor::String() );
		inline DirectionalLight & operator=( DirectionalLight const & p_rhs );
		template< typename T > inline DirectionalLight & operator=( T const & p_rhs );
		inline Light m_lightBase()const;
		inline Vec3 m_v3Direction()const;
		inline Mat4 m_mtxLightSpace()const;
	};

	struct PointLight
		: public Type
	{
		inline PointLight();
		inline PointLight( GlslWriter * p_writer, Castor::String const & p_name = Castor::String() );
		inline PointLight & operator=( PointLight const & p_rhs );
		template< typename T > inline PointLight & operator=( T const & p_rhs );
		inline Light m_lightBase()const;
		inline Vec3 m_v3Position()const;
		inline Vec3 m_v3Attenuation()const;
		inline Int m_iIndex()const;
	};

	struct SpotLight
		: public Type
	{
		inline SpotLight();
		inline SpotLight( GlslWriter * p_writer, Castor::String const & p_name = Castor::String() );
		inline SpotLight & operator=( SpotLight const & p_rhs );
		template< typename T > inline SpotLight & operator=( T const & p_rhs );
		inline Light m_lightBase()const;
		inline Vec3 m_v3Position()const;
		inline Vec3 m_v3Attenuation()const;
		inline Vec3 m_v3Direction()const;
		inline Float m_fExponent()const;
		inline Float m_fCutOff()const;
		inline Mat4 m_mtxLightSpace()const;
		inline Int m_iIndex()const;
	};
}

#include "GlslLight.inl"

#endif
