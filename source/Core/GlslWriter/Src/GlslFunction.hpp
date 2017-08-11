/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
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
