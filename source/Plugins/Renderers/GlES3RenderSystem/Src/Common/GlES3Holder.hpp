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
#ifndef ___C3DGLES3_HOLDER_H___
#define ___C3DGLES3_HOLDER_H___

#include "GlES3RenderSystemPrerequisites.hpp"

namespace GlES3Render
{
	class OpenGlES3;
	/*!
	@author
		Sylvain DOREMUS
	@date
		19/11/2015
	@version
		0.8.0
	@brief
		Base class for each class needing an OpenGlES3 weak pointer
	*/
	class Holder
	{
	public:
		/** Constructor
		@param[in] p_openGlES3
			The OpenGlES3 instance
		*/
		inline Holder( OpenGlES3 & p_openGlES3 )
			: m_openGlES3( p_openGlES3 )
		{
		}

		/** Destructor
		*/
		inline ~Holder()
		{
		}

		/** Retrieves the OpenGlES3 instance
		@return
			The instance
		*/
		inline OpenGlES3 const & GetOpenGlES3()const
		{
			return m_openGlES3;
		}

		/** Retrieves the OpenGlES3 instance
		@return
			The instance
		*/
		inline OpenGlES3 & GetOpenGlES3()
		{
			return m_openGlES3;
		}

	private:
		//! The OpenGlES3 instance
		OpenGlES3 & m_openGlES3;
	};
}

#endif
