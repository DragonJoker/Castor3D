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
		c3d::MeshRPtr mesh{};
		WavesConfiguration config{};
		c3d::Parameters parameters{};
		uint32_t wave{};
	};

	class WavesRenderComponent
		: public c3d::SubmeshComponent
	{
	public:
		struct RenderData
			: public c3d::SubmeshRenderData
		{
			RenderData( c3d::SubmeshComponent const & component );
			/**
			 *\copydoc	c3d::shader::SubmeshRenderShader::initialise
			 */
			bool initialise( c3d::RenderDevice const & device )override;
			/**
			 *\copydoc	c3d::shader::SubmeshRenderShader::cleanup
			 */
			void cleanup( c3d::RenderDevice const & device )override;
			/**
			 *\copydoc	c3d::shader::SubmeshRenderShader::update
			 */
			void update( c3d::CpuUpdater & updater )override;
			/**
			 *\copydoc	c3d::shader::SubmeshRenderShader::fillBindings
			 */
			void fillBindings( c3d::PipelineFlags const & flags
				, ashes::VkDescriptorSetLayoutBindingArray & bindings
				, uint32_t & index )const override;
			/**
			 *\copydoc	c3d::shader::SubmeshRenderShader::fillDescriptor
			 */
			void fillDescriptor( c3d::PipelineFlags const & flags
				, ashes::WriteDescriptorSetArray & descriptorWrites
				, uint32_t & index )const override;
			/**
			 *\copydoc	c3d::shader::SubmeshRenderShader::getBindingCount
			 */
			uint32_t getBindingCount()const noexcept override
			{
				return 1u;
			}
			/**
			 *\copydoc	c3d::shader::SubmeshRenderShader::getPrimitiveTopology
			 */
			VkPrimitiveTopology getPrimitiveTopology()const noexcept override
			{
				return VK_PRIMITIVE_TOPOLOGY_PATCH_LIST;
			}

		private:
			WavesRenderComponent const & m_component;
			c3d::RawUniquePtr< WavesUbo > m_ubo;
		};

		struct RenderShader
			: public c3d::SubmeshRenderShader
		{
			c3d::SubmeshRenderDataPtr createData( c3d::SubmeshComponent const & component )override
			{
				return c3d::makeRawUnique< RenderData >( component );
			}
			/**
			 *\copydoc	c3d::shader::SubmeshRenderShader::getShaderSource
			 */
			void getShaderSource( c3d::Engine const & engine
				, c3d::PipelineFlags const & flags
				, c3d::ComponentModeFlags const & componentsMask
				, ast::ShaderBuilder & builder )const override;
		};

		class Plugin
			: public c3d::SubmeshComponentPlugin
		{
		public:
			explicit Plugin( c3d::SubmeshComponentRegister const & submeshComponents );

			c3d::SubmeshComponentUPtr createComponent( c3d::Submesh & submesh )const override
			{
				return c3d::makeUniqueDerived< c3d::SubmeshComponent, WavesRenderComponent >( submesh );
			}

			c3d::SubmeshRenderShaderPtr createRenderShader()const override
			{
				return c3d::makeRawUnique< RenderShader >();
			}

			c3d::SubmeshComponentFlag getRenderFlag()const noexcept override
			{
				return getComponentFlags();
			}

			void createParsers( c3d::AttributeParsers & parsers )const override;
			void createSections( c3d::StrUInt32Map & sections )const override;
		};

		static c3d::SubmeshComponentPluginUPtr createPlugin( c3d::SubmeshComponentRegister const & submeshComponents )
		{
			return c3d::makeUniqueDerived< c3d::SubmeshComponentPlugin, Plugin >( submeshComponents );
		}
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	submesh	The parent submesh.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	submesh	Le sous-maillage parent.
		 */
		explicit WavesRenderComponent( c3d::Submesh & submesh );
		/**
		 *\copydoc		c3d::SubmeshComponent::clone
		 */
		c3d::SubmeshComponentUPtr clone( c3d::Submesh & submesh )const override;

		void setConfig( WavesConfiguration config )noexcept
		{
			m_config = c3d::move( config );
		}

		WavesConfiguration const & getConfig()const noexcept
		{
			return m_config;
		}

	public:
		static c3d::String const TypeName;
		static c3d::MbString const FullName;

	private:
		WavesConfiguration m_config;
	};

	CU_DeclareSmartPtr( waves, WavesRenderComponent, );
}

#endif
