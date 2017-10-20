/*
See LICENSE file in root folder
*/
#ifndef ___C3DGL_GlPushUniform_H___
#define ___C3DGL_GlPushUniform_H___

#include <Shader/Uniform/PushUniform.hpp>

#include "Shader/GlUniformBase.hpp"

namespace GlRender
{
	template< castor3d::UniformType Type >
	class GlPushUniform
		: public castor3d::TPushUniform< Type >
		, public Holder
	{
	public:
		GlPushUniform( OpenGl & p_gl, GlShaderProgram & p_program, uint32_t p_occurences );
		virtual ~GlPushUniform();

		inline uint32_t getGlName() const
		{
			return m_glName;
		}

	private:
		/**
		 *\copydoc		castor3d::Uniform::doInitialise
		 */
		bool doInitialise()override;
		/**
		 *\copydoc		castor3d::Uniform::doUpdate
		 */
		void doUpdate()const override;

	private:
		uint32_t m_glName{ GlInvalidIndex };
		bool m_presentInProgram{ true };
	};
}

#include "GlPushUniform.inl"

#endif
