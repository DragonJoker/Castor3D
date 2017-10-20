/* See LICENSE file in root folder */
#ifndef ___C3DGLES3_GlES3PushUniform_H___
#define ___C3DGLES3_GlES3PushUniform_H___

#include <Shader/PushUniform.hpp>

#include "Shader/GlES3UniformBase.hpp"

namespace GlES3Render
{
	template< Castor3D::UniformType Type >
	class GlES3PushUniform
		: public Castor3D::TPushUniform< Type >
		, public Holder
	{
	public:
		GlES3PushUniform( OpenGlES3 & p_gl, GlES3ShaderProgram & p_program, uint32_t p_occurences );
		virtual ~GlES3PushUniform();

		inline uint32_t GetGlES3Name() const
		{
			return m_glName;
		}

	private:
		/**
		 *\copydoc		Castor3D::Uniform::DoInitialise
		 */
		bool DoInitialise()override;
		/**
		 *\copydoc		Castor3D::Uniform::DoUpdate
		 */
		void DoUpdate()const override;

	private:
		uint32_t m_glName{ GlES3InvalidIndex };
		bool m_presentInProgram{ true };
	};
}

#include "GlES3PushUniform.inl"

#endif
