/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslMaterials_H___
#define ___C3D_GlslMaterials_H___

#include "SdwModule.hpp"

#include <ShaderWriter/MatTypes/Mat4.hpp>
#include <ShaderWriter/CompositeTypes/StructInstance.hpp>

namespace castor3d
{
	namespace shader
	{
		castor::String const PassBufferName = cuT( "Materials" );

		struct BaseMaterial
			: public sdw::StructInstance
		{
			friend class Materials;
			virtual ~BaseMaterial() = default;

			C3D_API virtual sdw::Vec3 m_diffuse()const = 0;

		protected:
			C3D_API BaseMaterial( sdw::ShaderWriter & writer
				, ast::expr::ExprPtr expr );

		protected:
			using sdw::StructInstance::getMember;
			using sdw::StructInstance::getMemberArray;

		protected:
			sdw::Vec4 m_common;
			sdw::Vec4 m_reflRefr;
			sdw::Vec4 m_sssInfo;

		public:
			sdw::Array< sdw::Vec4 > m_transmittanceProfile;

			sdw::Float m_opacity;
			sdw::Float m_emissive;
			sdw::Float m_alphaRef;
			sdw::Float m_gamma;
			sdw::Float m_refractionRatio;
			sdw::Int m_hasRefraction;
			sdw::Int m_hasReflection;
			sdw::Float m_bwAccumulationOperator;
			sdw::Int m_subsurfaceScatteringEnabled;
			sdw::Float m_gaussianWidth;
			sdw::Float m_subsurfaceScatteringStrength;
			sdw::Int m_transmittanceProfileSize;
		};

		CU_DeclareSmartPtr( BaseMaterial );

		struct LegacyMaterial
			: public BaseMaterial
		{
			friend class LegacyMaterials;

			C3D_API LegacyMaterial( sdw::ShaderWriter & writer
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

			C3D_API MetallicRoughnessMaterial( sdw::ShaderWriter & writer
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

			C3D_API SpecularGlossinessMaterial( sdw::ShaderWriter & writer
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
			virtual ~Materials() = default;
			C3D_API virtual void declare( bool hasSsbo ) = 0;
			C3D_API virtual BaseMaterialUPtr getBaseMaterial( sdw::UInt const & index )const = 0;

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
			C3D_API LegacyMaterial getMaterial( sdw::UInt const & index )const;
			C3D_API BaseMaterialUPtr getBaseMaterial( sdw::UInt const & index )const override;

		private:
			std::unique_ptr< sdw::ArraySsboT< LegacyMaterial > > m_ssbo;
			sdw::Function< LegacyMaterial, sdw::InUInt > m_getMaterial;
		};

		class PbrMRMaterials
			: public Materials
		{
		public:
			C3D_API explicit PbrMRMaterials( sdw::ShaderWriter & writer );
			C3D_API void declare( bool hasSsbo )override;
			C3D_API MetallicRoughnessMaterial getMaterial( sdw::UInt const & index )const;
			C3D_API BaseMaterialUPtr getBaseMaterial( sdw::UInt const & index )const override;

		private:
			std::unique_ptr< sdw::ArraySsboT< MetallicRoughnessMaterial > > m_ssbo;
			sdw::Function< MetallicRoughnessMaterial, sdw::InUInt > m_getMaterial;
		};

		class PbrSGMaterials
			: public Materials
		{
		public:
			C3D_API explicit PbrSGMaterials( sdw::ShaderWriter & writer );
			C3D_API void declare( bool hasSsbo )override;
			C3D_API SpecularGlossinessMaterial getMaterial( sdw::UInt const & index )const;
			C3D_API BaseMaterialUPtr getBaseMaterial( sdw::UInt const & index )const override;

		private:
			std::unique_ptr< sdw::ArraySsboT< SpecularGlossinessMaterial > > m_ssbo;
			sdw::Function< SpecularGlossinessMaterial, sdw::InUInt > m_getMaterial;
		};
	}
}

#endif
