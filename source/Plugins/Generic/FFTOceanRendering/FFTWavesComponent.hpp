/*
See LICENSE file in root folder
*/
#ifndef ___C3DORFFT_FFTWavesRenderComponent_H___
#define ___C3DORFFT_FFTWavesRenderComponent_H___

#include "OceanFFTUbo.hpp"
#include "OceanFFT.hpp"

#include <Castor3D/Model/Mesh/Submesh/Component/SubmeshComponent.hpp>
#include <CastorUtils/Miscellaneous/PreciseTimer.hpp>

namespace ocean_fft
{
	class FFTWavesComponent
		: public castor3d::SubmeshComponent
	{
	public:
		struct SurfaceShader
			: public castor3d::shader::SubmeshRasterSurfaceShader
		{
			void fillSurfaceType( sdw::type::Struct & type
				, uint32_t * index )const override;
		};

		struct RenderData
			: public castor3d::SubmeshRenderData
		{
			RenderData( castor3d::SubmeshComponent const & component );
			/**
			 *\copydoc	castor3d::shader::SubmeshRenderShader::initialise
			 */
			bool initialise( castor3d::RenderDevice const & device )override;
			/**
			 *\copydoc	castor3d::shader::SubmeshRenderShader::record
			 */
			crg::FramePassArray record( castor3d::RenderDevice const & device
				, crg::ResourcesCache & resources
				, crg::FrameGraph & graph
				, crg::FramePassArray previousPasses )override;
			/**
			 *\copydoc	castor3d::shader::SubmeshRenderShader::registerDependencies
			 */
			void registerDependencies( crg::FramePass & pass )const override;
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
			 *\copydoc		castor3d::SubmeshComponent::clone
			 */
			void accept( castor3d::ConfigurationVisitorBase & vis );
			/**
			 *\copydoc	castor3d::shader::SubmeshRenderShader::getBindingCount
			 */
			uint32_t getBindingCount()const noexcept override
			{
				return 4u;
			}
			/**
			 *\copydoc	castor3d::shader::SubmeshRenderShader::getPrimitiveTopology
			 */
			VkPrimitiveTopology getPrimitiveTopology()const noexcept override
			{
				return VK_PRIMITIVE_TOPOLOGY_PATCH_LIST;
			}
			/**
			 *\copydoc	castor3d::shader::SubmeshRenderShader::getPatchVertices
			 */
			uint32_t getPatchVertices()const noexcept override
			{
				return 1u;
			}

		private:
			FFTWavesComponent const & m_component;
			OceanUboConfiguration m_config;
			std::unique_ptr< OceanUbo > m_ubo;
			std::unique_ptr< OceanFFT > m_oceanFFT;
			ashes::SamplerPtr m_linearWrapSampler;
			ashes::SamplerPtr m_pointClampSampler;
			castor::PreciseTimer m_timer;
		};

		struct RenderShader
			: public castor3d::SubmeshRenderShader
		{
			castor3d::SubmeshRenderDataPtr createData( castor3d::SubmeshComponent const & component )override
			{
				return std::make_unique< RenderData >( component );
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
				return castor::makeUniqueDerived< castor3d::SubmeshComponent, FFTWavesComponent >( submesh );
			}

			castor3d::shader::SubmeshRasterSurfaceShaderPtr createRasterSurfaceShader()const override
			{
				return std::make_unique< SurfaceShader >();
			}

			castor3d::SubmeshRenderShaderPtr createRenderShader()const override
			{
				return std::make_unique< RenderShader >();
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
		explicit FFTWavesComponent( castor3d::Submesh & submesh );
		/**
		 *\copydoc		castor3d::SubmeshComponent::clone
		 */
		castor3d::SubmeshComponentUPtr clone( castor3d::Submesh & submesh )const override;
		/**
		 *\copydoc		castor3d::SubmeshComponent::clone
		 */
		void accept( castor3d::ConfigurationVisitorBase & vis )override;

		void setFftConfig( OceanFFTConfig config )noexcept
		{
			m_fftConfig = std::move( config );
		}

		OceanFFTConfig const & getFftConfig()const noexcept
		{
			return m_fftConfig;
		}

	public:
		static castor::String const TypeName;
		static castor::String const FullName;

	private:
		OceanFFTConfig m_fftConfig;
		std::unique_ptr< RenderData > m_data;
	};

	CU_DeclareSmartPtr( ocean_fft, FFTWavesComponent, );
}

#endif
