/*
See LICENSE file in root folder
*/
#ifndef ___GLSL_Materials_H___
#define ___GLSL_Materials_H___

#include "Castor3DPrerequisites.hpp"

#include <ShaderWriter/MatTypes/Mat4.hpp>
#include <ShaderWriter/CompositeTypes/StructInstance.hpp>

namespace castor3d
{
	namespace shader
	{
		static uint32_t constexpr MaxMaterialsCount = 2000u;
		static int constexpr MaxMaterialComponentsCount = 15;
		static uint32_t constexpr MaxTransmittanceProfileSize = 10u;
		castor::String const PassBufferName = cuT( "Materials" );

		class LegacyMaterials;
		class PbrMRMaterials;
		class PbrSGMaterials;

		struct BaseMaterial
			: public sdw::StructInstance
		{
			friend class Materials;

			C3D_API virtual sdw::Vec3 m_diffuse()const = 0;

		protected:
			C3D_API BaseMaterial( sdw::Shader * shader
				, ast::expr::ExprPtr expr );

		protected:
			using sdw::StructInstance::getMember;
			using sdw::StructInstance::getMemberArray;

		protected:
			sdw::Vec4 m_common;
			sdw::Vec4 m_reflRefr;
			sdw::Vec4 m_sssInfo;

		public:
			sdw::Float m_opacity;
			sdw::Float m_emissive;
			sdw::Float m_alphaRef;
			sdw::Float m_gamma;
			sdw::Float m_refractionRatio;
			sdw::Int m_hasRefraction;
			sdw::Int m_hasReflection;
			sdw::Float m_exposure;
			sdw::Int m_subsurfaceScatteringEnabled;
			sdw::Float m_gaussianWidth;
			sdw::Float m_subsurfaceScatteringStrength;
			sdw::Int m_transmittanceProfileSize;
			sdw::Array< sdw::Vec4 > m_transmittanceProfile;
		};

		DECLARE_SMART_PTR( BaseMaterial );

		struct LegacyMaterial
			: public BaseMaterial
		{
			friend class LegacyMaterials;

			C3D_API LegacyMaterial( sdw::Shader * shader
				, ast::expr::ExprPtr expr );

			C3D_API static ast::type::StructPtr makeType( ast::type::TypesCache & cache );
			C3D_API static std::unique_ptr< sdw::Struct > declare( sdw::ShaderWriter & writer );

			C3D_API sdw::Vec3 m_diffuse()const override;

		private:
			sdw::Vec4 m_diffAmb;
			sdw::Vec4 m_specShin;

		public:
			sdw::Float m_ambient;
			sdw::Vec3 m_specular;
			sdw::Float m_shininess;
		};

		struct MetallicRoughnessMaterial
			: public BaseMaterial
		{
			friend class PbrMRMaterials;

			C3D_API MetallicRoughnessMaterial( sdw::Shader * shader
				, ast::expr::ExprPtr expr );

			C3D_API static ast::type::StructPtr makeType( ast::type::TypesCache & cache );
			C3D_API static std::unique_ptr< sdw::Struct > declare( sdw::ShaderWriter & writer );

			C3D_API sdw::Vec3 m_diffuse()const override;

		protected:
			sdw::Vec4 m_albRough;
			sdw::Vec4 m_metDiv;

		public:
			sdw::Vec3 m_albedo;
			sdw::Float m_roughness;
			sdw::Float m_metallic;
		};

		struct SpecularGlossinessMaterial
			: public BaseMaterial
		{
			friend class PbrSGMaterials;

			C3D_API SpecularGlossinessMaterial( sdw::Shader * shader
				, ast::expr::ExprPtr expr );

			C3D_API static ast::type::StructPtr makeType( ast::type::TypesCache & cache );
			C3D_API static std::unique_ptr< sdw::Struct > declare( sdw::ShaderWriter & writer );

			C3D_API sdw::Vec3 m_diffuse()const override;

		protected:
			sdw::Vec4 m_diffDiv;
			sdw::Vec4 m_specGloss;

		public:
			sdw::Vec3 m_specular;
			sdw::Float m_glossiness;
		};

		class Materials
		{
		protected:
			C3D_API explicit Materials( sdw::ShaderWriter & writer );

		public:
			C3D_API virtual void declare( bool hasSsbo ) = 0;
			C3D_API virtual BaseMaterialUPtr getBaseMaterial( sdw::Int const & index )const = 0;

		protected:
			sdw::ShaderWriter & m_writer;
			std::unique_ptr< sdw::Struct > m_type;
		};

		class LegacyMaterials
			: public Materials
		{
		public:
			C3D_API explicit LegacyMaterials( sdw::ShaderWriter & writer );
			C3D_API void declare( bool hasSsbo )override;
			C3D_API LegacyMaterial getMaterial( sdw::Int const & index )const;
			C3D_API BaseMaterialUPtr getBaseMaterial( sdw::Int const & index )const override;

		private:
			std::unique_ptr< sdw::ArraySsboT< LegacyMaterial > > m_ssbo;
			sdw::Function< LegacyMaterial, sdw::InInt > m_getMaterial;
		};

		class PbrMRMaterials
			: public Materials
		{
		public:
			C3D_API explicit PbrMRMaterials( sdw::ShaderWriter & writer );
			C3D_API void declare( bool hasSsbo )override;
			C3D_API MetallicRoughnessMaterial getMaterial( sdw::Int const & index )const;
			C3D_API BaseMaterialUPtr getBaseMaterial( sdw::Int const & index )const override;

		private:
			std::unique_ptr< sdw::ArraySsboT< MetallicRoughnessMaterial > > m_ssbo;
			sdw::Function< MetallicRoughnessMaterial, sdw::InInt > m_getMaterial;
		};

		class PbrSGMaterials
			: public Materials
		{
		public:
			C3D_API explicit PbrSGMaterials( sdw::ShaderWriter & writer );
			C3D_API void declare( bool hasSsbo )override;
			C3D_API SpecularGlossinessMaterial getMaterial( sdw::Int const & index )const;
			C3D_API BaseMaterialUPtr getBaseMaterial( sdw::Int const & index )const override;

		private:
			std::unique_ptr< sdw::ArraySsboT< SpecularGlossinessMaterial > > m_ssbo;
			sdw::Function< SpecularGlossinessMaterial, sdw::InInt > m_getMaterial;
		};
	}
}

#endif
