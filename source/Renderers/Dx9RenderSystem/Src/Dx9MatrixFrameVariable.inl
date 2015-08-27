namespace Dx9Render
{
	template< typename T, std::size_t Rows, std::size_t Columns >
	DxMatrixFrameVariable< T, Rows, Columns >::DxMatrixFrameVariable( DxRenderSystem * p_pRenderSystem, DxShaderProgram * p_pProgram, uint32_t p_uiOcc )
		:	DxFrameVariableBase( p_pRenderSystem )
		,	Castor3D::MatrixFrameVariable< T, Rows, Columns >( p_pProgram, p_uiOcc )
	{
	}

	template< typename T, std::size_t Rows, std::size_t Columns >
	DxMatrixFrameVariable< T, Rows, Columns >::DxMatrixFrameVariable( DxRenderSystem * p_pRenderSystem, Castor3D::MatrixFrameVariable<T, Rows, Columns> * p_pVariable )
		:	DxFrameVariableBase( p_pRenderSystem )
		,	Castor3D::MatrixFrameVariable< T, Rows, Columns >( *p_pVariable )
	{
	}

	template< typename T, std::size_t Rows, std::size_t Columns >
	DxMatrixFrameVariable< T, Rows, Columns >::~DxMatrixFrameVariable()
	{
	}

	template< typename T, std::size_t Rows, std::size_t Columns >
	bool DxMatrixFrameVariable< T, Rows, Columns >::Initialise()
	{
		GetVariableLocation( Castor::str_utils::to_str( m_strName ).c_str(), static_cast< DxShaderProgram * >( Castor3D::FrameVariable::GetProgram() )->GetShaderEffect() );
		return m_d3dxHandle != NULL;
	}

	template< typename T, std::size_t Rows, std::size_t Columns >
	void DxMatrixFrameVariable< T, Rows, Columns >::Cleanup()
	{
		m_d3dxHandle = NULL;
	}

	template< typename T, std::size_t Rows, std::size_t Columns >
	void DxMatrixFrameVariable< T, Rows, Columns >::Bind()
	{
	}

	template< typename T, std::size_t Rows, std::size_t Columns >
	void DxMatrixFrameVariable< T, Rows, Columns >::Unbind()
	{
	}

	template< typename T, std::size_t Rows, std::size_t Columns >
	void DxMatrixFrameVariable< T, Rows, Columns >::Apply()
	{
		if ( Castor3D::FrameVariable::m_bChanged )
		{
			DxFrameVariableBase::DoApply< T, Rows, Columns >( *static_cast< DxShaderProgram * >( Castor3D::FrameVariable::GetProgram() ), Castor3D::FrameVariable::m_strName, Castor3D::MatrixFrameVariable< T, Rows, Columns >::m_pValues, Castor3D::FrameVariable::m_uiOcc );
			Castor3D::FrameVariable::m_bChanged = false;
		}
	}
}
