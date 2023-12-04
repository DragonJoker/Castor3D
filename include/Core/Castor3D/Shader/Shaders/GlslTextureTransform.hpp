/*
See LICENSE file in root folder
*/
#ifndef ___GLSL_TextureTransform_H___
#define ___GLSL_TextureTransform_H___

#include "GlslBuffer.hpp"

#include <ShaderWriter/MatTypes/Mat4.hpp>
#include <ShaderWriter/CompositeTypes/StructInstanceHelper.hpp>

namespace castor3d
{
	namespace shader
	{
		struct TextureTransformData
			: public sdw::StructInstanceHelperT< "C3D_TextureTransformData"
				, ast::type::MemoryLayout::eStd430
				, sdw::Vec3Field< "translate" >
				, sdw::FloatField< "rotateU" >
				, sdw::Vec3Field< "scale" >
				, sdw::FloatField< "rotateV" >
				, sdw::Vec4Field< "tileSet" > >
		{
		public:
			TextureTransformData( sdw::ShaderWriter & writer
				, ast::expr::ExprPtr expr
				, bool enabled )
				: StructInstanceHelperT{ writer, std::move( expr ), enabled }
			{
			}

			auto translate()const { return getMember< "translate" >(); }
			auto rotateU()const { return getMember< "rotateU" >(); }
			auto rotateV()const { return getMember< "rotateV" >(); }
			auto scale()const { return getMember< "scale" >(); }
			auto tileSet()const { return getMember< "tileSet" >(); }
		};
	}
}

#endif
