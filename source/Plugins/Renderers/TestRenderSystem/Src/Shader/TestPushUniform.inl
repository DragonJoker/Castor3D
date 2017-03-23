namespace TestRender
{
	template< Castor3D::UniformType Type >
	TestPushUniform< Type >::TestPushUniform( TestShaderProgram & p_program, uint32_t p_occurences )
		: Castor3D::TPushUniform< Type >( p_program, p_occurences )
	{
	}

	template< Castor3D::UniformType Type >
	TestPushUniform< Type >::~TestPushUniform()
	{
	}

	template< Castor3D::UniformType Type >
	bool TestPushUniform< Type >::DoInitialise()
	{
		return true;
	}

	template< Castor3D::UniformType Type >
	void TestPushUniform< Type >::DoUpdate()const
	{
	}
}
