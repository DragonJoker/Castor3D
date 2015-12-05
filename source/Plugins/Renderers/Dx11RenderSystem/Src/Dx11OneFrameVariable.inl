namespace Dx11Render
{
	template< typename T >
	DxOneFrameVariable< T >::DxOneFrameVariable( DxRenderSystem * p_renderSystem, DxShaderProgram * p_program, uint32_t p_occurences )
		:	DxFrameVariableBase( p_renderSystem )
		,	Castor3D::OneFrameVariable< T >	( p_program, p_occurences )
	{
	}

	template< typename T >
	DxOneFrameVariable< T >::DxOneFrameVariable( DxRenderSystem * p_renderSystem, Castor3D::OneFrameVariable< T > * p_variable )
		:	DxFrameVariableBase( p_renderSystem )
		,	Castor3D::OneFrameVariable< T >( *p_variable )
	{
	}

	template< typename T >
	DxOneFrameVariable< T >::~DxOneFrameVariable()
	{
	}

	template< typename T >
	void DxOneFrameVariable< T >::Bind()
	{
		DxFrameVariableBase::DoBind< T >( *static_cast< DxShaderProgram * >( Castor3D::FrameVariable::GetProgram() ), Castor3D::OneFrameVariable< T >::m_values );
		Castor3D::FrameVariable::m_changed = false;
	}

	template< typename T >
	void DxOneFrameVariable< T >::Unbind()
	{
		DxFrameVariableBase::DoUnbind< T >( *static_cast< DxShaderProgram * >( Castor3D::FrameVariable::GetProgram() ), Castor3D::OneFrameVariable< T >::m_values );
	}
}
