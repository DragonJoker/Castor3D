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

#include "Castor3DPrerequisites.hpp"

#include <GlslMat.hpp>

namespace castor3d
{
	namespace shader
	{
		struct Light
			: public glsl::Type
		{
			C3D_API Light();
			C3D_API Light( glsl::GlslWriter * writer
				, castor::String const & name = castor::String() );
			C3D_API Light & operator=( Light const & rhs );
			C3D_API glsl::Vec3 m_colour()const;
			C3D_API glsl::Vec2 m_intensity()const;

			template< typename T >
			inline Light & operator=( T const & rhs )
			{
				updateWriter( rhs );
				m_writer->writeAssign( *this, rhs );
				return *this;
			}
		};

		struct DirectionalLight
			: public glsl::Type
		{
			C3D_API DirectionalLight();
			C3D_API DirectionalLight( glsl::GlslWriter * writer
				, castor::String const & name = castor::String() );
			C3D_API DirectionalLight & operator=( DirectionalLight const & rhs );
			C3D_API Light m_lightBase()const;
			C3D_API glsl::Vec3 m_direction()const;
			C3D_API glsl::Mat4 m_transform()const;

			template< typename T >
			inline DirectionalLight & operator=( T const & rhs )
			{
				updateWriter( rhs );
				m_writer->writeAssign( *this, rhs );
				return *this;
			}
		};

		struct PointLight
			: public glsl::Type
		{
			C3D_API PointLight();
			C3D_API PointLight( glsl::GlslWriter * writer
				, castor::String const & name = castor::String() );
			C3D_API PointLight & operator=( PointLight const & rhs );
			C3D_API Light m_lightBase()const;
			C3D_API glsl::Vec3 m_position()const;
			C3D_API glsl::Vec3 m_attenuation()const;
			C3D_API glsl::Float m_farPlane()const;
			C3D_API glsl::Int m_index()const;

			template< typename T >
			inline PointLight & operator=( T const & rhs )
			{
				updateWriter( rhs );
				m_writer->writeAssign( *this, rhs );
				return *this;
			}
		};

		struct SpotLight
			: public glsl::Type
		{
			C3D_API SpotLight();
			C3D_API SpotLight( glsl::GlslWriter * writer
				, castor::String const & name = castor::String() );
			C3D_API SpotLight & operator=( SpotLight const & rhs );
			C3D_API Light m_lightBase()const;
			C3D_API glsl::Vec3 m_position()const;
			C3D_API glsl::Vec3 m_attenuation()const;
			C3D_API glsl::Float m_farPlane()const;
			C3D_API glsl::Vec3 m_direction()const;
			C3D_API glsl::Float m_exponent()const;
			C3D_API glsl::Float m_cutOff()const;
			C3D_API glsl::Mat4 m_transform()const;
			C3D_API glsl::Int m_index()const;

			template< typename T >
			inline SpotLight & operator=( T const & rhs )
			{
				updateWriter( rhs );
				m_writer->writeAssign( *this, rhs );
				return *this;
			}
		};
	}
}

#endif
