namespace TestRender
{
	template< typename T, uint32_t Rows, uint32_t Columns >
	TestMatrixFrameVariable< T, Rows, Columns >::TestMatrixFrameVariable( uint32_t p_occurences, TestShaderProgram & p_program )
		: Castor3D::MatrixFrameVariable< T, Rows, Columns >( p_program, p_occurences )
	{
	}

	template< typename T, uint32_t Rows, uint32_t Columns >
	TestMatrixFrameVariable< T, Rows, Columns >::TestMatrixFrameVariable( Castor3D::MatrixFrameVariable< T, Rows, Columns > & p_variable )
		: Castor3D::MatrixFrameVariable< T, Rows, Columns >( p_variable.GetProgram() )
	{
	}

	template< typename T, uint32_t Rows, uint32_t Columns >
	TestMatrixFrameVariable< T, Rows, Columns >::~TestMatrixFrameVariable()
	{
	}

	template< typename T, uint32_t Rows, uint32_t Columns >
	bool TestMatrixFrameVariable< T, Rows, Columns >::Initialise()
	{
		return true;
	}

	template< typename T, uint32_t Rows, uint32_t Columns >
	void TestMatrixFrameVariable< T, Rows, Columns >::Cleanup()
	{
	}

	template< typename T, uint32_t Rows, uint32_t Columns >
	void TestMatrixFrameVariable< T, Rows, Columns >::Bind()
	{
		if ( Castor3D::FrameVariable::m_changed )
		{
			Castor3D::FrameVariable::m_changed = false;
		}
	}
}
