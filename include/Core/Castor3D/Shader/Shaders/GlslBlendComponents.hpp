/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslBlendComponents_H___
#define ___C3D_GlslBlendComponents_H___

#include "Castor3D/Material/Pass/Component/ComponentModule.hpp"
#include "Castor3D/Shader/Shaders/SdwModule.hpp"

#include "Castor3D/Material/Pass/Component/PassComponent.hpp"

#include <ShaderWriter/BaseTypes/Float.hpp>
#include <ShaderWriter/CompositeTypes/StructInstance.hpp>
#include <ShaderWriter/MatTypes/Mat4.hpp>

#pragma warning( push )
#pragma warning( disable:4251 )
#pragma warning( disable:4275 )

namespace c3d::shader
{
	template< typename T >
	struct BlendComponentT
	{
		explicit BlendComponentT( T v, bool e = true )
			: value{ c3d::move( v ) }
			, enabled{ e }
		{
		}

		T value;
		bool enabled;
	};

	struct BlendComponents
		: sdw::StructInstance
	{
		SDW_DeclStructInstance( C3D_INL_API, BlendComponents );

		C3D_API BlendComponents( sdw::ShaderWriter & writer
			, sdw::expr::ExprPtr expr
			, bool enabled );
		C3D_API BlendComponents( Materials const & materials
			, Material const & material
			, SurfaceBase const & surface );
		C3D_API BlendComponents( Materials const & materials
			, Material const & material
			, DerivSurfaceBase const & surface );
		C3D_API BlendComponents( Materials const & materials
			, Material const & material
			, SurfaceBase const & surface
			, sdw::Vec4 const & clrCot );
		C3D_API BlendComponents( Materials const & materials
			, Material const & material
			, DerivSurfaceBase const & surface
			, sdw::Vec4 const & clrCot );
		C3D_API explicit BlendComponents( Materials const & materials
			, bool zeroInit = false );

		C3D_API void finish( PassShaders const & passShaders
			, DerivSurfaceBase const & surface
			, CameraData const & camera
			, ModelData const & model
			, Utils & utils );

		C3D_API void registerDebug( DebugOutputCategory const & debugOutput )const;

		C3D_API void setNormal( sdw::Vec3 const v );
		C3D_API void normalizeNormal();
		C3D_API sdw::Vec3 getRawNormal()const;
		C3D_API sdw::Vec4 getRawTangent()const;
		C3D_API sdw::Vec3 getRawBitangent()const;
		C3D_API shader::DerivVec3 getDerivNormal()const;
		C3D_API shader::DerivVec4 getDerivTangent()const;
		C3D_API shader::DerivVec3 getDerivBitangent()const;

		bool usesDerivativeValues()const noexcept
		{
			return m_derivativeValues;
		}

		C3D_API static ast::type::BaseStructPtr makeType( ast::type::TypesCache & cache
			, Materials const & materials
			, bool zeroInit
			, sdw::expr::ExprList & inits );
		C3D_API static ast::type::BaseStructPtr makeType( ast::type::TypesCache & cache
			, Materials const & materials
			, Material const & material
			, sdw::StructInstance const & surface
			, sdw::Vec4 const * clrCot
			, sdw::expr::ExprList & inits );
		C3D_API static ast::type::BaseStructPtr makeType( ast::type::TypesCache & cache
			, BlendComponents const & rhs );

		C3D_API static ast::type::BaseStructPtr makeType( ast::type::TypesCache & cache
			, Materials const & materials
			, bool zeroInit = false )
		{
			sdw::expr::ExprList inits;
			return makeType( cache, materials, zeroInit, inits );
		}

		static ast::type::BaseStructPtr makeType( ast::type::TypesCache & cache
			, Materials const & materials
			, Material const & material
			, sdw::StructInstance const & surface
			, sdw::Vec4 const * clrCot )
		{
			sdw::expr::ExprList inits;
			return makeType( cache, materials, material, surface, clrCot, inits );
		}

		C3D_API static sdw::Float computeRoughnessFromGlossiness( sdw::Float const & glossiness );
		C3D_API static sdw::Float computeGlossinessFromRoughness( sdw::Float const & roughness );
		C3D_API static sdw::Float computeGlossinessFromShininess( sdw::Float const & shininess );
		C3D_API static sdw::Float computeShininessFromGlossiness( sdw::Float const & glossiness );

		static sdw::Float computeRoughnessFromShininess( sdw::Float const & shininess )
		{
			return computeRoughnessFromGlossiness( computeGlossinessFromShininess( shininess ) );
		}

		static sdw::Float computeShininessFromRoughness( sdw::Float const & roughness )
		{
			return computeShininessFromGlossiness( computeGlossinessFromRoughness( roughness ) );
		}

	public:
		sdw::DefaultedT< sdw::Float > ior;
		sdw::DefaultedT< sdw::Float > perceptualRoughness;
		sdw::DefaultedT< sdw::Vec3 > dielectricF0;
		sdw::DefaultedT< sdw::Float > alphaRoughness;
		sdw::DefaultedT< sdw::Vec3 > f90;
		sdw::DefaultedT< sdw::Vec3 > dielectricF90;
		sdw::DefaultedT< sdw::Float > metalness;
		sdw::DefaultedT< sdw::Vec3 > baseColour;

		sdw::DefaultedT< sdw::Vec3 > sheenColour;
		sdw::DefaultedT< sdw::Float > sheenRoughness;

		sdw::DefaultedT< sdw::Vec3 > clearcoatF0;
		sdw::DefaultedT< sdw::Vec3 > clearcoatF90;
		sdw::DefaultedT< sdw::Float > clearcoatFactor;
		sdw::DefaultedT< sdw::Vec3 > clearcoatNormal;
		sdw::DefaultedT< sdw::Float > clearcoatRoughness;

		sdw::DefaultedT< sdw::Float > specularWeight;

		sdw::DefaultedT< sdw::Float > transmissionFactor;

		sdw::DefaultedT< sdw::Float > thicknessFactor;
		sdw::DefaultedT< sdw::Vec3 > attenuationColour;
		sdw::DefaultedT< sdw::Float > attenuationDistance;

		sdw::DefaultedT< sdw::Float > iridescenceFactor;
		sdw::DefaultedT< sdw::Float > iridescenceThickness;
		sdw::DefaultedT< sdw::Float > iridescenceIor;

		sdw::DefaultedT< sdw::Vec3 > diffuseTransmissionColour;
		sdw::DefaultedT< sdw::Float > diffuseTransmissionFactor;

		sdw::DefaultedT< sdw::Float > dispersion;

		sdw::DefaultedT< sdw::Vec3 > emissiveColour;
		sdw::DefaultedT< sdw::Float > emissiveFactor;

		sdw::DefaultedT< sdw::Float > opacity;
		sdw::DefaultedT< sdw::UInt > bwAccumulationOperator;
		sdw::DefaultedT< sdw::Float > alphaRef;
		sdw::DefaultedT< sdw::Float > occlusion;
		sdw::DefaultedT< sdw::Float > transmittance;
		sdw::DefaultedT< sdw::UInt > hasReflection;

	protected:
		C3D_API static void fillType( ast::type::BaseStruct & type
			, Materials const & materials
			, sdw::expr::ExprList & inits );
		C3D_API static void fillType( ast::type::BaseStruct & type
			, Materials const & materials
			, Material const & material
			, sdw::StructInstance const & surface
			, sdw::Vec4 const * clrCot
			, sdw::expr::ExprList & inits );
		C3D_API static void fillInit( sdw::type::BaseStruct const & components
			, Materials const & materials
			, sdw::expr::ExprList & inits );
		C3D_API static void fillInit( sdw::type::BaseStruct const & components
			, Materials const & materials
			, Material const & material
			, sdw::StructInstance const & surface
			, sdw::Vec4 const * clrCot
			, sdw::expr::ExprList & inits );
		C3D_API static sdw::expr::ExprPtr makeInit( Materials const & materials
			, bool zeroInit );
		C3D_API static sdw::expr::ExprPtr makeInit( Materials const & materials
			, Material const & material
			, sdw::StructInstance const & surface
			, sdw::Vec4 const * clrCot );

	private:
		bool m_derivativeValues{};
	};
}

#pragma warning( pop )
#endif
