namespace GLSL
{
	//*********************************************************************************************

	Light::Light()
		: Type( cuT( "Light " ) )
	{
	}

	Light::Light( GlslWriter * p_writer, Castor::String const & p_name )
		: Type( cuT( "Light " ), p_writer, p_name )
	{
	}

	Light & Light::operator=( Light const & p_rhs )
	{
		if ( m_writer )
		{
			m_writer->WriteAssign( *this, p_rhs );
		}
		else
		{
			Type::operator=( p_rhs );
			m_writer = p_rhs.m_writer;
		}

		return *this;
	}

	template< typename T >
	Light & Light::operator=( T const & p_rhs )
	{
		UpdateWriter( p_rhs );
		m_writer->WriteAssign( *this, p_rhs );
		return *this;
	}

	Vec3 Light::m_v3Colour()const
	{
		return Vec3( m_writer, m_name + cuT( ".m_v3Colour" ) );
	}

	Vec3 Light::m_v3Intensity()const
	{
		return Vec3( m_writer, Castor::String( *this ) + cuT( ".m_v3Intensity" ) );
	}

	//*********************************************************************************************

	DirectionalLight::DirectionalLight()
		: Type( cuT( "DirectionalLight " ) )
	{
	}

	DirectionalLight::DirectionalLight( GlslWriter * p_writer, Castor::String const & p_name )
		: Type( cuT( "DirectionalLight " ), p_writer, p_name )
	{
	}

	DirectionalLight & DirectionalLight::operator=( DirectionalLight const & p_rhs )
	{
		if ( m_writer )
		{
			m_writer->WriteAssign( *this, p_rhs );
		}
		else
		{
			Type::operator=( p_rhs );
			m_writer = p_rhs.m_writer;
		}

		return *this;
	}

	template< typename T >
	DirectionalLight & DirectionalLight::operator=( T const & p_rhs )
	{
		UpdateWriter( p_rhs );
		m_writer->WriteAssign( *this, p_rhs );
		return *this;
	}

	Light DirectionalLight::m_lightBase()const
	{
		return Light( m_writer, m_name + cuT( ".m_lightBase" ) );
	}

	Vec3 DirectionalLight::m_v3Direction()const
	{
		return Vec3( m_writer, Castor::String( *this ) + cuT( ".m_v3Direction" ) );
	}

	Mat4 DirectionalLight::m_mtxLightSpace()const
	{
		return Mat4( m_writer, Castor::String( *this ) + cuT( ".m_mtxLightSpace" ) );
	}

	//*********************************************************************************************

	PointLight::PointLight()
		: Type( cuT( "PointLight " ) )
	{
	}

	PointLight::PointLight( GlslWriter * p_writer, Castor::String const & p_name )
		: Type( cuT( "PointLight " ), p_writer, p_name )
	{
	}

	PointLight & PointLight::operator=( PointLight const & p_rhs )
	{
		if ( m_writer )
		{
			m_writer->WriteAssign( *this, p_rhs );
		}
		else
		{
			Type::operator=( p_rhs );
			m_writer = p_rhs.m_writer;
		}

		return *this;
	}

	template< typename T >
	PointLight & PointLight::operator=( T const & p_rhs )
	{
		UpdateWriter( p_rhs );
		m_writer->WriteAssign( *this, p_rhs );
		return *this;
	}

	Light PointLight::m_lightBase()const
	{
		return Light( m_writer, m_name + cuT( ".m_lightBase" ) );
	}

	Vec3 PointLight::m_v3Position()const
	{
		return Vec3( m_writer, Castor::String( *this ) + cuT( ".m_v3Position" ) );
	}

	Vec3 PointLight::m_v3Attenuation()const
	{
		return Vec3( m_writer, Castor::String( *this ) + cuT( ".m_v3Attenuation" ) );
	}

	//*********************************************************************************************

	SpotLight::SpotLight()
		: Type( cuT( "SpotLight " ) )
	{
	}

	SpotLight::SpotLight( GlslWriter * p_writer, Castor::String const & p_name )
		: Type( cuT( "SpotLight " ), p_writer, p_name )
	{
	}

	SpotLight & SpotLight::operator=( SpotLight const & p_rhs )
	{
		if ( m_writer )
		{
			m_writer->WriteAssign( *this, p_rhs );
		}
		else
		{
			Type::operator=( p_rhs );
			m_writer = p_rhs.m_writer;
		}

		return *this;
	}

	template< typename T >
	SpotLight & SpotLight::operator=( T const & p_rhs )
	{
		UpdateWriter( p_rhs );
		m_writer->WriteAssign( *this, p_rhs );
		return *this;
	}

	Light SpotLight::m_lightBase()const
	{
		return Light( m_writer, m_name + cuT( ".m_lightBase" ) );
	}

	Vec3 SpotLight::m_v3Position()const
	{
		return Vec3( m_writer, Castor::String( *this ) + cuT( ".m_v3Position" ) );
	}

	Vec3 SpotLight::m_v3Attenuation()const
	{
		return Vec3( m_writer, Castor::String( *this ) + cuT( ".m_v3Attenuation" ) );
	}

	Vec3 SpotLight::m_v3Direction()const
	{
		return Vec3( m_writer, Castor::String( *this ) + cuT( ".m_v3Direction" ) );
	}

	Float SpotLight::m_fExponent()const
	{
		return Float( m_writer, Castor::String( *this ) + cuT( ".m_fExponent" ) );
	}

	Float SpotLight::m_fCutOff()const
	{
		return Float( m_writer, Castor::String( *this ) + cuT( ".m_fCutOff" ) );
	}

	Mat4 SpotLight::m_mtxLightSpace()const
	{
		return Mat4( m_writer, Castor::String( *this ) + cuT( ".m_mtxLightSpace" ) );
	}

	//*********************************************************************************************
}
