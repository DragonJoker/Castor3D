namespace Castor3D
{
	template< UniformType Type >
	std::shared_ptr< TUniform< Type > > UniformBuffer::GetUniform( Castor::String const & p_name )const
	{
		std::shared_ptr< TUniform< Type > > l_return;
		auto l_it = m_mapVariables.find( p_name );

		if ( l_it != m_mapVariables.end() )
		{
			auto l_variable = l_it->second.lock();
			REQUIRE( l_variable->GetFullType() == Type );
			l_return = std::static_pointer_cast< TUniform< Type > >( l_variable );
		}

		return l_return;
	}
}
