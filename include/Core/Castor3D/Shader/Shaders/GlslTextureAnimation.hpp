/*
See LICENSE file in root folder
*/
#ifndef ___GLSL_TextureAnimation_H___
#define ___GLSL_TextureAnimation_H___

#include "GlslBuffer.hpp"

#include <ShaderWriter/MatTypes/Mat4.hpp>
#include <ShaderWriter/CompositeTypes/StructInstance.hpp>

namespace castor3d
{
	namespace shader
	{
		castor::String const TextureAnimationBufferName = cuT( "C3D_TextureAnimations" );

		struct TextureAnimData
			: public sdw::StructInstance
		{
			friend class TextureAnimations;

		public:
			C3D_API TextureAnimData( sdw::ShaderWriter & writer
				, ast::expr::ExprPtr expr
				, bool enabled );
			SDW_DeclStructInstance( C3D_API, TextureAnimData );

			C3D_API static std::unique_ptr< sdw::Struct > declare( sdw::ShaderWriter & writer );
			C3D_API static ast::type::BaseStructPtr makeType( ast::type::TypesCache & cache );

		private:
			using sdw::StructInstance::getMember;
			using sdw::StructInstance::getMemberArray;

		public:
			sdw::Vec4 texTrn;
			sdw::Vec4 texRot;
			sdw::Vec4 texScl;
			sdw::Vec4 tleSet;
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
				return BufferT< TextureAnimData >::getData( index );
			}
		};
	}
}

#endif
