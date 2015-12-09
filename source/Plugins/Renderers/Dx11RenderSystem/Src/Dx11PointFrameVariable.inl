namespace Dx11Render
{
	template< typename T, uint32_t Count >
	DxPointFrameVariable< T, Count >::DxPointFrameVariable( DxRenderSystem * p_renderSystem, DxShaderProgram * p_program, uint32_t p_occurences )
		:	DxFrameVariableBase( p_renderSystem )
		,	Castor3D::PointFrameVariable< T, Count >( p_program, p_occurences )
	{
	}

	template< typename T, uint32_t Count >
	DxPointFrameVariable< T, Count >::DxPointFrameVariable( DxRenderSystem * p_renderSystem, Castor3D::PointFrameVariable<T, Count> * p_variable )
		:	DxFrameVariableBase( p_renderSystem )
		,	Castor3D::PointFrameVariable< T, Count >( *p_variable )
	{
	}

	template< typename T, uint32_t Count >
	DxPointFrameVariable< T, Count >::~DxPointFrameVariable()
	{
	}

	template< typename T, uint32_t Count >
	void DxPointFrameVariable< T, Count >::Bind()
	{
		DxFrameVariableBase::DoBind< T, Count >( *static_cast< DxShaderProgram * >( Castor3D::FrameVariable::GetProgram() ), Castor3D::PointFrameVariable< T, Count >::m_values );
		Castor3D::FrameVariable::m_changed = false;
	}

	template< typename T, uint32_t Count >
	void DxPointFrameVariable< T, Count >::Unbind()
	{
		DxFrameVariableBase::DoUnbind< T, Count >( *static_cast< DxShaderProgram * >( Castor3D::FrameVariable::GetProgram() ), Castor3D::PointFrameVariable< T, Count >::m_values );
	}
}
