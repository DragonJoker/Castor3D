/*
See LICENSE file in root folder
*/
#ifndef ___LightStreaks_KawaseUbo_H___
#define ___LightStreaks_KawaseUbo_H___

#include <Castor3D/Castor3DModule.hpp>

#include <Castor3D/Buffer/UniformBufferOffset.hpp>

namespace light_streaks
{
	struct KawaseUboConfiguration
	{
		castor::Point2f pixelSize;
		castor::Point2f direction;
		int samples;
		float attenuation;
		int pass;
	};
		
	class KawaseUbo
	{
	public:
		using Configuration = KawaseUboConfiguration;

	public:
		explicit KawaseUbo( castor3d::RenderDevice const & device );
		~KawaseUbo();
		void update( uint32_t index
			, VkExtent2D const & size
			, castor::Point2f const & direction
			, uint32_t pass );

		castor3d::UniformBufferOffsetT< Configuration > const & getUbo( uint32_t index )const
		{
			return m_ubo[index];
		}

		castor3d::UniformBufferOffsetT< Configuration > & getUbo( uint32_t index )
		{
			return m_ubo[index];
		}

		void createSizedBinding( ashes::DescriptorSet & descriptorSet
			, VkDescriptorSetLayoutBinding const & layoutBinding
			, uint32_t index )const
		{
			return m_ubo[index].createSizedBinding( descriptorSet, layoutBinding );
		}

		void createPassBinding( crg::FramePass & pass
			, uint32_t binding
			, uint32_t index )const
		{
			return m_ubo[index].createPassBinding( pass, "KawaseCfg", binding );
		}

	public:
		static castor::String const Name;
		static castor::String const PixelSize;
		static castor::String const Direction;
		static castor::String const Samples;
		static castor::String const Attenuation;
		static castor::String const Pass;

	private:
		castor3d::RenderDevice const & m_device;
		std::vector< castor3d::UniformBufferOffsetT< Configuration > > m_ubo;
	};
}

#define UBO_KAWASE( writer, binding, set )\
	sdw::Ubo kawase{ writer, KawaseUbo::Name, binding, set };\
	auto c3d_pixelSize = kawase.declMember< Vec2 >( KawaseUbo::PixelSize );\
	auto c3d_direction = kawase.declMember< Vec2 >( KawaseUbo::Direction );\
	auto c3d_samples = kawase.declMember< Int >( KawaseUbo::Samples );\
	auto c3d_attenuation = kawase.declMember< Float >( KawaseUbo::Attenuation );\
	auto c3d_pass = kawase.declMember< Int >( KawaseUbo::Pass );\
	kawase.end()

#endif
