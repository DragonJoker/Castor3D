namespace TestRender
{
	template< typename T, uint32_t Count >
	TestPointUniform< T, Count >::TestPointUniform( uint32_t p_occurences, TestShaderProgram & p_program )
		: Castor3D::PointUniform< T, Count >( p_program, p_occurences )
	{
	}

	template< typename T, uint32_t Count >
	TestPointUniform< T, Count >::TestPointUniform( Castor3D::PointUniform< T, Count > & p_variable )
		: Castor3D::PointUniform< T, Count >( p_variable.GetProgram() )
	{
	}

	template< typename T, uint32_t Count >
	TestPointUniform< T, Count >::~TestPointUniform()
	{
	}

	template< typename T, uint32_t Count >
	bool TestPointUniform< T, Count >::Initialise()
	{
		return true;
	}

	template< typename T, uint32_t Count >
	void TestPointUniform< T, Count >::Cleanup()
	{
	}

	template< typename T, uint32_t Count >
	void TestPointUniform< T, Count >::Bind()const
	{
		if ( Castor3D::Uniform::m_changed )
		{
			Castor3D::Uniform::m_changed = false;
		}
	}
}
