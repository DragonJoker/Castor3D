/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RsmConfigUbo_H___
#define ___C3D_RsmConfigUbo_H___

#include "UbosModule.hpp"

#include "Castor3D/Render/Technique/Opaque/Lighting/LightingModule.hpp"

#include "Castor3D/Buffer/UniformBufferOffset.hpp"
#include "Castor3D/Scene/Light/LightModule.hpp"

namespace castor3d
{
	class RsmConfigUbo
	{
	public:
		using Configuration = RsmUboConfiguration;

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
		 *\param[in]	device	The GPU device.
		 *\~french
		 *\brief		Initialise l'UBO.
		 *\param[in]	device	Le device GPU.
		 */
		C3D_API void initialise( RenderDevice const & device );
		/**
		 *\~english
		 *\brief		Cleanup function.
		 *\param[in]	device	The GPU device.
		 *\~french
		 *\brief		Fonction de nettoyage.
		 *\param[in]	device	Le device GPU.
		 */
		C3D_API void cleanup( RenderDevice const & device );
		/**
		 *\~english
		 *\brief		Updates the UBO from given values.
		 *\param[in]	light	The light source.
		 *\~french
		 *\brief		Met à jour l'UBO avec les valeurs données.
		 *\param[in]	light	La source lumineuse.
		 */
		C3D_API void update( Light const & light );

		void createSizedBinding( ashes::DescriptorSet & descriptorSet
			, VkDescriptorSetLayoutBinding const & layoutBinding )const
		{
			return m_ubo.createSizedBinding( descriptorSet, layoutBinding );
		}

		UniformBufferOffsetT< Configuration > const & getUbo()const
		{
			return m_ubo;
		}

	public:
		C3D_API static std::string const BufferRsmConfig;
		C3D_API static std::string const Intensity;
		C3D_API static std::string const RMax;
		C3D_API static std::string const SampleCount;
		C3D_API static std::string const Index;

	private:
		Engine & m_engine;
		UniformBufferOffsetT< Configuration > m_ubo;
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
