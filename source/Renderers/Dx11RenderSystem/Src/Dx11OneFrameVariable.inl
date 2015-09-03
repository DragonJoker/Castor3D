namespace Dx11Render
{
	template< typename T >
	DxOneFrameVariable< T >::DxOneFrameVariable( DxRenderSystem * p_pRenderSystem, DxShaderProgram * p_pProgram, uint32_t p_uiOcc )
		:	DxFrameVariableBase( p_pRenderSystem )
		,	Castor3D::OneFrameVariable< T >	( p_pProgram, p_uiOcc )
	{
	}

	template< typename T >
	DxOneFrameVariable< T >::DxOneFrameVariable( DxRenderSystem * p_pRenderSystem, Castor3D::OneFrameVariable< T > * p_pVariable )
		:	DxFrameVariableBase( p_pRenderSystem )
		,	Castor3D::OneFrameVariable< T >( *p_pVariable )
	{
	}

	template< typename T >
	DxOneFrameVariable< T >::~DxOneFrameVariable()
	{
	}

	template< typename T >
	void DxOneFrameVariable< T >::Bind()
	{
		DxFrameVariableBase::DoBind< T >( *static_cast< DxShaderProgram * >( Castor3D::FrameVariable::GetProgram() ), Castor3D::OneFrameVariable< T >::m_pValues );
		Castor3D::FrameVariable::m_bChanged = false;
	}

	template< typename T >
	void DxOneFrameVariable< T >::Unbind()
	{
		DxFrameVariableBase::DoUnbind< T >( *static_cast< DxShaderProgram * >( Castor3D::FrameVariable::GetProgram() ), Castor3D::OneFrameVariable< T >::m_pValues );
	}
}
