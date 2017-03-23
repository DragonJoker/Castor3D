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
#ifndef ___GL_HOLDER_H___
#define ___GL_HOLDER_H___

#include "GlRenderSystemPrerequisites.hpp"

namespace GlRender
{
	class OpenGl;
	/*!
	@author
		Sylvain DOREMUS
	@date
		19/11/2015
	@version
		0.8.0
	@brief
		Base class for each class needing an OpenGl weak pointer
	*/
	class Holder
	{
	public:
		/** Constructor
		@param[in] p_openGl
			The OpenGl instance
		*/
		inline Holder( OpenGl & p_openGl )
			: m_openGl( p_openGl )
		{
		}

		/** Destructor
		*/
		inline ~Holder()
		{
		}

		/** Retrieves the OpenGl instance
		@return
			The instance
		*/
		inline OpenGl const & GetOpenGl()const
		{
			return m_openGl;
		}

		/** Retrieves the OpenGl instance
		@return
			The instance
		*/
		inline OpenGl & GetOpenGl()
		{
			return m_openGl;
		}

	private:
		//! The OpenGl instance
		OpenGl & m_openGl;
	};
}

#endif
