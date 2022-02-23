/*
See LICENSE file in root folder
*/
#ifndef ___LightStreaks_KawaseUbo_H___
#define ___LightStreaks_KawaseUbo_H___

#include <Castor3D/Castor3DModule.hpp>
#include <Castor3D/Buffer/UniformBufferOffset.hpp>

#include <ShaderWriter/BaseTypes/Float.hpp>
#include <ShaderWriter/BaseTypes/Int.hpp>
#include <ShaderWriter/CompositeTypes/StructInstance.hpp>
#include <ShaderWriter/VecTypes/Vec2.hpp>

namespace light_streaks
{
	struct KawaseConfig
	{
		int samples{ 4 };
		float attenuation{ 0.9f };
	};

	struct KawaseUboConfiguration
	{
		castor::Point2f pixelSize{};
		castor::Point2f direction{};
		int samples{ 4 };
		float attenuation{ 0.9f };
		int pass{};
	};

	struct KawaseData
		: public sdw::StructInstance
	{
	public:
		KawaseData( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled );
		SDW_DeclStructInstance( , KawaseData );

		static ast::type::BaseStructPtr makeType( ast::type::TypesCache & cache );

	public:
		sdw::Vec2 pixelSize;
		sdw::Vec2 direction;
		sdw::Int samples;
		sdw::Float attenuation;
		sdw::Int pass;

	private:
		using sdw::StructInstance::getMember;
		using sdw::StructInstance::getMemberArray;
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
		void update( KawaseConfig const & config );

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
			return m_ubo[index].createPassBinding( pass, "KawaseCfg" + std::to_string( index ), binding );
		}

	public:
		static castor::String const Buffer;
		static castor::String const Data;

	private:
		castor3d::RenderDevice const & m_device;
		std::vector< castor3d::UniformBufferOffsetT< Configuration > > m_ubo;
	};
}

#define C3D_Kawase( writer, binding, set )\
	sdw::Ubo kawase{ writer, KawaseUbo::Buffer, binding, set };\
	auto c3d_kawaseData = kawase.declMember< KawaseData >( KawaseUbo::Data );\
	kawase.end()

#endif
