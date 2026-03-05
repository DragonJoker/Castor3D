/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RenderUbo_H___
#define ___C3D_RenderUbo_H___

#include "Castor3D/Render/ToneMapping/HdrConfig.hpp"

#include "Castor3D/Shader/Ubos/Ubo.hpp"

#include <ShaderWriter/CompositeTypes/StructInstanceHelper.hpp>
#include <ShaderWriter/VecTypes/Vec4.hpp>

namespace c3d
{
	namespace shader
	{
		struct RenderData
			: public sdw::StructInstanceHelperT< "C3D_RenderData"
			, ast::type::MemoryLayout::eStd140
			, sdw::U32Vec2Field< "renderSize" >
			, sdw::Vec2Field< "invRenderSize" >
			, sdw::FloatField< "gamma" >
			, sdw::FloatField< "exposure" >
			, sdw::UInt32Field< "debugIndex" >
			, sdw::FloatField< "pad" > >
		{
			SDW_DeclStructInstance( C3D_INL_API, RenderData );

			RenderData( sdw::ShaderWriter & writer
				, ast::expr::ExprPtr expr
				, bool enabled )
				: StructInstanceHelperT{ writer, c3d::move( expr ), enabled }
			{
			}

			C3D_API sdw::Vec3 removeGamma( sdw::Vec3 const & srgb )const;
			C3D_API sdw::Vec3 applyGamma( sdw::Vec3 const & hdr )const;
			C3D_API sdw::Vec2 calcTexCoord( Utils & utils
				, sdw::Vec2 const & fragCoord )const;

			auto renderSize()const { return getMember< "renderSize" >(); }
			auto invRenderSize()const { return getMember< "invRenderSize" >(); }
			auto gamma()const { return getMember< "gamma" >(); }
			auto exposure()const { return getMember< "exposure" >(); }
			auto debugIndex()const { return getMember< "debugIndex" >(); }
		};
	}

	class RenderUbo
		: public UboT< RenderUboConfiguration >
	{
	public:
		using Configuration = RenderUboConfiguration;

	public:
		C3D_API explicit RenderUbo( RenderDevice const & device );
		/**
		 *\~english
		 *\brief		Updates the UBO from given values.
		 *\param[in]	parent		The UBO from which configuration is copied.
		 *\param[in]	renderSize	The render size.
		 *\~french
		 *\brief		Met à jour l'UBO avec les valeurs données.
		 *\param[in]	parent		L'UBO depuis lequel la configuration est copiée.
		 *\param[in]	renderSize	Les dimensions du rendu.
		 */
		C3D_API void cpuUpdate( RenderUbo const & parent
			, Size const & renderSize );
		/**
		 *\~english
		 *\brief		Updates the UBO from given values.
		 *\param[in]	hdrConfig	The HDR configuration.
		 *\param[in]	renderSize	The render size.
		 *\param[in]	debugIndex	The currently selected debug data index.
		 *\~french
		 *\brief		Met à jour l'UBO avec les valeurs données.
		 *\param[in]	hdrConfig	La configuration HDR.
		 *\param[in]	renderSize	Les dimensions du rendu.
		 *\param[in]	debugIndex	L'indice des données de debug actuellement sélectionnées.
		 */
		C3D_API void cpuUpdate( HdrConfig const & hdrConfig
			, Size const & renderSize, bool safeBanded
			, uint32_t debugIndex );
	};
}

#define C3D_Render( writer, binding, set )\
	sdw::UniformBuffer renderConfig{ writer\
		, "C3D_Render"\
		, "c3d_renderConfig"\
		, uint32_t( binding )\
		, uint32_t( set )\
		, ast::type::MemoryLayout::eStd140\
		, true };\
	auto c3d_renderData = renderConfig.declMember< c3d::shader::RenderData >( "d" );\
	renderConfig.end()

#endif
