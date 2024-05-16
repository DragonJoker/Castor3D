/*
See LICENSE file in root folder
*/
#ifndef ___C3D_FontUbo_H___
#define ___C3D_FontUbo_H___

#include "Castor3D/Shader/Ubos/UbosModule.hpp"
#include "Castor3D/Buffer/UniformBufferOffset.hpp"

#include <ShaderWriter/BaseTypes/Float.hpp>
#include <ShaderWriter/CompositeTypes/StructInstanceHelper.hpp>

namespace castor3d
{
	namespace shader
	{
		struct FontData
			: public sdw::StructInstanceHelperT< "C3D_FontData"
				, sdw::type::MemoryLayout::eStd140
				, sdw::Vec2Field< "imgSize" >
				, sdw::UIntField< "sdfFont" >
				, sdw::FloatField< "pixelRange" > >
		{
		public:
			FontData( sdw::ShaderWriter & writer
				, sdw::expr::ExprPtr expr
				, bool enabled = true )
				: StructInstanceHelperT{ writer, std::move( expr ), enabled }
			{
			}

			auto imgSize()const { return getMember< "imgSize" >(); }
			auto isSDFFont()const { return getMember< "sdfFont" >() != 0_u; }
			auto pixelRange()const { return getMember< "pixelRange" >(); }
		};
	}

	class FontUbo
	{
	public:
		using Configuration = FontUboConfiguration;
		C3D_API FontUbo( FontUbo const & rhs ) = delete;
		C3D_API FontUbo & operator=( FontUbo const & rhs ) = delete;
		C3D_API FontUbo( FontUbo && rhs )noexcept = default;
		C3D_API FontUbo & operator=( FontUbo && rhs )noexcept = delete;
		C3D_API explicit FontUbo( RenderDevice const & device );
		C3D_API ~FontUbo()noexcept;
		/**
		 *\~english
		 *\brief		Updates the UBO from given values.
		 *\param[in]	imgSize		The font texture dimensions.
		 *\param[in]	sdfFont		\p true if the font texture stores SDF.
		 *\param[in]	pixelRange	The distance field thickness.
		 *\~french
		 *\brief		Met à jour l'UBO avec les valeurs données.
		 *\param[in]	imgSize		Les dimensions de la texture de police.
		 *\param[in]	sdfFont		\p true si la texture de police stocke des SDF.
		 *\param[in]	pixelRange	L'épaisseur du distance field.
		 */
		C3D_API void cpuUpdate( castor::Size const & imgSize
			, bool sdfFont
			, float pixelRange );

		void createPassBinding( crg::FramePass & pass
			, uint32_t binding )const
		{
			return m_ubo.createPassBinding( pass, "Font", binding );
		}

		void createSizedBinding( ashes::DescriptorSet & descriptorSet
			, VkDescriptorSetLayoutBinding const & layoutBinding )const
		{
			return m_ubo.createSizedBinding( descriptorSet, layoutBinding );
		}

		ashes::WriteDescriptorSet getDescriptorWrite( uint32_t dstBinding
			, uint32_t dstArrayElement = 0u )const
		{
			return m_ubo.getDescriptorWrite( dstBinding, dstArrayElement );
		}

		UniformBufferOffsetT< Configuration > const & getUbo()const
		{
			return m_ubo;
		}

	private:
		RenderDevice const & m_device;
		UniformBufferOffsetT< Configuration > m_ubo;
	};
}

#define C3D_FontEx( writer, binding, set, enable )\
	sdw::UniformBuffer font{ writer\
		, "C3D_Font"\
		, "c3d_font"\
		, uint32_t( binding )\
		, uint32_t( set )\
		, ast::type::MemoryLayout::eStd140\
		, enable };\
	auto c3d_fontData = font.declMember< castor3d::shader::FontData >( "f", enable );\
	font.end()

#define C3D_Font( writer, binding, set )\
	C3D_FontEx( writer, binding, set, true )

#endif
