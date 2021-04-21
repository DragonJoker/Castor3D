/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ModelInstancesUbo_H___
#define ___C3D_ModelInstancesUbo_H___

#include "UbosModule.hpp"

#include "Castor3D/Buffer/UniformBufferOffset.hpp"
#include "Castor3D/Render/RenderModule.hpp"

#include <CastorUtils/Math/SquareMatrix.hpp>

namespace castor3d
{
	class ModelInstancesUbo
	{
	public:
		using Configuration = ModelInstancesUboConfiguration;
		/**
		*\~english
		*\name
		*	Copy/Move construction/assignment operation.
		*\~french
		*\name
		*	Constructeurs/Opérateurs d'affectation par copie/déplacement.
		*/
		/**@{*/
		C3D_API ModelInstancesUbo( ModelInstancesUbo const & ) = delete;
		C3D_API ModelInstancesUbo & operator=( ModelInstancesUbo const & ) = delete;
		C3D_API ModelInstancesUbo( ModelInstancesUbo && ) = default;
		C3D_API ModelInstancesUbo & operator=( ModelInstancesUbo && ) = delete;
		/**@}*/
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine	The engine.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine	Le moteur.
		 */
		C3D_API explicit ModelInstancesUbo( Engine & engine );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~ModelInstancesUbo();
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
		 *\param[in]	instances	The instances data.
		 *\~french
		 *\brief		Met à jour l'UBO avec les valeurs données.
		 *\param[in]	instances	Les données d'instance.
		 */
		C3D_API void cpuUpdate( UInt32Array const & instances );

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
		C3D_API static castor::String const BufferModelInstances;
		C3D_API static castor::String const Instances;
		C3D_API static castor::String const InstanceCount;

	private:
		Engine & m_engine;
		UniformBufferOffsetT< Configuration > m_ubo;
	};
	void cpuUpdate( UniformBufferOffsetT< ModelInstancesUboConfiguration > & buffer
		, UInt32Array const & instances );
}

#define UBO_MODEL_INSTANCES( writer, binding, set )\
	sdw::Ubo modelInstances{ writer\
		, castor3d::ModelInstancesUbo::BufferModelInstances\
		, binding\
		, set };\
	auto c3d_instances = modelInstances.declMember< sdw::UVec4 >( castor3d::ModelInstancesUbo::Instances, 7u );\
	auto c3d_instanceCount = modelInstances.declMember< sdw::UInt >( castor3d::ModelInstancesUbo::InstanceCount );\
	modelInstances.end()

#endif
