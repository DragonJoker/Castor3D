namespace Dx11Render
{
	template< typename T, uint32_t Rows, uint32_t Columns >
	DxMatrixFrameVariable< T, Rows, Columns >::DxMatrixFrameVariable( DxRenderSystem * p_pRenderSystem, DxShaderProgram * p_pProgram, uint32_t p_uiOcc )
		:	DxFrameVariableBase( p_pRenderSystem )
		,	Castor3D::MatrixFrameVariable< T, Rows, Columns >( p_pProgram, p_uiOcc )
	{
	}

	template< typename T, uint32_t Rows, uint32_t Columns >
	DxMatrixFrameVariable< T, Rows, Columns >::DxMatrixFrameVariable( DxRenderSystem * p_pRenderSystem, Castor3D::MatrixFrameVariable<T, Rows, Columns> * p_pVariable )
		:	DxFrameVariableBase( p_pRenderSystem )
		,	Castor3D::MatrixFrameVariable< T, Rows, Columns >( *p_pVariable )
	{
	}

	template< typename T, uint32_t Rows, uint32_t Columns >
	DxMatrixFrameVariable< T, Rows, Columns >::~DxMatrixFrameVariable()
	{
	}

	template< typename T, uint32_t Rows, uint32_t Columns >
	void DxMatrixFrameVariable< T, Rows, Columns >::Bind()
	{
	}

	template< typename T, uint32_t Rows, uint32_t Columns >
	void DxMatrixFrameVariable< T, Rows, Columns >::Unbind()
	{
	}

	template< typename T, uint32_t Rows, uint32_t Columns >
	void DxMatrixFrameVariable< T, Rows, Columns >::Apply()
	{
		if ( Castor3D::FrameVariable::m_bChanged )
		{
			DxFrameVariableBase::DoApply< T, Rows, Columns >( *static_cast< DxShaderProgram * >( Castor3D::FrameVariable::GetProgram() ), Castor3D::MatrixFrameVariable< T, Rows, Columns >::m_pValues );
			Castor3D::FrameVariable::m_bChanged = false;
		}
	}
}
