/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslPbrMaterialModel_H___
#define ___C3D_GlslPbrMaterialModel_H___

#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Shader/Ubos/UbosModule.hpp"

#include "Castor3D/Shader/Shaders/GlslCookTorranceBRDF.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"

namespace castor3d::shader
{
	C3D_API ast::expr::ExprList makeFnArg( sdw::ShaderWriter & shader
		, PbrLightMaterial const & value );

	struct MetallicRoughnessMaterial
		: public BaseMaterial
	{
		friend class PbrMRMaterials;

		C3D_API MetallicRoughnessMaterial( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled );

		C3D_API PbrLightMaterial getLightMaterial()const;
		C3D_API static PbrLightMaterial getLightMaterial( sdw::Vec3 albedo
			, sdw::Float metalness
			, sdw::Float roughness );
		C3D_API static ast::type::StructPtr makeType( ast::type::TypesCache & cache );
		C3D_API static std::unique_ptr< sdw::Struct > declare( sdw::ShaderWriter & writer );

		C3D_API sdw::Vec3 colour()const override;

	protected:
		sdw::Vec4 m_albRough;
		sdw::Vec4 m_metDiv;

	public:
		sdw::Vec3 albedo;
		sdw::Float roughness;
		sdw::Float metalness;
	};

	struct SpecularGlossinessMaterial
		: public BaseMaterial
	{
		friend class PbrSGMaterials;

		C3D_API SpecularGlossinessMaterial( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled );

		C3D_API PbrLightMaterial getLightMaterial()const;
		C3D_API static PbrLightMaterial getLightMaterial( sdw::Vec3 albedo
			, sdw::Vec3 specular
			, sdw::Float glossiness );
		C3D_API static ast::type::StructPtr makeType( ast::type::TypesCache & cache );
		C3D_API static std::unique_ptr< sdw::Struct > declare( sdw::ShaderWriter & writer );

		C3D_API sdw::Vec3 colour()const override;

	protected:
		sdw::Vec4 m_diffDiv;
		sdw::Vec4 m_specGloss;

	public:
		sdw::Vec3 albedo;
		sdw::Vec3 specular;
		sdw::Float glossiness;
	};

	class PbrMRMaterials
		: public Materials
	{
	public:
		C3D_API explicit PbrMRMaterials( sdw::ShaderWriter & writer );
		C3D_API void declare( bool hasSsbo
			, uint32_t binding
			, uint32_t set )override;
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
		C3D_API void declare( bool hasSsbo
			, uint32_t binding
			, uint32_t set )override;
		C3D_API SpecularGlossinessMaterial getMaterial( sdw::UInt const & index )const;
		C3D_API BaseMaterialUPtr getBaseMaterial( sdw::UInt const & index )const override;

	private:
		std::unique_ptr< sdw::ArraySsboT< SpecularGlossinessMaterial > > m_ssbo;
		sdw::Function< SpecularGlossinessMaterial, sdw::InUInt > m_getMaterial;
	};
}

#endif
