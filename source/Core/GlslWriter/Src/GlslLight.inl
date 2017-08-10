namespace GLSL
{
	//*********************************************************************************************

	Light::Light()
		: Type( cuT( "Light " ) )
	{
	}

	Light::Light( GlslWriter * p_writer, castor::String const & p_name )
		: Type( cuT( "Light " ), p_writer, p_name )
	{
	}

	Light & Light::operator=( Light const & p_rhs )
	{
		if ( m_writer )
		{
			m_writer->writeAssign( *this, p_rhs );
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
		updateWriter( p_rhs );
		m_writer->writeAssign( *this, p_rhs );
		return *this;
	}

	Vec3 Light::m_colour()const
	{
		return Vec3( m_writer, m_name + cuT( ".m_colour" ) );
	}

	Vec2 Light::m_intensity()const
	{
		return Vec2( m_writer, castor::String( *this ) + cuT( ".m_intensity" ) );
	}

	//*********************************************************************************************

	DirectionalLight::DirectionalLight()
		: Type( cuT( "DirectionalLight " ) )
	{
	}

	DirectionalLight::DirectionalLight( GlslWriter * p_writer, castor::String const & p_name )
		: Type( cuT( "DirectionalLight " ), p_writer, p_name )
	{
	}

	DirectionalLight & DirectionalLight::operator=( DirectionalLight const & p_rhs )
	{
		if ( m_writer )
		{
			m_writer->writeAssign( *this, p_rhs );
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
		updateWriter( p_rhs );
		m_writer->writeAssign( *this, p_rhs );
		return *this;
	}

	Light DirectionalLight::m_lightBase()const
	{
		return Light( m_writer, m_name + cuT( ".m_lightBase" ) );
	}

	Vec3 DirectionalLight::m_direction()const
	{
		return Vec3( m_writer, castor::String( *this ) + cuT( ".m_direction" ) );
	}

	Mat4 DirectionalLight::m_transform()const
	{
		return Mat4( m_writer, castor::String( *this ) + cuT( ".m_transform" ) );
	}

	//*********************************************************************************************

	PointLight::PointLight()
		: Type( cuT( "PointLight " ) )
	{
	}

	PointLight::PointLight( GlslWriter * p_writer, castor::String const & p_name )
		: Type( cuT( "PointLight " ), p_writer, p_name )
	{
	}

	PointLight & PointLight::operator=( PointLight const & p_rhs )
	{
		if ( m_writer )
		{
			m_writer->writeAssign( *this, p_rhs );
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
		updateWriter( p_rhs );
		m_writer->writeAssign( *this, p_rhs );
		return *this;
	}

	Light PointLight::m_lightBase()const
	{
		return Light( m_writer, m_name + cuT( ".m_lightBase" ) );
	}

	Vec3 PointLight::m_position()const
	{
		return Vec3( m_writer, castor::String( *this ) + cuT( ".m_position" ) );
	}

	Vec3 PointLight::m_attenuation()const
	{
		return Vec3( m_writer, castor::String( *this ) + cuT( ".m_attenuation" ) );
	}

	Int PointLight::m_index()const
	{
		return Int( m_writer, castor::String( *this ) + cuT( ".m_index" ) );
	}

	//*********************************************************************************************

	SpotLight::SpotLight()
		: Type( cuT( "SpotLight " ) )
	{
	}

	SpotLight::SpotLight( GlslWriter * p_writer, castor::String const & p_name )
		: Type( cuT( "SpotLight " ), p_writer, p_name )
	{
	}

	SpotLight & SpotLight::operator=( SpotLight const & p_rhs )
	{
		if ( m_writer )
		{
			m_writer->writeAssign( *this, p_rhs );
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
		updateWriter( p_rhs );
		m_writer->writeAssign( *this, p_rhs );
		return *this;
	}

	Light SpotLight::m_lightBase()const
	{
		return Light( m_writer, m_name + cuT( ".m_lightBase" ) );
	}

	Vec3 SpotLight::m_position()const
	{
		return Vec3( m_writer, castor::String( *this ) + cuT( ".m_position" ) );
	}

	Vec3 SpotLight::m_attenuation()const
	{
		return Vec3( m_writer, castor::String( *this ) + cuT( ".m_attenuation" ) );
	}

	Vec3 SpotLight::m_direction()const
	{
		return Vec3( m_writer, castor::String( *this ) + cuT( ".m_direction" ) );
	}

	Float SpotLight::m_exponent()const
	{
		return Float( m_writer, castor::String( *this ) + cuT( ".m_exponent" ) );
	}

	Float SpotLight::m_cutOff()const
	{
		return Float( m_writer, castor::String( *this ) + cuT( ".m_cutOff" ) );
	}

	Mat4 SpotLight::m_transform()const
	{
		return Mat4( m_writer, castor::String( *this ) + cuT( ".m_transform" ) );
	}

	Int SpotLight::m_index()const
	{
		return Int( m_writer, castor::String( *this ) + cuT( ".m_index" ) );
	}

	//*********************************************************************************************
}
