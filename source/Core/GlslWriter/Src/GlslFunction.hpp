/*
See LICENSE file in root folder
*/
#ifndef ___GLSL_Function_H___
#define ___GLSL_Function_H___

#include "GlslOptionalArray.hpp"

namespace glsl
{
	template< typename RetT, typename ... ParamsT >
	inline RetT writeFunctionCall( GlslWriter * p_writer, castor::String const & p_name, ParamsT const & ... p_params );

	template< typename Return, typename ... Params >
	inline void writeFunctionHeader( GlslWriter & p_writer, castor::String const & p_name, Params && ... p_params );

	template< typename RetT, typename ... ParamsT >
	struct Function
	{
	public:
		Function() = default;
		Function( GlslWriter * p_writer, castor::String const & p_name );
		RetT operator()( ParamsT && ... p_params )const;

	private:
		GlslWriter * m_writer{ nullptr };
		castor::String m_name;
	};
}

#include "GlslFunction.inl"

#endif
