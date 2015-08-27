namespace Dx11Render
{
	template< typename T, uint32_t Count >
	DxPointFrameVariable< T, Count >::DxPointFrameVariable( DxRenderSystem * p_pRenderSystem, DxShaderProgram * p_pProgram, uint32_t p_uiOcc )
		:	DxFrameVariableBase( p_pRenderSystem )
		,	Castor3D::PointFrameVariable< T, Count >( p_pProgram, p_uiOcc )
	{
	}

	template< typename T, uint32_t Count >
	DxPointFrameVariable< T, Count >::DxPointFrameVariable( DxRenderSystem * p_pRenderSystem, Castor3D::PointFrameVariable<T, Count> * p_pVariable )
		:	DxFrameVariableBase( p_pRenderSystem )
		,	Castor3D::PointFrameVariable< T, Count >( *p_pVariable )
	{
	}

	template< typename T, uint32_t Count >
	DxPointFrameVariable< T, Count >::~DxPointFrameVariable()
	{
	}

	template< typename T, uint32_t Count >
	void DxPointFrameVariable< T, Count >::Bind()
	{
	}

	template< typename T, uint32_t Count >
	void DxPointFrameVariable< T, Count >::Unbind()
	{
	}

	template< typename T, uint32_t Count >
	void DxPointFrameVariable< T, Count >::Apply()
	{
		if ( Castor3D::FrameVariable::m_bChanged )
		{
			DxFrameVariableBase::DoApply< T, Count >( *static_cast< DxShaderProgram * >( Castor3D::FrameVariable::GetProgram() ), Castor3D::PointFrameVariable< T, Count >::m_pValues );
			Castor3D::FrameVariable::m_bChanged = false;
		}
	}
}
