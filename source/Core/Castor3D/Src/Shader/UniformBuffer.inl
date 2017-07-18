namespace Castor3D
{
	template< UniformType Type >
	inline std::shared_ptr< TUniform< Type > > UniformBuffer::GetUniform( Castor::String const & p_name )const
	{
		std::shared_ptr< TUniform< Type > > l_result;
		auto l_it = m_mapVariables.find( p_name );

		if ( l_it != m_mapVariables.end() )
		{
			auto l_variable = l_it->second.lock();
			REQUIRE( l_variable->GetFullType() == Type );
			l_result = std::static_pointer_cast< TUniform< Type > >( l_variable );
		}

		return l_result;
	}
}
