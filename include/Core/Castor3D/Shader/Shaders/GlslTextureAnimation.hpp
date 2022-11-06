/*
See LICENSE file in root folder
*/
#ifndef ___GLSL_TextureAnimation_H___
#define ___GLSL_TextureAnimation_H___

#include "GlslBuffer.hpp"

#include <ShaderWriter/MatTypes/Mat4.hpp>
#include <ShaderWriter/CompositeTypes/StructInstanceHelper.hpp>

namespace castor3d
{
	namespace shader
	{
		struct TextureAnimData
			: public sdw::StructInstanceHelperT< "C3D_TextureAnimData"
				, ast::type::MemoryLayout::eStd430
				, sdw::Vec3Field< "translate" >
				, sdw::FloatField< "rotateU" >
				, sdw::Vec3Field< "scale" >
				, sdw::FloatField< "rotateV" >
				, sdw::Vec4Field< "tileSet" > >
		{
			friend class TextureAnimations;

		public:
			TextureAnimData( sdw::ShaderWriter & writer
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

		class TextureAnimations
			: public BufferT< TextureAnimData >
		{
		public:
			C3D_API TextureAnimations( sdw::ShaderWriter & writer
				, uint32_t binding
				, uint32_t set 
				, bool enable = true );

			TextureAnimData getTextureAnimation( sdw::UInt const & index )const
			{
				return BufferT< TextureAnimData >::getData( index - 1u );
			}
		};
	}
}

#endif
