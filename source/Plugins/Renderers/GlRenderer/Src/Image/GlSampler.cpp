#include "Image/GlSampler.hpp"

#include "Core/GlDevice.hpp"

namespace gl_renderer
{
	Sampler::Sampler( renderer::Device const & device
		, renderer::SamplerCreateInfo const & createInfo )
		: renderer::Sampler{ device, createInfo }
	{
		glLogCall( gl::GenSamplers, 1, &m_sampler );
		glLogCall( gl::BindSampler, 0u, m_sampler );
		glLogCall( gl::SamplerParameteri, m_sampler, GL_SAMPLER_PARAMETER_MIN_FILTER, convert( createInfo.minFilter, createInfo.mipmapMode ) );
		glLogCall( gl::SamplerParameteri, m_sampler, GL_SAMPLER_PARAMETER_MAG_FILTER, convert( createInfo.magFilter ) );
		glLogCall( gl::SamplerParameteri, m_sampler, GL_SAMPLER_PARAMETER_WRAP_S, convert( createInfo.addressModeU ) );
		glLogCall( gl::SamplerParameteri, m_sampler, GL_SAMPLER_PARAMETER_WRAP_T, convert( createInfo.addressModeV ) );
		glLogCall( gl::SamplerParameteri, m_sampler, GL_SAMPLER_PARAMETER_WRAP_R, convert( createInfo.addressModeW ) );
		glLogCall( gl::SamplerParameterf, m_sampler, GL_SAMPLER_PARAMETER_MIN_LOD, createInfo.minLod );
		glLogCall( gl::SamplerParameterf, m_sampler, GL_SAMPLER_PARAMETER_MAX_LOD, createInfo.maxLod );

		if ( device.getFeatures().samplerAnisotropy && createInfo.anisotropyEnable )
		{
			glLogCall( gl::SamplerParameterf, m_sampler, GL_SAMPLER_PARAMETER_MAX_ANISOTROPY, createInfo.maxAnisotropy );
		}

		if ( createInfo.compareEnable )
		{
			glLogCall( gl::SamplerParameteri, m_sampler, GL_SAMPLER_PARAMETER_COMPARE_MODE, GL_SAMPLER_PARAMETER_COMPARE_REF_TO_TEXTURE );
			glLogCall( gl::SamplerParameteri, m_sampler, GL_SAMPLER_PARAMETER_COMPARE_FUNC, convert( createInfo.compareOp ) );
		}

		float fvalues[4] = { 0.0f };
		int ivalues[4] = { 0 };

		switch ( createInfo.borderColor )
		{
		case renderer::BorderColour::eFloatTransparentBlack:
			glLogCall( gl::SamplerParameterfv, m_sampler, GL_SAMPLER_PARAMETER_BORDER_COLOR, fvalues );
			break;

		case renderer::BorderColour::eIntTransparentBlack:
			glLogCall( gl::SamplerParameteriv, m_sampler, GL_SAMPLER_PARAMETER_BORDER_COLOR, ivalues );
			break;

		case renderer::BorderColour::eFloatOpaqueBlack:
			fvalues[3] = 1.0f;
			glLogCall( gl::SamplerParameterfv, m_sampler, GL_SAMPLER_PARAMETER_BORDER_COLOR, fvalues );
			break;

		case renderer::BorderColour::eIntOpaqueBlack:
			ivalues[3] = 255;
			glLogCall( gl::SamplerParameteriv, m_sampler, GL_SAMPLER_PARAMETER_BORDER_COLOR, ivalues );
			break;

		case renderer::BorderColour::eFloatOpaqueWhite:
			fvalues[0] = 1.0f;
			fvalues[1] = 1.0f;
			fvalues[2] = 1.0f;
			fvalues[3] = 1.0f;
			glLogCall( gl::SamplerParameterfv, m_sampler, GL_SAMPLER_PARAMETER_BORDER_COLOR, fvalues );
			break;

		case renderer::BorderColour::eIntOpaqueWhite:
			ivalues[0] = 255;
			ivalues[1] = 255;
			ivalues[2] = 255;
			ivalues[3] = 255;
			glLogCall( gl::SamplerParameteriv, m_sampler, GL_SAMPLER_PARAMETER_BORDER_COLOR, ivalues );
			break;
		}
	}

	Sampler::~Sampler()
	{
		glLogCall( gl::DeleteSamplers, 1, &m_sampler );
	}
}
