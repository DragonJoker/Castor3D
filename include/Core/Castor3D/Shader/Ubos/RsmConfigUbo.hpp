/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RsmConfigUbo_H___
#define ___C3D_RsmConfigUbo_H___

#include "UbosModule.hpp"

#include "Castor3D/Render/Technique/Opaque/Lighting/LightingModule.hpp"

#include "Castor3D/Buffer/UniformBufferOffset.hpp"
#include "Castor3D/Scene/Light/LightModule.hpp"

#include <ShaderWriter/CompositeTypes/StructInstance.hpp>
#include <ShaderWriter/BaseTypes/Float.hpp>
#include <ShaderWriter/BaseTypes/UInt.hpp>

namespace castor3d
{
	namespace shader
	{
		struct RsmConfigData
			: public sdw::StructInstance
		{
			C3D_API RsmConfigData( sdw::ShaderWriter & writer
				, ast::expr::ExprPtr expr
				, bool enabled );
			C3D_API RsmConfigData & operator=( RsmConfigData const & rhs );

			C3D_API static ast::type::StructPtr makeType( ast::type::TypesCache & cache );
			C3D_API static std::unique_ptr< sdw::Struct > declare( sdw::ShaderWriter & writer );

			sdw::Float rsmIntensity;
			sdw::Float rsmRMax;
			sdw::UInt rsmSampleCount;
			sdw::UInt rsmIndex;

		private:
			using sdw::StructInstance::getMember;
			using sdw::StructInstance::getMemberArray;
		};
	}

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
		C3D_API static std::string const RsmConfigData;

	private:
		Engine & m_engine;
		UniformBufferOffsetT< Configuration > m_ubo;
	};
}

#define UBO_RSM_CONFIG( writer, binding, set )\
	sdw::Ubo rsmConfig{ writer\
		, castor3d::RsmConfigUbo::BufferRsmConfig\
		, binding\
		, set\
		, ast::type::MemoryLayout::eStd140\
		, true };\
	auto c3d_rsmConfigData = rsmConfig.declStructMember< castor3d::shader::RsmConfigData >( castor3d::RsmConfigUbo::RsmConfigData );\
	rsmConfig.end()

#endif
