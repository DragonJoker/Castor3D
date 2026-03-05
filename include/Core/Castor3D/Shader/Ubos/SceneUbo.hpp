/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SceneUbo_H___
#define ___C3D_SceneUbo_H___

#include "Castor3D/Shader/Shaders/SdwModule.hpp"

#include "Castor3D/Shader/Ubos/Ubo.hpp"

#include <ShaderWriter/CompositeTypes/StructInstanceHelper.hpp>
#include <ShaderWriter/VecTypes/Vec4.hpp>

namespace c3d
{
	namespace shader
	{
		struct SceneData
			: public sdw::StructInstanceHelperT< "C3D_SceneData"
				, ast::type::MemoryLayout::eStd140
				, sdw::Vec3Field< "ambientLight" >
				, sdw::UInt32Field< "fogType" >
				, sdw::Vec3Field< "backgroundColour" >
				, sdw::FloatField< "fogDensity" > >
		{
			friend struct BillboardData;
			friend class Fog;
			friend class CommonFog;

			SDW_DeclStructInstance( C3D_INL_API, SceneData );

			SceneData( sdw::ShaderWriter & writer
				, ast::expr::ExprPtr expr
				, bool enabled )
				: StructInstanceHelperT{ writer, c3d::move( expr ), enabled }
			{
			}

			C3D_API sdw::Vec4 getBackgroundColour( Utils & utils
				, sdw::Float const gamma )const;
			C3D_API sdw::Vec4 getBackgroundColour( RenderData const & renderData )const;
			C3D_API sdw::Vec4 computeAccumulation( Utils & utils
				, CameraData const & camera
				, sdw::Float const & depth
				, sdw::Vec3 const & colour
				, sdw::Float const & alpha
				, sdw::UInt const & accumulationOperator )const;

			auto ambientLight()const { return getMember< "ambientLight" >(); }
			auto backgroundColour()const { return getMember< "backgroundColour" >(); }
			auto fogType()const { return getMember< "fogType" >(); }
			auto fogDensity()const { return getMember< "fogDensity" >(); }
		};
	}

	class SceneUbo
		: public UboT< SceneUboConfiguration >
	{
	public:
		using Configuration = SceneUboConfiguration;

	public:
		C3D_API explicit SceneUbo( RenderDevice const & device );
		/**
		 *\~english
		 *\brief		Updates the UBO from given values.
		 *\param[in]	fog		The fog configuration.
		 *\~french
		 *\brief		Met à jour l'UBO avec les valeurs données.
		 *\param[in]	fog		La configuration du brouillard.
		 */
		C3D_API Configuration & cpuUpdate( Fog const & fog );
		/**
		 *\~english
		 *\brief		Updates the UBO from given values.
		 *\param[in]	scene	The rendered scene.
		 *\~french
		 *\brief		Met à jour l'UBO avec les valeurs données.
		 *\param[in]	scene	La scène dessinée.
		 */
		C3D_API Configuration & cpuUpdate( Scene const & scene );
	};
}

#define C3D_Scene( writer, binding, set )\
	sdw::UniformBuffer sceneBuffer{ writer\
		, "C3D_Scene"\
		, "c3d_scene"\
		, uint32_t( binding )\
		, uint32_t( set )\
		, ast::type::MemoryLayout::eStd140\
		, true };\
	auto c3d_sceneData = sceneBuffer.declMember< c3d::shader::SceneData >( "d" );\
	sceneBuffer.end()

#endif
