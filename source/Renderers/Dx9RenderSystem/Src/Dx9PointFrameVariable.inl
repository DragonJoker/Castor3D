namespace Dx9Render
{
	template< typename T, std::size_t Count >
	DxPointFrameVariable< T, Count >::DxPointFrameVariable( DxRenderSystem * p_pRenderSystem, DxShaderProgram * p_pProgram, uint32_t p_uiOcc )
		:	DxFrameVariableBase( p_pRenderSystem )
		,	Castor3D::PointFrameVariable< T, Count >( p_pProgram, p_uiOcc )
	{
	}

	template< typename T, std::size_t Count >
	DxPointFrameVariable< T, Count >::DxPointFrameVariable( DxRenderSystem * p_pRenderSystem, Castor3D::PointFrameVariable<T, Count> * p_pVariable )
		:	DxFrameVariableBase( p_pRenderSystem )
		,	Castor3D::PointFrameVariable< T, Count >( *p_pVariable )
	{
	}

	template< typename T, std::size_t Count >
	DxPointFrameVariable< T, Count >::~DxPointFrameVariable()
	{
	}

	template< typename T, std::size_t Count >
	bool DxPointFrameVariable< T, Count >::Initialise()
	{
		GetVariableLocation( Castor::str_utils::to_str( m_strName ).c_str(), static_cast< DxShaderProgram * >( Castor3D::FrameVariable::GetProgram() )->GetShaderEffect() );
		return m_d3dxHandle != NULL;
	}

	template< typename T, std::size_t Count >
	void DxPointFrameVariable< T, Count >::Cleanup()
	{
		m_d3dxHandle = NULL;
	}

	template< typename T, std::size_t Count >
	void DxPointFrameVariable< T, Count >::Bind()
	{
	}

	template< typename T, std::size_t Count >
	void DxPointFrameVariable< T, Count >::Unbind()
	{
	}

	template< typename T, std::size_t Count >
	void DxPointFrameVariable< T, Count >::Apply()
	{
		if ( Castor3D::FrameVariable::m_bChanged )
		{
			DxFrameVariableBase::DoApply<T, Count>( *static_cast< DxShaderProgram * >( Castor3D::FrameVariable::GetProgram() ), Castor3D::FrameVariable::m_strName, Castor3D::PointFrameVariable< T, Count >::m_pValues, Castor3D::FrameVariable::m_uiOcc );
			Castor3D::FrameVariable::m_bChanged = false;
		}
	}
}
