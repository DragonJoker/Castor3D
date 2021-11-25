/*
See LICENSE file in root folder
*/
#ifndef ___GLSL_TextureConfiguration_H___
#define ___GLSL_TextureConfiguration_H___

#include "SdwModule.hpp"

#include <ShaderWriter/MatTypes/Mat4.hpp>
#include <ShaderWriter/CompositeTypes/StructInstance.hpp>

namespace castor3d
{
	namespace shader
	{
		castor::String const TextureConfigurationBufferName = cuT( "TextureConfigurations" );

		struct TextureConfigData
			: public sdw::StructInstance
		{
			friend class TextureConfigurations;

		public:
			C3D_API TextureConfigData( sdw::ShaderWriter & writer
				, ast::expr::ExprPtr expr
				, bool enabled );
			SDW_DeclStructInstance( C3D_API, TextureConfigData );

			C3D_API static std::unique_ptr< sdw::Struct > declare( sdw::ShaderWriter & writer );
			C3D_API static ast::type::BaseStructPtr makeType( ast::type::TypesCache & cache );

			C3D_API sdw::Vec3 getDiffuse( sdw::ShaderWriter & writer
				, sdw::Vec4 const & sampled
				, sdw::Vec3 const & diffuse )const;
			C3D_API sdw::Vec3 getAlbedo( sdw::ShaderWriter & writer
				, sdw::Vec4 const & sampled
				, sdw::Vec3 const & diffuse )const;
			C3D_API sdw::Vec3 getEmissive( sdw::ShaderWriter & writer
				, sdw::Vec4 const & sampled
				, sdw::Vec3 const & emissive )const;
			C3D_API sdw::Vec3 getSpecular( sdw::ShaderWriter & writer
				, sdw::Vec4 const & sampled
				, sdw::Vec3 const & specular )const;
			C3D_API sdw::Float getMetalness( sdw::ShaderWriter & writer
				, sdw::Vec4 const & sampled
				, sdw::Float const & metalness )const;
			C3D_API sdw::Float getShininess( sdw::ShaderWriter & writer
				, sdw::Vec4 const & sampled
				, sdw::Float const & shininess )const;
			C3D_API sdw::Float getGlossiness( sdw::ShaderWriter & writer
				, sdw::Vec4 const & sampled
				, sdw::Float const & glossiness )const;
			C3D_API sdw::Float getRoughness( sdw::ShaderWriter & writer
				, sdw::Vec4 const & sampled
				, sdw::Float const & roughness )const;
			C3D_API sdw::Float getOpacity( sdw::ShaderWriter & writer
				, sdw::Vec4 const & sampled
				, sdw::Float const & opacity )const;
			C3D_API sdw::Vec3 getNormal( sdw::ShaderWriter & writer
				, sdw::Vec4 const & sampled
				, sdw::Mat3 const & tbn )const;
			C3D_API sdw::Vec3 getNormal( sdw::ShaderWriter & writer
				, sdw::Vec4 const & sampled
				, sdw::Vec3 const & normal
				, sdw::Vec3 const & tangent
				, sdw::Vec3 const & bitangent )const;
			C3D_API sdw::Float getHeight( sdw::ShaderWriter & writer
				, sdw::Vec4 const & sampled )const;
			C3D_API sdw::Float getOcclusion( sdw::ShaderWriter & writer
				, sdw::Vec4 const & sampled
				, sdw::Float const & occlusion )const;
			C3D_API sdw::Float getTransmittance( sdw::ShaderWriter & writer
				, sdw::Vec4 const & sampled
				, sdw::Float const & transmittance )const;
			C3D_API void convertUV( sdw::ShaderWriter & writer
				, sdw::Vec2 & uv )const;
			C3D_API void convertUVW( sdw::ShaderWriter & writer
				, sdw::Vec3 & uvw )const;

		private:
			sdw::Float getFloat( sdw::ShaderWriter & writer
				, sdw::Vec4 const & sampled
				, sdw::Float const & mask )const;
			sdw::Vec3 getVec3( sdw::ShaderWriter & writer
				, sdw::Vec4 const & sampled
				, sdw::Float const & mask )const;

		private:
			using sdw::StructInstance::getMember;
			using sdw::StructInstance::getMemberArray;

		public:
			sdw::Vec4 colOpa;
			sdw::Vec4 spcShn;
			sdw::Vec4 metRgh;
			sdw::Vec4 emsOcc;
			sdw::Vec4 trsDum;
			sdw::Vec4 nmlFcr;
			sdw::Vec4 hgtFcr;
			sdw::Vec4 mscVls;
			sdw::Vec4 texTrn;
			sdw::Vec4 texRot;
			sdw::Vec4 texScl;

		public:
			sdw::Float colEnbl;
			sdw::Float colMask;
			sdw::Float opaEnbl;
			sdw::Float opaMask;
			sdw::Float spcEnbl;
			sdw::Float spcMask;
			sdw::Float shnEnbl;
			sdw::Float shnMask;
			sdw::Float metEnbl;
			sdw::Float metMask;
			sdw::Float rghEnbl;
			sdw::Float rghMask;
			sdw::Float emsEnbl;
			sdw::Float emsMask;
			sdw::Float occEnbl;
			sdw::Float occMask;
			sdw::Float trsEnbl;
			sdw::Float trsMask;
			sdw::Float nmlEnbl;
			sdw::Float nmlMask;
			sdw::Float nmlFact;
			sdw::Float nmlGMul;
			sdw::Float hgtEnbl;
			sdw::Float hgtMask;
			sdw::Float hgtFact;
			sdw::Float fneedYI;
			sdw::UInt needsYI;
		};

		class TextureConfigurations
		{
		public:
			C3D_API explicit TextureConfigurations( sdw::ShaderWriter & writer );
			C3D_API void declare( bool hasSsbo
				, uint32_t binding
				, uint32_t set );
			C3D_API TextureConfigData getTextureConfiguration( sdw::UInt const & index )const;

		private:
			sdw::ShaderWriter & m_writer;
			std::unique_ptr< sdw::Struct > m_type;
			std::unique_ptr< sdw::ArraySsboT< TextureConfigData > > m_ssbo;
			sdw::Function< TextureConfigData
				, sdw::InUInt > m_getTextureConfiguration;
		};
	}
}

#endif
