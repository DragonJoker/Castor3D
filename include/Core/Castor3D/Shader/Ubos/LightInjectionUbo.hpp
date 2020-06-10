/*
See LICENSE file in root folder
*/
#ifndef ___C3D_LightInjectionUbo_H___
#define ___C3D_LightInjectionUbo_H___

#include "UbosModule.hpp"

#include "Castor3D/Buffer/UniformBuffer.hpp"

namespace castor3d
{
	class LightInjectionUbo
	{
	public:
		using Configuration = LightInjectionUboConfiguration;

	public:
		/**
		 *\~english
		 *\name			Copy/Move construction/assignment operation.
		 *\~french
		 *\name			Constructeurs/Opérateurs d'affectation par copie/déplacement.
		 */
		/**@{*/
		C3D_API LightInjectionUbo( LightInjectionUbo const & ) = delete;
		C3D_API LightInjectionUbo & operator=( LightInjectionUbo const & ) = delete;
		C3D_API LightInjectionUbo( LightInjectionUbo && ) = default;
		C3D_API LightInjectionUbo & operator=( LightInjectionUbo && ) = delete;
		/**@}*/
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine	The engine.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine	Le moteur.
		 */
		explicit LightInjectionUbo( Engine & engine );
		/**
		 *\~english
		 *\brief		Updates UBO data.
		 *\~french
		 *\brief		Met à jour les données de l'UBO.
		 */
		C3D_API void update( castor::Point3f const & minVolumeCorner
			, castor::Point3ui const & gridSize
			, uint32_t lightIndex
			, float cellSize
			, float indirectAttenuation = 1.7f );
		/**
		 *\~english
		 *\name			getters.
		 *\~french
		 *\name			getters.
		 */
		inline UniformBuffer< Configuration > & getUbo()
		{
			return *m_ubo;
		}

		inline UniformBuffer< Configuration > const & getUbo()const
		{
			return *m_ubo;
		}
		/**@}*/

	public:
		C3D_API static const std::string LightInjection;
		C3D_API static const std::string MinVolumeCorner;
		C3D_API static const std::string GridSizes;
		C3D_API static const std::string Config;

	private:
		Engine & m_engine;
		UniformBufferUPtr< Configuration > m_ubo;
	};
}

#define UBO_LIGHTINJECTION( writer, binding, set )\
	sdw::Ubo billboard{ writer\
		, castor3d::LightInjectionUbo::LightInjection\
		, binding\
		, set\
		, ast::type::MemoryLayout::eStd140 };\
	auto c3d_minVolumeCorner = billboard.declMember< Vec4 >( castor3d::LightInjectionUbo::MinVolumeCorner );\
	auto c3d_gridSizes = billboard.declMember< UVec4 >( castor3d::LightInjectionUbo::GridSizes );\
	auto c3d_config = billboard.declMember< Vec4 >( castor3d::LightInjectionUbo::Config );\
	billboard.end()

#endif
