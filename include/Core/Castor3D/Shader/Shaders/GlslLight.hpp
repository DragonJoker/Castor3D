/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslLight_H___
#define ___C3D_GlslLight_H___

#include "SdwModule.hpp"

#include <ShaderWriter/MatTypes/Mat4.hpp>
#include <ShaderWriter/CompositeTypes/StructInstance.hpp>

namespace castor3d
{
	namespace shader
	{
		struct LightData
			: public sdw::StructInstance
		{
			C3D_API LightData( sdw::ShaderWriter & writer
				, ast::expr::ExprPtr expr
				, bool enabled );
			SDW_DeclStructInstance( C3D_API, LightData );

			C3D_API static ast::type::BaseStructPtr makeType( ast::type::TypesCache & cache );

			sdw::Array< sdw::Vec4 > data;
		};

		struct Light
			: public sdw::StructInstance
		{
			C3D_API Light( sdw::ShaderWriter & writer
				, ast::expr::ExprPtr expr
				, bool enabled );
			SDW_DeclStructInstance( C3D_API, Light );

			C3D_API static ast::type::BaseStructPtr makeType( ast::type::TypesCache & cache );
			C3D_API static std::unique_ptr< sdw::Struct > declare( sdw::ShaderWriter & writer );

			// Raw values
			sdw::Vec4 m_colourIndex;
			sdw::Vec4 m_intensityFarPlane;
			sdw::Vec4 m_volumetric;
			sdw::Vec4 m_shadowsOffsets;
			sdw::Vec4 m_shadowsVariances;
			// Specific values
			sdw::Vec3 m_colour;
			sdw::Vec2 m_intensity;
			sdw::Float m_farPlane;
			sdw::Int m_shadowType;
			sdw::Int m_index;
			sdw::UInt m_volumetricSteps;
			sdw::Float m_volumetricScattering;
			sdw::Vec2 m_rawShadowOffsets;
			sdw::Vec2 m_pcfShadowOffsets;
			sdw::Vec2 m_vsmShadowVariance;

		private:
			using sdw::StructInstance::getMember;
			using sdw::StructInstance::getMemberArray;
		};

		struct DirectionalLight
			: public sdw::StructInstance
		{
			C3D_API DirectionalLight( sdw::ShaderWriter & writer
				, ast::expr::ExprPtr expr
				, bool enabled );
			SDW_DeclStructInstance( C3D_API, DirectionalLight );

			C3D_API static ast::type::BaseStructPtr makeType( ast::type::TypesCache & cache );
			C3D_API static std::unique_ptr< sdw::Struct > declare( sdw::ShaderWriter & writer );

			// Raw values
			Light m_lightBase;
			sdw::Vec4 m_directionCount;
			sdw::Vec4 m_splitDepths;
			sdw::Vec4 m_splitScales;
			sdw::Array< sdw::Mat4 > m_transforms;
			// Specific values
			sdw::Vec3 m_direction;
			sdw::UInt m_cascadeCount;

		private:
			using sdw::StructInstance::getMember;
			using sdw::StructInstance::getMemberArray;
		};

		struct TiledDirectionalLight
			: public sdw::StructInstance
		{
			C3D_API TiledDirectionalLight( sdw::ShaderWriter & writer
				, ast::expr::ExprPtr expr
				, bool enabled );
			SDW_DeclStructInstance( C3D_API, TiledDirectionalLight );

			C3D_API static ast::type::BaseStructPtr makeType( ast::type::TypesCache & cache );
			C3D_API static std::unique_ptr< sdw::Struct > declare( sdw::ShaderWriter & writer );

			// Raw values
			Light m_lightBase;
			sdw::Vec4 m_directionCount;
			sdw::Vec4 m_tiles;
			sdw::Array< sdw::Vec4 > m_splitDepths;
			sdw::Array< sdw::Vec4 > m_splitScales;
			sdw::Array< sdw::Mat4 > m_transforms;
			// Specific values
			sdw::Vec3 m_direction;
			sdw::UInt m_cascadeCount;

		private:
			using sdw::StructInstance::getMember;
			using sdw::StructInstance::getMemberArray;
		};

		struct PointLight
			: public sdw::StructInstance
		{
			C3D_API PointLight( sdw::ShaderWriter & writer
				, ast::expr::ExprPtr expr
				, bool enabled );
			SDW_DeclStructInstance( C3D_API, PointLight );

			C3D_API static ast::type::BaseStructPtr makeType( ast::type::TypesCache & cache );
			C3D_API static std::unique_ptr< sdw::Struct > declare( sdw::ShaderWriter & writer );

			// Raw values
			Light m_lightBase;
			sdw::Vec4 m_position4;
			sdw::Vec4 m_attenuation4;
			// SpecificValues
			sdw::Vec3 m_position;
			sdw::Vec3 m_attenuation;

		private:
			using sdw::StructInstance::getMember;
			using sdw::StructInstance::getMemberArray;
		};

		struct SpotLight
			: public sdw::StructInstance
		{
			C3D_API SpotLight( sdw::ShaderWriter & writer
				, ast::expr::ExprPtr expr
				, bool enabled );
			SDW_DeclStructInstance( C3D_API, SpotLight );

			C3D_API static ast::type::BaseStructPtr makeType( ast::type::TypesCache & cache );
			C3D_API static std::unique_ptr< sdw::Struct > declare( sdw::ShaderWriter & writer );

			// Raw values
			Light m_lightBase;
			sdw::Vec4 m_position4;
			sdw::Vec4 m_attenuation4;
			sdw::Vec4 m_direction4;
			sdw::Vec4 m_exponentCutOff;
			sdw::Mat4 m_transform;
			// SpecificValues
			sdw::Vec3 m_position;
			sdw::Vec3 m_attenuation;
			sdw::Vec3 m_direction;
			sdw::Float m_exponent;
			sdw::Float m_cutOff;

		private:
			using sdw::StructInstance::getMember;
			using sdw::StructInstance::getMemberArray;
		};
	}
}

#endif
