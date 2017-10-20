/*
See LICENSE file in root folder
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
		inline OpenGl const & getOpenGl()const
		{
			return m_openGl;
		}

		/** Retrieves the OpenGl instance
		@return
			The instance
		*/
		inline OpenGl & getOpenGl()
		{
			return m_openGl;
		}

	private:
		//! The OpenGl instance
		OpenGl & m_openGl;
	};
}

#endif
