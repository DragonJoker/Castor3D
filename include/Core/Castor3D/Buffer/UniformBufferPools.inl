namespace castor3d
{
	template< typename DataT >
	UniformBufferOffsetT< DataT > UniformBufferPools::getBuffer( VkMemoryPropertyFlags flags )
	{
		return m_pools[size_t( UniformBufferPools::getPoolType< DataT >() )].getBuffer< DataT >( flags );
	}

	template< typename DataT >
	void UniformBufferPools::putBuffer( UniformBufferOffsetT< DataT > const & bufferOffset )
	{
		return m_pools[size_t( UniformBufferPools::getPoolType< DataT >() )].putBuffer( bufferOffset );
	}

	template< typename DataT >
	UniformBufferPools::PoolType UniformBufferPools::getPoolType()
	{
		if constexpr ( std::is_same_v< DataT, MatrixUboConfiguration > )
		{
			return PoolType::eMatrix;
		}
		else if constexpr ( std::is_same_v< DataT, ModelInstancesUboConfiguration > )
		{
			return PoolType::eModelInstances;
		}
		else
		{
			return PoolType::eGeneric;
		}
	}
}
