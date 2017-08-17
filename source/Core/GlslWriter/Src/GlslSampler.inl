namespace glsl
{
	//*****************************************************************************************

	SamplerBuffer::SamplerBuffer()
		: Type( cuT( "samplerBuffer " ) )
	{
	}

	SamplerBuffer::SamplerBuffer( GlslWriter * p_writer, castor::String const & p_name )
		: Type( cuT( "samplerBuffer " ), p_writer, p_name )
	{
	}

	template< typename T >
	SamplerBuffer & SamplerBuffer::operator=( T const & p_rhs )
	{
		updateWriter( p_rhs );
		m_writer->writeAssign( *this, p_rhs );
		return *this;
	}

	SamplerBuffer::operator uint32_t()
	{
		return 0u;
	}

	//*****************************************************************************************

	Sampler1D::Sampler1D()
		: Type( cuT( "sampler1D " ) )
	{
	}

	Sampler1D::Sampler1D( GlslWriter * p_writer, castor::String const & p_name )
		: Type( cuT( "sampler1D " ), p_writer, p_name )
	{
	}

	template< typename T >
	Sampler1D & Sampler1D::operator=( T const & p_rhs )
	{
		updateWriter( p_rhs );
		m_writer->writeAssign( *this, p_rhs );
		return *this;
	}

	Sampler1D::operator uint32_t()
	{
		return 0u;
	}

	//*****************************************************************************************

	Sampler2D::Sampler2D()
		: Type( cuT( "sampler2D " ) )
	{
	}

	Sampler2D::Sampler2D( GlslWriter * p_writer, castor::String const & p_name )
		: Type( cuT( "sampler2D " ), p_writer, p_name )
	{
	}

	template< typename T >
	Sampler2D & Sampler2D::operator=( T const & p_rhs )
	{
		updateWriter( p_rhs );
		m_writer->writeAssign( *this, p_rhs );
		return *this;
	}

	Sampler2D::operator uint32_t()
	{
		return 0u;
	}

	//*****************************************************************************************

	Sampler3D::Sampler3D()
		: Type( cuT( "sampler3D " ) )
	{
	}

	Sampler3D::Sampler3D( GlslWriter * p_writer, castor::String const & p_name )
		: Type( cuT( "sampler3D " ), p_writer, p_name )
	{
	}

	template< typename T >
	Sampler3D & Sampler3D::operator=( T const & p_rhs )
	{
		updateWriter( p_rhs );
		m_writer->writeAssign( *this, p_rhs );
		return *this;
	}

	Sampler3D::operator uint32_t()
	{
		return 0u;
	}

	//*****************************************************************************************

	SamplerCube::SamplerCube()
		: Type( cuT( "samplerCube " ) )
	{
	}

	SamplerCube::SamplerCube( GlslWriter * p_writer, castor::String const & p_name )
		: Type( cuT( "samplerCube " ), p_writer, p_name )
	{
	}

	template< typename T >
	SamplerCube & SamplerCube::operator=( T const & p_rhs )
	{
		updateWriter( p_rhs );
		m_writer->writeAssign( *this, p_rhs );
		return *this;
	}

	SamplerCube::operator uint32_t()
	{
		return 0u;
	}

	//*****************************************************************************************

	Sampler1DArray::Sampler1DArray()
		: Type( cuT( "sampler1DArray " ) )
	{
	}

	Sampler1DArray::Sampler1DArray( GlslWriter * p_writer, castor::String const & p_name )
		: Type( cuT( "sampler1DArray " ), p_writer, p_name )
	{
	}

	template< typename T >
	Sampler1DArray & Sampler1DArray::operator=( T const & p_rhs )
	{
		updateWriter( p_rhs );
		m_writer->writeAssign( *this, p_rhs );
		return *this;
	}

	Sampler1DArray::operator uint32_t()
	{
		return 0u;
	}

	//*****************************************************************************************

	Sampler2DArray::Sampler2DArray()
		: Type( cuT( "sampler2DArray " ) )
	{
	}

	Sampler2DArray::Sampler2DArray( GlslWriter * p_writer, castor::String const & p_name )
		: Type( cuT( "sampler2DArray " ), p_writer, p_name )
	{
	}

	template< typename T >
	Sampler2DArray & Sampler2DArray::operator=( T const & p_rhs )
	{
		updateWriter( p_rhs );
		m_writer->writeAssign( *this, p_rhs );
		return *this;
	}

	Sampler2DArray::operator uint32_t()
	{
		return 0u;
	}

	//*****************************************************************************************

	SamplerCubeArray::SamplerCubeArray()
		: Type( cuT( "samplerCubeArray " ) )
	{
	}

	SamplerCubeArray::SamplerCubeArray( GlslWriter * p_writer, castor::String const & p_name )
		: Type( cuT( "samplerCubeArray " ), p_writer, p_name )
	{
	}

	template< typename T >
	SamplerCubeArray & SamplerCubeArray::operator=( T const & p_rhs )
	{
		updateWriter( p_rhs );
		m_writer->writeAssign( *this, p_rhs );
		return *this;
	}

	SamplerCubeArray::operator uint32_t()
	{
		return 0u;
	}

	//*****************************************************************************************

	Sampler1DShadow::Sampler1DShadow()
		: Type( cuT( "sampler1DShadow " ) )
	{
	}

	Sampler1DShadow::Sampler1DShadow( GlslWriter * p_writer, castor::String const & p_name )
		: Type( cuT( "sampler1DShadow " ), p_writer, p_name )
	{
	}

	template< typename T >
	Sampler1DShadow & Sampler1DShadow::operator=( T const & p_rhs )
	{
		updateWriter( p_rhs );
		m_writer->writeAssign( *this, p_rhs );
		return *this;
	}

	Sampler1DShadow::operator uint32_t()
	{
		return 0u;
	}

	//*****************************************************************************************

	Sampler2DShadow::Sampler2DShadow()
		: Type( cuT( "sampler2DShadow " ) )
	{
	}

	Sampler2DShadow::Sampler2DShadow( GlslWriter * p_writer, castor::String const & p_name )
		: Type( cuT( "sampler2DShadow " ), p_writer, p_name )
	{
	}

	template< typename T >
	Sampler2DShadow & Sampler2DShadow::operator=( T const & p_rhs )
	{
		updateWriter( p_rhs );
		m_writer->writeAssign( *this, p_rhs );
		return *this;
	}

	Sampler2DShadow::operator uint32_t()
	{
		return 0u;
	}

	//*****************************************************************************************

	SamplerCubeShadow::SamplerCubeShadow()
		: Type( cuT( "samplerCubeShadow " ) )
	{
	}

	SamplerCubeShadow::SamplerCubeShadow( GlslWriter * p_writer, castor::String const & p_name )
		: Type( cuT( "samplerCubeShadow " ), p_writer, p_name )
	{
	}

	template< typename T >
	SamplerCubeShadow & SamplerCubeShadow::operator=( T const & p_rhs )
	{
		updateWriter( p_rhs );
		m_writer->writeAssign( *this, p_rhs );
		return *this;
	}

	SamplerCubeShadow::operator uint32_t()
	{
		return 0u;
	}

	//*****************************************************************************************

	Sampler1DArrayShadow::Sampler1DArrayShadow()
		: Type( cuT( "sampler1DArrayShadow " ) )
	{
	}

	Sampler1DArrayShadow::Sampler1DArrayShadow( GlslWriter * p_writer, castor::String const & p_name )
		: Type( cuT( "sampler1DArrayShadow " ), p_writer, p_name )
	{
	}

	template< typename T >
	Sampler1DArrayShadow & Sampler1DArrayShadow::operator=( T const & p_rhs )
	{
		updateWriter( p_rhs );
		m_writer->writeAssign( *this, p_rhs );
		return *this;
	}

	Sampler1DArrayShadow::operator uint32_t()
	{
		return 0u;
	}

	//*****************************************************************************************

	Sampler2DArrayShadow::Sampler2DArrayShadow()
		: Type( cuT( "sampler2DArrayShadow " ) )
	{
	}

	Sampler2DArrayShadow::Sampler2DArrayShadow( GlslWriter * p_writer, castor::String const & p_name )
		: Type( cuT( "sampler2DArrayShadow " ), p_writer, p_name )
	{
	}

	template< typename T >
	Sampler2DArrayShadow & Sampler2DArrayShadow::operator=( T const & p_rhs )
	{
		updateWriter( p_rhs );
		m_writer->writeAssign( *this, p_rhs );
		return *this;
	}

	Sampler2DArrayShadow::operator uint32_t()
	{
		return 0u;
	}

	//*****************************************************************************************

	SamplerCubeArrayShadow::SamplerCubeArrayShadow()
		: Type( cuT( "samplerCubeArrayShadow " ) )
	{
	}

	SamplerCubeArrayShadow::SamplerCubeArrayShadow( GlslWriter * p_writer, castor::String const & p_name )
		: Type( cuT( "samplerCubeArrayShadow " ), p_writer, p_name )
	{
	}

	template< typename T >
	SamplerCubeArrayShadow & SamplerCubeArrayShadow::operator=( T const & p_rhs )
	{
		updateWriter( p_rhs );
		m_writer->writeAssign( *this, p_rhs );
		return *this;
	}

	SamplerCubeArrayShadow::operator uint32_t()
	{
		return 0u;
	}

	//*****************************************************************************************
}
