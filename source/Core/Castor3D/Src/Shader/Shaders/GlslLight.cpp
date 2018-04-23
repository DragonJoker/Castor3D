#include "GlslLight.hpp"

using namespace castor;
using namespace glsl;

namespace castor3d
{
	namespace shader
	{
		//*********************************************************************************************

		Light::Light()
			: Type( cuT( "Light " ) )
		{
		}

		Light::Light( GlslWriter * writer, String const & name )
			: Type( cuT( "Light " ), writer, name )
		{
		}

		Light & Light::operator=( Light const & rhs )
		{
			if ( m_writer )
			{
				m_writer->writeAssign( *this, rhs );
			}
			else
			{
				Type::operator=( rhs );
				m_writer = rhs.m_writer;
			}

			return *this;
		}

		Vec3 Light::m_colour()const
		{
			return Vec3( m_writer, m_name + cuT( ".m_colourIndex.xyz" ) );
		}

		Vec2 Light::m_intensity()const
		{
			return Vec2( m_writer, String( *this ) + cuT( ".m_intensityFarPlane.xy" ) );
		}

		Float Light::m_farPlane()const
		{
			return Float( m_writer, String( *this ) + cuT( ".m_intensityFarPlane.z" ) );
		}

		Int Light::m_index()const
		{
			return Int( m_writer, cuT( "int( " ) + String( *this ) + cuT( ".m_colourIndex.w )" ) );
		}

		//*********************************************************************************************

		DirectionalLight::DirectionalLight()
			: Type( cuT( "DirectionalLight " ) )
		{
		}

		DirectionalLight::DirectionalLight( GlslWriter * writer, String const & name )
			: Type( cuT( "DirectionalLight " ), writer, name )
		{
		}

		DirectionalLight & DirectionalLight::operator=( DirectionalLight const & rhs )
		{
			if ( m_writer )
			{
				m_writer->writeAssign( *this, rhs );
			}
			else
			{
				Type::operator=( rhs );
				m_writer = rhs.m_writer;
			}

			return *this;
		}

		Light DirectionalLight::m_lightBase()const
		{
			return Light( m_writer, m_name + cuT( ".m_lightBase" ) );
		}

		Vec3 DirectionalLight::m_direction()const
		{
			return Vec3( m_writer, String( *this ) + cuT( ".m_direction.xyz" ) );
		}

		Mat4 DirectionalLight::m_transform()const
		{
			return Mat4( m_writer, String( *this ) + cuT( ".m_transform" ) );
		}

		//*********************************************************************************************

		PointLight::PointLight()
			: Type( cuT( "PointLight " ) )
		{
		}

		PointLight::PointLight( GlslWriter * writer, String const & name )
			: Type( cuT( "PointLight " ), writer, name )
		{
		}

		PointLight & PointLight::operator=( PointLight const & rhs )
		{
			if ( m_writer )
			{
				m_writer->writeAssign( *this, rhs );
			}
			else
			{
				Type::operator=( rhs );
				m_writer = rhs.m_writer;
			}

			return *this;
		}

		Light PointLight::m_lightBase()const
		{
			return Light( m_writer, m_name + cuT( ".m_lightBase" ) );
		}

		Vec3 PointLight::m_position()const
		{
			return Vec3( m_writer, String( *this ) + cuT( ".m_position" ) );
		}

		Vec3 PointLight::m_attenuation()const
		{
			return Vec3( m_writer, String( *this ) + cuT( ".m_attenuation.xyz" ) );
		}

		//*********************************************************************************************

		SpotLight::SpotLight()
			: Type( cuT( "SpotLight " ) )
		{
		}

		SpotLight::SpotLight( GlslWriter * writer, String const & name )
			: Type( cuT( "SpotLight " ), writer, name )
		{
		}

		SpotLight & SpotLight::operator=( SpotLight const & rhs )
		{
			if ( m_writer )
			{
				m_writer->writeAssign( *this, rhs );
			}
			else
			{
				Type::operator=( rhs );
				m_writer = rhs.m_writer;
			}

			return *this;
		}

		Light SpotLight::m_lightBase()const
		{
			return Light( m_writer, m_name + cuT( ".m_lightBase" ) );
		}

		Vec3 SpotLight::m_position()const
		{
			return Vec3( m_writer, String( *this ) + cuT( ".m_position.xyz" ) );
		}

		Vec3 SpotLight::m_direction()const
		{
			return Vec3( m_writer, String( *this ) + cuT( ".m_direction.xyz" ) );
		}

		Vec3 SpotLight::m_attenuation()const
		{
			return Vec3( m_writer, String( *this ) + cuT( ".m_attenuation.xyz" ) );
		}

		Float SpotLight::m_exponent()const
		{
			return Float( m_writer, String( *this ) + cuT( ".m_exponentCutOff.x" ) );
		}

		Float SpotLight::m_cutOff()const
		{
			return Float( m_writer, String( *this ) + cuT( ".m_exponentCutOff.y" ) );
		}

		Mat4 SpotLight::m_transform()const
		{
			return Mat4( m_writer, String( *this ) + cuT( ".m_transform" ) );
		}

		//*********************************************************************************************
	}
}
