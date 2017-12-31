namespace TestRender
{
	template< castor3d::UniformType Type >
	TestPushUniform< Type >::TestPushUniform( TestShaderProgram & p_program, uint32_t p_occurences )
		: castor3d::TPushUniform< Type >( p_program, p_occurences )
	{
	}

	template< castor3d::UniformType Type >
	TestPushUniform< Type >::~TestPushUniform()
	{
	}

	template< castor3d::UniformType Type >
	bool TestPushUniform< Type >::doInitialise()
	{
		return true;
	}

	template< castor3d::UniformType Type >
	void TestPushUniform< Type >::doUpdate()const
	{
	}
}
