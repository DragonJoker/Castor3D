namespace castor3d
{
	template< typename DataT >
	UniformBufferOffsetT< DataT > UniformBufferPools::getBuffer( VkMemoryPropertyFlags flags )
	{
		if constexpr ( std::is_same_v< DataT, MatrixUboConfiguration > )
		{
			return m_matrixUboPool.getBuffer( flags );
		}
		else if constexpr ( std::is_same_v< DataT, HdrConfig > )
		{
			return m_hdrConfigUboPool.getBuffer( flags );
		}
		else if constexpr ( std::is_same_v< DataT, RsmUboConfiguration > )
		{
			return m_rsmConfigUboPool.getBuffer( flags );
		}
		else if constexpr ( std::is_same_v< DataT, ModelMatrixUboConfiguration > )
		{
			return m_modelMatrixUboPool.getBuffer( flags );
		}
		else if constexpr ( std::is_same_v< DataT, ModelUboConfiguration > )
		{
			return m_modelUboPool.getBuffer( flags );
		}
		else if constexpr ( std::is_same_v< DataT, BillboardUboConfiguration > )
		{
			return m_billboardUboPool.getBuffer( flags );
		}
		else if constexpr ( std::is_same_v< DataT, PickingUboConfiguration > )
		{
			return m_pickingUboPool.getBuffer( flags );
		}
		else if constexpr ( std::is_same_v< DataT, TexturesUboConfiguration > )
		{
			return m_texturesUboPool.getBuffer( flags );
		}
		else if constexpr ( std::is_same_v< DataT, ShadowMapUboConfiguration > )
		{
			return m_shadowMapUboPool.getBuffer( flags );
		}
		else if constexpr ( std::is_same_v< DataT, SkinningUboConfiguration > )
		{
			return m_skinningUboPool.getBuffer( flags );
		}
		else if constexpr ( std::is_same_v< DataT, MorphingUboConfiguration > )
		{
			return m_morphingUboPool.getBuffer( flags );
		}
		else
		{
			static_assert( false, "Unsupported object type" );
			CU_Exception( "Unsupported object type" );
		}
	}

	template< typename DataT >
	void UniformBufferPools::putBuffer( UniformBufferOffsetT< DataT > const & bufferOffset )
	{
		if constexpr ( std::is_same_v< DataT, MatrixUboConfiguration > )
		{
			return m_matrixUboPool.putBuffer( bufferOffset );
		}
		else if constexpr ( std::is_same_v< DataT, HdrConfig > )
		{
			return m_hdrConfigUboPool.putBuffer( bufferOffset );
		}
		else if constexpr ( std::is_same_v< DataT, RsmUboConfiguration > )
		{
			return m_rsmConfigUboPool.putBuffer( bufferOffset );
		}
		else if constexpr ( std::is_same_v< DataT, ModelMatrixUboConfiguration > )
		{
			return m_modelMatrixUboPool.putBuffer( bufferOffset );
		}
		else if constexpr ( std::is_same_v< DataT, ModelUboConfiguration > )
		{
			return m_modelUboPool.putBuffer( bufferOffset );
		}
		else if constexpr ( std::is_same_v< DataT, BillboardUboConfiguration > )
		{
			return m_billboardUboPool.putBuffer( bufferOffset );
		}
		else if constexpr ( std::is_same_v< DataT, PickingUboConfiguration > )
		{
			return m_pickingUboPool.putBuffer( bufferOffset );
		}
		else if constexpr ( std::is_same_v< DataT, TexturesUboConfiguration > )
		{
			return m_texturesUboPool.putBuffer( bufferOffset );
		}
		else if constexpr ( std::is_same_v< DataT, ShadowMapUboConfiguration > )
		{
			return m_shadowMapUboPool.putBuffer( bufferOffset );
		}
		else if constexpr ( std::is_same_v< DataT, SkinningUboConfiguration > )
		{
			return m_skinningUboPool.putBuffer( bufferOffset );
		}
		else if constexpr ( std::is_same_v< DataT, MorphingUboConfiguration > )
		{
			return m_morphingUboPool.putBuffer( bufferOffset );
		}
		else
		{
			static_assert( false, "Unsupported object type" );
			CU_Exception( "Unsupported object type" );
		}
	}
}
