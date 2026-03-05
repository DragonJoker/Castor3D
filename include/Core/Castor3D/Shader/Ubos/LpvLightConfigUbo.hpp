/*
See LICENSE file in root folder
*/
#ifndef ___C3D_LpvLightConfigUbo_H___
#define ___C3D_LpvLightConfigUbo_H___

#include "Castor3D/Scene/Light/LightModule.hpp"

#include "Castor3D/Shader/Ubos/Ubo.hpp"

#include <CastorUtils/Graphics/GraphicsModule.hpp>

#include <ShaderWriter/CompositeTypes/StructInstanceHelper.hpp>
#include <ShaderWriter/MatTypes/Mat4.hpp>

namespace c3d
{
	namespace shader
	{
		struct LpvLightData
			: public sdw::StructInstanceHelperT< "C3D_LpvLightData"
				, sdw::type::MemoryLayout::eStd140
				, sdw::Mat4x4Field< "lightView" >
				, sdw::FloatField< "texelAreaModifier" >
				, sdw::FloatField< "tanFovXHalf" >
				, sdw::FloatField< "tanFovYHalf" >
				, sdw::IntField< "lightOffset" > >
		{
			SDW_DeclStructInstance( C3D_INL_API, LpvLightData );

			C3D_API LpvLightData( sdw::ShaderWriter & writer
				, ast::expr::ExprPtr expr
				, bool enabled )
				: StructInstanceHelperT{ writer, c3d::move( expr ), enabled }
			{
			}

			auto lightView()const { return getMember< "lightView" >(); }
			auto texelAreaModifier()const { return getMember< "texelAreaModifier" >(); }
			auto tanFovXHalf()const { return getMember< "tanFovXHalf" >(); }
			auto tanFovYHalf()const { return getMember< "tanFovYHalf" >(); }
			auto lightOffset()const { return getMember< "lightOffset" >(); }
		};
	}

	class LpvLightConfigUbo
		: public UboT< LpvLightConfigUboConfiguration >
	{
	public:
		using Configuration = LpvLightConfigUboConfiguration;

	public:
		C3D_API explicit LpvLightConfigUbo( RenderDevice const & device );

		C3D_API void cpuUpdate( LightInstance const & light
			, float lpvCellSize
			, uint32_t faceIndex );
		C3D_API void cpuUpdate( DirectionalLightInstance const & light
			, uint32_t cascadeIndex
			, float lpvCellSize );
	};
}

#define C3D_LpvLightConfig( writer, binding, set )\
	sdw::UniformBuffer lpvLightConfig{ writer\
		, "C3D_LpvLightConfig"\
		, "c3d_lpvLightConfig"\
		, uint32_t( binding )\
		, uint32_t( set )\
		, ast::type::MemoryLayout::eStd140 };\
	auto c3d_lpvLightData = lpvLightConfig.declMember< c3d::shader::LpvLightData >( "d" );\
	lpvLightConfig.end()

#endif
