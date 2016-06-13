namespace TestRender
{
	template< typename T >
	TestOneFrameVariable< T >::TestOneFrameVariable( uint32_t p_occurences, TestShaderProgram * p_program )
		: Castor3D::OneFrameVariable< T >( p_program, p_occurences )
	{
	}

	template< typename T >
	TestOneFrameVariable< T >::TestOneFrameVariable( Castor3D::OneFrameVariable< T > * p_variable )
		: Castor3D::OneFrameVariable< T >( *p_variable )
	{
	}

	template< typename T >
	TestOneFrameVariable< T >::~TestOneFrameVariable()
	{
	}

	template< typename T >
	bool TestOneFrameVariable< T >::Initialise()
	{
		return true;
	}

	template< typename T >
	void TestOneFrameVariable< T >::Cleanup()
	{
	}

	template< typename T >
	void TestOneFrameVariable< T >::Bind()
	{
		if ( Castor3D::FrameVariable::m_changed )
		{
			Castor3D::FrameVariable::m_changed = false;
		}
	}
}
