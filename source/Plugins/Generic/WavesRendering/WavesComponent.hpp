/*
See LICENSE file in root folder
*/
#ifndef ___C3D_WavesComponent_H___
#define ___C3D_WavesComponent_H___

#include "WavesUbo.hpp"

#include <Castor3D/Miscellaneous/Parameter.hpp>
#include <Castor3D/Model/Mesh/Submesh/Component/SubmeshComponent.hpp>

namespace waves
{
	struct WavesContext
	{
		castor3d::MeshRPtr mesh{};
		WavesConfiguration config{};
		castor3d::Parameters parameters{};
		uint32_t wave{};
	};

	class WavesRenderComponent
		: public castor3d::SubmeshComponent
	{
	public:
		struct RenderData
			: public castor3d::SubmeshRenderData
		{
			RenderData( castor3d::SubmeshComponent const & component );
			/**
			 *\copydoc	castor3d::shader::SubmeshRenderShader::initialise
			 */
			bool initialise( castor3d::RenderDevice const & device )override;
			/**
			 *\copydoc	castor3d::shader::SubmeshRenderShader::cleanup
			 */
			void cleanup( castor3d::RenderDevice const & device )override;
			/**
			 *\copydoc	castor3d::shader::SubmeshRenderShader::update
			 */
			void update( castor3d::CpuUpdater & updater )override;
			/**
			 *\copydoc	castor3d::shader::SubmeshRenderShader::fillBindings
			 */
			void fillBindings( castor3d::PipelineFlags const & flags
				, ashes::VkDescriptorSetLayoutBindingArray & bindings
				, uint32_t & index )const override;
			/**
			 *\copydoc	castor3d::shader::SubmeshRenderShader::fillDescriptor
			 */
			void fillDescriptor( castor3d::PipelineFlags const & flags
				, ashes::WriteDescriptorSetArray & descriptorWrites
				, uint32_t & index )const override;
			/**
			 *\copydoc	castor3d::shader::SubmeshRenderShader::getBindingCount
			 */
			uint32_t getBindingCount()const noexcept override
			{
				return 1u;
			}
			/**
			 *\copydoc	castor3d::shader::SubmeshRenderShader::getPrimitiveTopology
			 */
			VkPrimitiveTopology getPrimitiveTopology()const noexcept override
			{
				return VK_PRIMITIVE_TOPOLOGY_PATCH_LIST;
			}

		private:
			WavesRenderComponent const & m_component;
			castor::RawUniquePtr< WavesUbo > m_ubo;
		};

		struct RenderShader
			: public castor3d::SubmeshRenderShader
		{
			castor3d::SubmeshRenderDataPtr createData( castor3d::SubmeshComponent const & component )override
			{
				return castor::make_unique< RenderData >( component );
			}
			/**
			 *\copydoc	castor3d::shader::SubmeshRenderShader::getShaderSource
			 */
			void getShaderSource( castor3d::Engine const & engine
				, castor3d::PipelineFlags const & flags
				, castor3d::ComponentModeFlags const & componentsMask
				, ast::ShaderBuilder & builder )const override;
		};

		class Plugin
			: public castor3d::SubmeshComponentPlugin
		{
		public:
			explicit Plugin( castor3d::SubmeshComponentRegister const & submeshComponents );

			castor3d::SubmeshComponentUPtr createComponent( castor3d::Submesh & submesh )const override
			{
				return castor::makeUniqueDerived< castor3d::SubmeshComponent, WavesRenderComponent >( submesh );
			}

			castor3d::SubmeshRenderShaderPtr createRenderShader()const override
			{
				return castor::make_unique< RenderShader >();
			}

			castor3d::SubmeshComponentFlag getRenderFlag()const noexcept override
			{
				return getComponentFlags();
			}

			void createParsers( castor::AttributeParsers & parsers )const override;
			void createSections( castor::StrUInt32Map & sections )const override;
		};

		static castor3d::SubmeshComponentPluginUPtr createPlugin( castor3d::SubmeshComponentRegister const & submeshComponents )
		{
			return castor::makeUniqueDerived< castor3d::SubmeshComponentPlugin, Plugin >( submeshComponents );
		}
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	submesh	The parent submesh.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	submesh	Le sous-maillage parent.
		 */
		explicit WavesRenderComponent( castor3d::Submesh & submesh );
		/**
		 *\copydoc		castor3d::SubmeshComponent::clone
		 */
		castor3d::SubmeshComponentUPtr clone( castor3d::Submesh & submesh )const override;

		void setConfig( WavesConfiguration config )noexcept
		{
			m_config = castor::move( config );
		}

		WavesConfiguration const & getConfig()const noexcept
		{
			return m_config;
		}

	public:
		static castor::String const TypeName;
		static castor::MbString const FullName;

	private:
		WavesConfiguration m_config;
	};

	CU_DeclareSmartPtr( waves, WavesRenderComponent, );
}

#endif
