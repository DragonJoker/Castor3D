/*
See LICENSE file in root folder
*/
#ifndef ___GLSL_Font_H___
#define ___GLSL_Font_H___

#include "GlslBuffer.hpp"

#include <ShaderWriter/VecTypes/Vec2.hpp>
#include <ShaderWriter/CompositeTypes/StructInstanceHelper.hpp>

namespace castor3d::shader
{
	class FontGlyph
		: public sdw::StructInstanceHelperT< "C3D_FontGlyph"
			, sdw::type::MemoryLayout::eStd430
			, sdw::Vec2Field< "size" >
			, sdw::Vec2Field< "bearing" >
			, sdw::Vec2Field< "position" >
			, sdw::FloatField< "advance" >
			, sdw::FloatField< "pad" > >
	{
	public:
		FontGlyph( sdw::ShaderWriter & writer
			, sdw::expr::ExprPtr expr
			, bool enabled )
			: StructInstanceHelperT{ writer, std::move( expr ), enabled }
		{
		}

		auto size()const { return getMember< "size" >(); }
		auto bearing()const { return getMember< "bearing" >(); }
		auto position()const { return getMember< "position" >(); }
		auto advance()const { return getMember< "advance" >(); }
	};

	class FontData
		: public shader::BufferT< FontGlyph >
	{
	public:
		C3D_API FontData( sdw::ShaderWriter & writer
			, uint32_t binding
			, uint32_t set
			, bool enabled = true );

		C3D_API sdw::Float imgWidth()const;
		C3D_API sdw::Float imgHeight()const;
		C3D_API sdw::Float maxHeight()const;

		auto operator[]( sdw::UInt const & index )const
		{
			return this->getData( index );
		}
	};
}

#endif
