/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RsmConfigUbo_H___
#define ___C3D_RsmConfigUbo_H___

#include "UbosModule.hpp"

#include "Castor3D/Render/Technique/Opaque/Lighting/LightingModule.hpp"

#include "Castor3D/Buffer/UniformBufferPool.hpp"
#include "Castor3D/Scene/Light/LightModule.hpp"

namespace castor3d
{
	class RsmConfigUbo
	{
	public:
		/**
		 *\~english
		 *\name			Copy/Move construction/assignment operation.
		 *\~french
		 *\name			Constructeurs/Opérateurs d'affectation par copie/déplacement.
		 */
		/**@{*/
		C3D_API RsmConfigUbo( RsmConfigUbo const & ) = delete;
		C3D_API RsmConfigUbo & operator=( RsmConfigUbo const & ) = delete;
		C3D_API RsmConfigUbo( RsmConfigUbo && ) = default;
		C3D_API RsmConfigUbo & operator=( RsmConfigUbo && ) = delete;
		/**@}*/
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine	The engine.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine	Le moteur.
		 */
		C3D_API explicit RsmConfigUbo( Engine & engine );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~RsmConfigUbo();
		/**
		 *\~english
		 *\brief		Initialises the UBO.
		 *\~french
		 *\brief		Initialise l'UBO.
		 */
		C3D_API void initialise();
		/**
		 *\~english
		 *\brief		Cleanup function.
		 *\~french
		 *\brief		Fonction de nettoyage.
		 */
		C3D_API void cleanup();
		/**
		 *\~english
		 *\brief		Updates the UBO from given values.
		 *\~french
		 *\brief		Met à jour l'UBO avec les valeurs données.
		 */
		C3D_API void update( Light const & light );
		/**
		 *\~english
		 *\name			getters.
		 *\~french
		 *\name			getters.
		 */
		/**@{*/
		inline UniformBufferOffsetT< RsmUboConfiguration > & getUbo()
		{
			return m_ubo;
		}

		inline UniformBufferOffsetT< RsmUboConfiguration > const & getUbo()const
		{
			return m_ubo;
		}
		/**@}*/

	public:
		C3D_API static std::string const BufferRsmConfig;
		C3D_API static std::string const Intensity;
		C3D_API static std::string const RMax;
		C3D_API static std::string const SampleCount;
		C3D_API static std::string const Index;

	private:
		Engine & m_engine;
		UniformBufferOffsetT< RsmUboConfiguration > m_ubo;
	};
}

#define UBO_RSM_CONFIG( writer, binding, set )\
	sdw::Ubo rsmConfig{ writer\
		, castor3d::RsmConfigUbo::BufferRsmConfig\
		, binding\
		, set };\
	auto c3d_rsmIntensity = rsmConfig.declMember< sdw::Float >( castor3d::RsmConfigUbo::Intensity );\
	auto c3d_rsmRMax = rsmConfig.declMember< sdw::Float >( castor3d::RsmConfigUbo::RMax );\
	auto c3d_rsmSampleCount = rsmConfig.declMember< sdw::UInt >( castor3d::RsmConfigUbo::SampleCount );\
	auto c3d_rsmIndex = rsmConfig.declMember< sdw::UInt >( castor3d::RsmConfigUbo::Index );\
	rsmConfig.end()

#endif
