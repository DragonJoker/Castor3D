namespace Dx11Render
{
	template< typename T, uint32_t Rows, uint32_t Columns >
	DxMatrixFrameVariable< T, Rows, Columns >::DxMatrixFrameVariable( DxRenderSystem * p_renderSystem, DxShaderProgram * p_program, uint32_t p_occurences )
		:	DxFrameVariableBase( p_renderSystem )
		,	Castor3D::MatrixFrameVariable< T, Rows, Columns >( p_program, p_occurences )
	{
	}

	template< typename T, uint32_t Rows, uint32_t Columns >
	DxMatrixFrameVariable< T, Rows, Columns >::DxMatrixFrameVariable( DxRenderSystem * p_renderSystem, Castor3D::MatrixFrameVariable<T, Rows, Columns> * p_variable )
		:	DxFrameVariableBase( p_renderSystem )
		,	Castor3D::MatrixFrameVariable< T, Rows, Columns >( *p_variable )
	{
	}

	template< typename T, uint32_t Rows, uint32_t Columns >
	DxMatrixFrameVariable< T, Rows, Columns >::~DxMatrixFrameVariable()
	{
	}

	template< typename T, uint32_t Rows, uint32_t Columns >
	void DxMatrixFrameVariable< T, Rows, Columns >::Bind()
	{
		DxFrameVariableBase::DoBind< T, Rows, Columns >( *static_cast< DxShaderProgram * >( Castor3D::FrameVariable::GetProgram() ), Castor3D::MatrixFrameVariable< T, Rows, Columns >::m_values );
		Castor3D::FrameVariable::m_changed = false;
	}

	template< typename T, uint32_t Rows, uint32_t Columns >
	void DxMatrixFrameVariable< T, Rows, Columns >::Unbind()
	{
		DxFrameVariableBase::DoUnbind< T, Rows, Columns >( *static_cast< DxShaderProgram * >( Castor3D::FrameVariable::GetProgram() ), Castor3D::MatrixFrameVariable< T, Rows, Columns >::m_values );
	}
}
