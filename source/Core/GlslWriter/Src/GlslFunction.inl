namespace GLSL
{
	//***********************************************************************************************

	template< typename RetT, typename ... ParamsT >
	Function< RetT, ParamsT... >::Function( GlslWriter * p_writer, Castor::String const & p_name )
		: m_writer{ p_writer }
		, m_name{ p_name }
	{
	}

	template< typename RetT, typename ... ParamsT >
	RetT Function< RetT, ParamsT... >::operator()( ParamsT && ... p_params )
	{
		return WriteFunctionCall< RetT >( m_writer, m_name, std::forward< ParamsT >( p_params )... );
	}

	//***********************************************************************************************
}
