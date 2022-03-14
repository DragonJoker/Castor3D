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

			void updateShadowType( ShadowType type )
			{
				m_intensityFarPlane.w() = sdw::Float{ float( type ) };
			}

			C3D_API static ast::type::BaseStructPtr makeType( ast::type::TypesCache & cache );
			C3D_API static std::unique_ptr< sdw::Struct > declare( sdw::ShaderWriter & writer );

		private:
			friend class LightingModel;
			// Raw values
			sdw::Vec4 m_colourIndex;
			sdw::Vec4 m_intensityFarPlane;
			sdw::Vec4 m_volumetric;
			sdw::Vec4 m_shadowsOffsets;
			sdw::Vec4 m_shadowsVariances;

		public:
			// Specific values
			sdw::Vec3 colour;
			sdw::Vec2 intensity;
			sdw::Float farPlane;
			sdw::Int shadowType;
			sdw::Int index;
			sdw::UInt volumetricSteps;
			sdw::Float volumetricScattering;
			sdw::Vec2 rawShadowOffsets;
			sdw::Vec2 pcfShadowOffsets;
			sdw::Vec2 vsmShadowVariance;

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

			Light base;

		private:
			friend class LightingModel;
			// Raw values
			sdw::Vec4 m_directionCount;

		public:
			sdw::Vec4 splitDepths;
			sdw::Vec4 splitScales;
			sdw::Array< sdw::Mat4 > transforms;
			// Specific values
			sdw::Vec3 direction;
			sdw::UInt cascadeCount;

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

			C3D_API sdw::Float getAttenuationFactor( sdw::Float const & distance )const;

			C3D_API static ast::type::BaseStructPtr makeType( ast::type::TypesCache & cache );
			C3D_API static std::unique_ptr< sdw::Struct > declare( sdw::ShaderWriter & writer );

			Light base;

		private:
			friend class LightingModel;
			// Raw values
			sdw::Vec4 m_position4;
			sdw::Vec4 m_attenuation4;

		public:
			// SpecificValues
			sdw::Vec3 position;

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

			C3D_API sdw::Float getAttenuationFactor( sdw::Float const & distance )const;

			C3D_API static ast::type::BaseStructPtr makeType( ast::type::TypesCache & cache );
			C3D_API static std::unique_ptr< sdw::Struct > declare( sdw::ShaderWriter & writer );

			Light base;

		private:
			friend class LightingModel;
			// Raw values
			sdw::Vec4 m_position4;
			sdw::Vec4 m_attenuation4;
			sdw::Vec4 m_direction4;
			sdw::Vec4 m_exponentCutOff;

		public:
			sdw::Mat4 transform;
			// SpecificValues
			sdw::Vec3 position;
			sdw::Vec3 direction;
			sdw::Float exponent;
			sdw::Float cutOff;

		private:
			using sdw::StructInstance::getMember;
			using sdw::StructInstance::getMemberArray;
		};
	}
}

#endif
