/*
See LICENSE file in root folder
*/
#ifndef ___C3D_Bloom_BlurPass_HPP___
#define ___C3D_Bloom_BlurPass_HPP___

#include <Castor3D/Render/Passes/GaussianBlur.hpp>
#include <Castor3D/Render/PostEffect/PostEffect.hpp>

namespace Bloom
{
	using UboOffset = c3d::UniformBufferOffsetT< c3d::GaussianBlur::Configuration >;
	using UboOffsetArray = c3d::Vector< UboOffset >;

	class BlurPass
	{
	public:
		BlurPass( crg::FramePassGroup & graph
			, c3d::RenderDevice const & device
			, c3d::Texture const & srcImage
			, c3d::Texture & dstImage
			, c3d::Extent2D dimensions
			, uint32_t blurKernelSize
			, uint32_t blurPassesCount
			, bool isVertical
			, bool const * enabled );
		~BlurPass()noexcept;
		void accept( c3d::ConfigurationVisitorBase & visitor )const;

		void update( uint32_t kernelSize );

		c3d::Texture const & getResult()const
		{
			return m_result;
		}

	public:
		static constexpr uint32_t MaxCoefficients{ 64u };

	private:
		c3d::RenderDevice const & m_device;
		UboOffsetArray m_blurUbo;
		c3d::ProgramModule m_shader;
		ashes::PipelineShaderStageCreateInfoArray m_stages;
		c3d::Texture & m_result;
	};
}

#endif
