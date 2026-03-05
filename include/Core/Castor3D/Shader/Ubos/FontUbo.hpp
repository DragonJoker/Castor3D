/*
See LICENSE file in root folder
*/
#ifndef ___C3D_FontUbo_H___
#define ___C3D_FontUbo_H___

#include "Castor3D/Shader/Ubos/Ubo.hpp"

#include <ShaderWriter/BaseTypes/Float.hpp>
#include <ShaderWriter/CompositeTypes/StructInstanceHelper.hpp>

namespace c3d
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
			SDW_DeclStructInstance( C3D_INL_API, FontData );

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
		: public UboT< FontUboConfiguration >
	{
	public:
		using Configuration = FontUboConfiguration;

	public:
		C3D_API explicit FontUbo( RenderDevice const & device );
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
		C3D_API void cpuUpdate( Size const & imgSize
			, bool sdfFont
			, float pixelRange );
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
	auto c3d_fontData = font.declMember< c3d::shader::FontData >( "f", enable );\
	font.end()

#define C3D_Font( writer, binding, set )\
	C3D_FontEx( writer, binding, set, true )

#endif
