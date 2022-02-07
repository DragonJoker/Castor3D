/*
See LICENSE file in root folder
*/
#ifndef ___GLSL_TextureAnimation_H___
#define ___GLSL_TextureAnimation_H___

#include "SdwModule.hpp"

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

			C3D_API void animUV( TextureConfigData const & config
				, sdw::Vec2 & uv )const;
			C3D_API void animUVW( TextureConfigData const & config
				, sdw::Vec3 & uvw )const;

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
		{
		public:
			C3D_API explicit TextureAnimations( sdw::ShaderWriter & writer );
			C3D_API void declare( uint32_t binding
				, uint32_t set );
			C3D_API TextureAnimData getTextureAnimation( sdw::UInt const & index )const;

		private:
			sdw::ShaderWriter & m_writer;
			std::unique_ptr< sdw::ArraySsboT< TextureAnimData > > m_ssbo;
		};
	}
}

#endif
