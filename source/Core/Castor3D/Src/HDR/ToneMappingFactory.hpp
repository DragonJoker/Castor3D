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
#ifndef ___C3D_TONE_MAPPING_FACTORY_H___
#define ___C3D_TONE_MAPPING_FACTORY_H___

#include "Castor3DPrerequisites.hpp"

#include <Design/Factory.hpp>

namespace Castor3D
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.8.0
	\date		11/03/2016
	\~english
	\brief		Tone mapping factory.
	\~french
	\brief		Fabrique de mappage de tons.
	*/
	using ToneMappingFactory = Castor::Factory< ToneMapping, Castor::String, ToneMappingSPtr, std::function< ToneMappingSPtr( Engine &, Parameters const & ) > >;
}

#endif
