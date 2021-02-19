/*
See LICENSE file in root folder
*/
#ifndef ___C3D_HdrConfigUbo_H___
#define ___C3D_HdrConfigUbo_H___

#include "UbosModule.hpp"

#include "Castor3D/Render/ToneMapping/HdrConfig.hpp"

#include "Castor3D/Buffer/UniformBufferOffset.hpp"

namespace castor3d
{
	class HdrConfigUbo
	{
	public:
		using Configuration = HdrConfig;

	public:
		/**
		 *\~english
		 *\name			Copy/Move construction/assignment operation.
		 *\~french
		 *\name			Constructeurs/Opérateurs d'affectation par copie/déplacement.
		 */
		/**@{*/
		C3D_API HdrConfigUbo( HdrConfigUbo const & ) = delete;
		C3D_API HdrConfigUbo & operator=( HdrConfigUbo const & ) = delete;
		C3D_API HdrConfigUbo( HdrConfigUbo && ) = default;
		C3D_API HdrConfigUbo & operator=( HdrConfigUbo && ) = delete;
		/**@}*/
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine	The engine.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine	Le moteur.
		 */
		C3D_API explicit HdrConfigUbo( Engine & engine );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~HdrConfigUbo();
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
		 *\param[in]	config	The HDR configuration.
		 *\~french
		 *\brief		Met à jour l'UBO avec les valeurs données.
		 *\param[in]	config	La configuration HDR.
		 */
		C3D_API void cpuUpdate( HdrConfig const & config );

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
		C3D_API static uint32_t const BindingPoint;
		//!\~english	Name of the HDR configuration frame variable buffer.
		//!\~french		Nom du frame variable buffer contenant la configuration du HDR.
		C3D_API static castor::String const BufferHdrConfig;
		//!\~english	Name of the exposure frame variable.
		//!\~french		Nom de la frame variable contenant l'exposition.
		C3D_API static castor::String const Exposure;
		//!\~english	Name of the gamma correction frame variable.
		//!\~french		Nom de la frame variable contenant la correction gamma.
		C3D_API static castor::String const Gamma;

	private:
		Engine & m_engine;
		UniformBufferOffsetT< HdrConfig > m_ubo;
	};
}

#define UBO_HDR_CONFIG( writer, binding, set )\
	Ubo hdrConfig{ writer\
		, castor3d::HdrConfigUbo::BufferHdrConfig\
		, binding\
		, set };\
	auto c3d_exposure = hdrConfig.declMember< Float >( castor3d::HdrConfigUbo::Exposure );\
	auto c3d_gamma = hdrConfig.declMember< Float >( castor3d::HdrConfigUbo::Gamma );\
	hdrConfig.end()

#endif
