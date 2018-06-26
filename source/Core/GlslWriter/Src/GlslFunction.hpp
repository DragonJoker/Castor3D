/*
See LICENSE file in root folder
*/
#ifndef ___GLSL_Function_H___
#define ___GLSL_Function_H___

#include "GlslOptionalArray.hpp"

namespace glsl
{
	//***********************************************************************************************

	template< typename T >
	struct ParamTranslater
	{
		using Type = T;
	};

	template< typename T >
	struct ParamTranslater< InParam< T > >
	{
		using Type = T const &;
	};

	template< typename T >
	struct ParamTranslater< OutParam< T > >
	{
		using Type = T;
	};

	template< typename T >
	struct ParamTranslater< InOutParam< T > >
	{
		using Type = T;
	};

	//***********************************************************************************************

	template< typename ... ValuesT >
	inline GlslWriter * findWriter( ValuesT const & ... values );

	template< typename RetT, typename ... ParamsT >
	inline RetT writeFunctionCall( GlslWriter * writer
		, castor::String const & name
		, ParamsT const & ... params );

	template< typename Return, typename ... Params >
	inline void writeFunctionHeader( GlslWriter & writer
		, castor::String const & name
		, Params && ... params );

	//***********************************************************************************************

	template< typename RetT, typename ... ParamsT >
	struct Function
	{
	public:
		Function() = default;
		Function( GlslWriter * writer, castor::String const & p_name );
		RetT operator()( ParamsT && ... p_params )const;

	private:
		GlslWriter * m_writer{ nullptr };
		castor::String m_name;
	};

	//***********************************************************************************************
}

#include "GlslFunction.inl"

#endif
