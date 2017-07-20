namespace Castor3D
{
	template< UniformType Type >
	inline std::shared_ptr< TUniform< Type > > UniformBuffer::GetUniform( Castor::String const & p_name )const
	{
		std::shared_ptr< TUniform< Type > > result;
		auto it = m_mapVariables.find( p_name );

		if ( it != m_mapVariables.end() )
		{
			auto variable = it->second.lock();
			REQUIRE( variable->GetFullType() == Type );
			result = std::static_pointer_cast< TUniform< Type > >( variable );
		}

		return result;
	}
}
