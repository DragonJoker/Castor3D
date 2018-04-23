/*
See LICENSE file in root folder
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
			C3D_API glsl::Vec4 m_colourIndex()const;
			C3D_API glsl::Vec4 m_intensityFarPlane()const;
			C3D_API glsl::Vec3 m_colour()const;
			C3D_API glsl::Vec2 m_intensity()const;
			C3D_API glsl::Float m_farPlane()const;
			C3D_API glsl::Int m_index()const;

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
			C3D_API glsl::Vec3 m_direction()const;
			C3D_API glsl::Vec4 m_exponentCutOff()const;
			C3D_API glsl::Float m_exponent()const;
			C3D_API glsl::Float m_cutOff()const;
			C3D_API glsl::Mat4 m_transform()const;

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
