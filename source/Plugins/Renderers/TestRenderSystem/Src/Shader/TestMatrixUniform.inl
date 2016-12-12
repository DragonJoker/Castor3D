namespace TestRender
{
	template< typename T, uint32_t Rows, uint32_t Columns >
	TestMatrixUniform< T, Rows, Columns >::TestMatrixUniform( uint32_t p_occurences, TestShaderProgram & p_program )
		: Castor3D::MatrixUniform< T, Rows, Columns >( p_program, p_occurences )
	{
	}

	template< typename T, uint32_t Rows, uint32_t Columns >
	TestMatrixUniform< T, Rows, Columns >::TestMatrixUniform( Castor3D::MatrixUniform< T, Rows, Columns > & p_variable )
		: Castor3D::MatrixUniform< T, Rows, Columns >( p_variable.GetProgram() )
	{
	}

	template< typename T, uint32_t Rows, uint32_t Columns >
	TestMatrixUniform< T, Rows, Columns >::~TestMatrixUniform()
	{
	}

	template< typename T, uint32_t Rows, uint32_t Columns >
	bool TestMatrixUniform< T, Rows, Columns >::Initialise()
	{
		return true;
	}

	template< typename T, uint32_t Rows, uint32_t Columns >
	void TestMatrixUniform< T, Rows, Columns >::Cleanup()
	{
	}

	template< typename T, uint32_t Rows, uint32_t Columns >
	void TestMatrixUniform< T, Rows, Columns >::Bind()const
	{
		if ( Castor3D::Uniform::m_changed )
		{
			Castor3D::Uniform::m_changed = false;
		}
	}
}
