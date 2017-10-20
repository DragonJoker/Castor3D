/* See LICENSE file in root folder */
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
