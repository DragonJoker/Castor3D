/*
See LICENSE file in root folder
*/
#ifndef ___C3D_LpvConfigUbo_H___
#define ___C3D_LpvConfigUbo_H___

#include "UbosModule.hpp"

#include "Castor3D/Buffer/UniformBuffer.hpp"

namespace castor3d
{
	class LpvConfigUbo
	{
	public:
		using Configuration = LpvConfigUboConfiguration;

	public:
		/**
		 *\~english
		 *\name			Copy/Move construction/assignment operation.
		 *\~french
		 *\name			Constructeurs/Opérateurs d'affectation par copie/déplacement.
		 */
		/**@{*/
		C3D_API LpvConfigUbo( LpvConfigUbo const & ) = delete;
		C3D_API LpvConfigUbo & operator=( LpvConfigUbo const & ) = delete;
		C3D_API LpvConfigUbo( LpvConfigUbo && ) = default;
		C3D_API LpvConfigUbo & operator=( LpvConfigUbo && ) = delete;
		/**@}*/
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine	The engine.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine	Le moteur.
		 */
		explicit LpvConfigUbo( Engine & engine );
		/**
		 *\~english
		 *\brief		Updates UBO data.
		 *\~french
		 *\brief		Met à jour les données de l'UBO.
		 */
		C3D_API void update( castor::Grid const & grid
			, Light const & light
			, uint32_t cascadeIndex
			, float texelAreaModifier = 1.0f
			, float indirectAttenuation = 1.7f );
		/**
		 *\~english
		 *\brief		Updates UBO data.
		 *\~french
		 *\brief		Met à jour les données de l'UBO.
		 */
		C3D_API void update( castor::Grid const & grid
			, Light const & light
			, float texelAreaModifier = 1.0f
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
		C3D_API static const std::string LpvConfig;
		C3D_API static const std::string LightView;
		C3D_API static const std::string MinVolumeCorner;
		C3D_API static const std::string GridSizes;
		C3D_API static const std::string Config;

	private:
		Engine & m_engine;
		UniformBufferUPtr< Configuration > m_ubo;
	};
}

#define UBO_LPVCONFIG( writer, binding, set )\
	sdw::Ubo lpvConfig{ writer\
		, castor3d::LpvConfigUbo::LpvConfig\
		, binding\
		, set\
		, ast::type::MemoryLayout::eStd140 };\
	auto c3d_lightView = lpvConfig.declMember< Mat4 >( castor3d::LpvConfigUbo::LightView );\
	auto c3d_minVolumeCorner = lpvConfig.declMember< Vec4 >( castor3d::LpvConfigUbo::MinVolumeCorner );\
	auto c3d_gridSizes = lpvConfig.declMember< UVec4 >( castor3d::LpvConfigUbo::GridSizes );\
	auto c3d_lpvConfig = lpvConfig.declMember< Vec4 >( castor3d::LpvConfigUbo::Config );\
	lpvConfig.end()

#endif
