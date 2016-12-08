namespace TestRender
{
	template< typename T, uint32_t Count >
	TestPointFrameVariable< T, Count >::TestPointFrameVariable( uint32_t p_occurences, TestShaderProgram & p_program )
		: Castor3D::PointFrameVariable< T, Count >( p_program, p_occurences )
	{
	}

	template< typename T, uint32_t Count >
	TestPointFrameVariable< T, Count >::TestPointFrameVariable( Castor3D::PointFrameVariable< T, Count > & p_variable )
		: Castor3D::PointFrameVariable< T, Count >( p_variable.GetProgram() )
	{
	}

	template< typename T, uint32_t Count >
	TestPointFrameVariable< T, Count >::~TestPointFrameVariable()
	{
	}

	template< typename T, uint32_t Count >
	bool TestPointFrameVariable< T, Count >::Initialise()
	{
		return true;
	}

	template< typename T, uint32_t Count >
	void TestPointFrameVariable< T, Count >::Cleanup()
	{
	}

	template< typename T, uint32_t Count >
	void TestPointFrameVariable< T, Count >::Bind()const
	{
		if ( Castor3D::FrameVariable::m_changed )
		{
			Castor3D::FrameVariable::m_changed = false;
		}
	}
}
