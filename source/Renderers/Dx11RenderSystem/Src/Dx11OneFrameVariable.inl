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
	}

	template< typename T >
	void DxOneFrameVariable< T >::Unbind()
	{
	}

	template< typename T >
	void DxOneFrameVariable< T >::Apply()
	{
		if ( Castor3D::FrameVariable::m_bChanged )
		{
			DxFrameVariableBase::DoApply< T >( *static_cast< DxShaderProgram * >( Castor3D::FrameVariable::GetProgram() ), Castor3D::OneFrameVariable< T >::m_pValues );
			Castor3D::FrameVariable::m_bChanged = false;
		}
	}
}
