namespace TestRender
{
	template< typename T >
	TestOneUniform< T >::TestOneUniform( uint32_t p_occurences, TestShaderProgram & p_program )
		: Castor3D::OneUniform< T >( p_program, p_occurences )
	{
	}

	template< typename T >
	TestOneUniform< T >::TestOneUniform( Castor3D::OneUniform< T > & p_variable )
		: Castor3D::OneUniform< T >( p_variable.GetProgram() )
	{
	}

	template< typename T >
	TestOneUniform< T >::~TestOneUniform()
	{
	}

	template< typename T >
	bool TestOneUniform< T >::Initialise()
	{
		return true;
	}

	template< typename T >
	void TestOneUniform< T >::Cleanup()
	{
	}

	template< typename T >
	void TestOneUniform< T >::Bind()const
	{
		if ( Castor3D::Uniform::m_changed )
		{
			Castor3D::Uniform::m_changed = false;
		}
	}
}
