/*
See LICENSE file in root folder
*/
#ifndef ___LightStreaks_KawaseUbo_H___
#define ___LightStreaks_KawaseUbo_H___

#include <Castor3D/Castor3DModule.hpp>
#include <Castor3D/Buffer/UniformBufferOffset.hpp>

#include <ShaderWriter/BaseTypes/Float.hpp>
#include <ShaderWriter/BaseTypes/Int.hpp>
#include <ShaderWriter/CompositeTypes/StructInstanceHelper.hpp>
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
		c3d::Point2f pixelSize{};
		c3d::Point2f direction{};
		int samples{ 4 };
		float attenuation{ 0.9f };
		int pass{};
	};

	struct KawaseData
		: public sdw::StructInstanceHelperT< "C3D_KawaseData"
			, sdw::type::MemoryLayout::eStd140
			, sdw::Vec2Field< "pixelSize" >
			, sdw::Vec2Field< "direction" >
			, sdw::IntField< "samples" >
			, sdw::FloatField< "attenuation" >
			, sdw::IntField< "pass" >
			, sdw::IntField< "pad" > >
	{
		SDW_DeclStructInstance( , KawaseData );

		KawaseData( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled )
			: StructInstanceHelperT{ writer, c3d::move( expr ), enabled }
			, pixelSize{ StructInstanceHelperT::getMember< "pixelSize" >() }
			, direction{ StructInstanceHelperT::getMember< "direction" >() }
			, samples{ StructInstanceHelperT::getMember< "samples" >() }
			, attenuation{ StructInstanceHelperT::getMember< "attenuation" >() }
			, pass{ StructInstanceHelperT::getMember< "pass" >() }
		{
		}

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
		KawaseUbo( KawaseUbo const & ) = delete;
		KawaseUbo & operator=( KawaseUbo const & ) = delete;
		KawaseUbo( KawaseUbo && )noexcept = delete;
		KawaseUbo & operator=( KawaseUbo && )noexcept = delete;

	public:
		explicit KawaseUbo( c3d::RenderDevice const & device );
		~KawaseUbo();
		void update( uint32_t index
			, c3d::Extent2D const & size
			, c3d::Point2f const & direction
			, uint32_t pass );
		void update( KawaseConfig const & config );

		c3d::UniformBufferOffsetT< Configuration > const & getUbo( uint32_t index )const
		{
			return m_ubo[index];
		}

		c3d::UniformBufferOffsetT< Configuration > & getUbo( uint32_t index )
		{
			return m_ubo[index];
		}

		void createSizedBinding( ashes::DescriptorSet & descriptorSet
			, VkDescriptorSetLayoutBinding const & layoutBinding
			, uint32_t index )const
		{
			return m_ubo[index].createSizedBinding( descriptorSet, layoutBinding );
		}

		template< typename BindingT >
		void createPassBinding( crg::FramePass & pass
			, BindingT binding
			, uint32_t index )const
		{
			return m_ubo[index].createPassBinding( pass, binding );
		}

	public:
		static c3d::MbString const Buffer;
		static c3d::MbString const Data;

	private:
		c3d::RenderDevice const & m_device;
		c3d::Vector< c3d::UniformBufferOffsetT< Configuration > > m_ubo;
	};
}

#define C3D_Kawase( writer, binding, set )\
	sdw::UniformBuffer kawase{ writer, KawaseUbo::Buffer, uint32_t( binding ), uint32_t( set ) };\
	auto c3d_kawaseData = kawase.declMember< KawaseData >( KawaseUbo::Data );\
	kawase.end()

#endif
