/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslMaterials_H___
#define ___C3D_GlslMaterials_H___

#include "SdwModule.hpp"

#include <ShaderWriter/MatTypes/Mat4.hpp>
#include <ShaderWriter/CompositeTypes/StructInstance.hpp>

namespace castor3d::shader
{
	castor::String const PassBufferName = cuT( "Materials" );

	struct Material
		: public sdw::StructInstance
	{
		friend class Materials;
		C3D_API Material( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled );
		SDW_DeclStructInstance( C3D_API, Material );

		C3D_API sdw::Vec3 colour()const;

		C3D_API static ast::type::BaseStructPtr makeType( ast::type::TypesCache & cache );

	protected:
		using sdw::StructInstance::getMember;
		using sdw::StructInstance::getMemberArray;

	public:
		sdw::Vec4 colourDiv;
		sdw::Vec4 specDiv;
		sdw::Float edgeWidth;
		sdw::Float depthFactor;
		sdw::Float normalFactor;
		sdw::Float objectFactor;
		sdw::Vec4 edgeColour;
		sdw::Vec4 specific;
		sdw::UInt index;
		sdw::Float emissive;
		sdw::Float alphaRef;
		sdw::UInt sssProfileIndex;
		sdw::Vec3 transmission;
		sdw::Float opacity;
		sdw::Float refractionRatio;
		sdw::Int hasRefraction;
		sdw::Int hasReflection;
		sdw::Float bwAccumulationOperator;
	};

	CU_DeclareSmartPtr( Material );

	class Materials
	{
	public:
		C3D_API explicit Materials( sdw::ShaderWriter & writer );
		C3D_API explicit Materials( sdw::ShaderWriter & writer
			, uint32_t binding
			, uint32_t set
			, bool enable = true );
		C3D_API void declare( uint32_t binding
			, uint32_t set );
		C3D_API Material getMaterial( sdw::UInt const & index )const;

	protected:
		sdw::ShaderWriter & m_writer;
		std::unique_ptr< sdw::ArraySsboT< Material > > m_ssbo;
	};

	struct SssProfile
		: public sdw::StructInstance
	{
		friend class SssProfiles;
		C3D_API SssProfile( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled );
		SDW_DeclStructInstance( C3D_API, SssProfile );

		C3D_API static ast::type::BaseStructPtr makeType( ast::type::TypesCache & cache );

	protected:
		using sdw::StructInstance::getMember;
		using sdw::StructInstance::getMemberArray;

	public:
		sdw::Int transmittanceProfileSize;
		sdw::Float gaussianWidth;
		sdw::Float subsurfaceScatteringStrength;
		sdw::Float pad;
		sdw::Array< sdw::Vec4 > transmittanceProfile;
	};

	CU_DeclareSmartPtr( SssProfile );

	class SssProfiles
	{
	public:
		C3D_API explicit SssProfiles( sdw::ShaderWriter & writer );
		C3D_API explicit SssProfiles( sdw::ShaderWriter & writer
			, uint32_t binding
			, uint32_t set
			, bool enable = true );
		C3D_API void declare( uint32_t binding
			, uint32_t set );
		C3D_API SssProfile getProfile( sdw::UInt const & index )const;

	protected:
		sdw::ShaderWriter & m_writer;
		std::unique_ptr< sdw::ArraySsboT< SssProfile > > m_ssbo;
	};
}

#endif
