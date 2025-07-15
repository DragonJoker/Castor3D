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
		: public c3d::SubmeshComponent
	{
	public:
		struct SurfaceShader
			: public c3d::shader::SubmeshRasterSurfaceShader
		{
			void fillSurfaceType( sdw::type::Struct & type
				, uint32_t * index )const override;
		};

		struct RenderData
			: public c3d::SubmeshRenderData
		{
			RenderData( c3d::SubmeshComponent const & component );
			/**
			 *\copydoc	c3d::shader::SubmeshRenderShader::initialise
			 */
			bool initialise( c3d::RenderDevice const & device )override;
			/**
			 *\copydoc	c3d::shader::SubmeshRenderShader::record
			 */
			crg::FramePassArray record( c3d::RenderDevice const & device
				, crg::ResourcesCache & resources
				, crg::FramePassGroup & graph
				, crg::FramePassArray previousPasses )override;
			/**
			 *\copydoc	c3d::shader::SubmeshRenderShader::registerDependencies
			 */
			void registerDependencies( crg::FramePass & pass )const override;
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
			 *\copydoc		c3d::SubmeshComponent::clone
			 */
			void accept( c3d::ConfigurationVisitorBase & vis );
			/**
			 *\copydoc	c3d::shader::SubmeshRenderShader::getBindingCount
			 */
			uint32_t getBindingCount()const noexcept override
			{
				return 4u;
			}
			/**
			 *\copydoc	c3d::shader::SubmeshRenderShader::getPrimitiveTopology
			 */
			VkPrimitiveTopology getPrimitiveTopology()const noexcept override
			{
				return VK_PRIMITIVE_TOPOLOGY_PATCH_LIST;
			}
			/**
			 *\copydoc	c3d::shader::SubmeshRenderShader::getPatchVertices
			 */
			uint32_t getPatchVertices()const noexcept override
			{
				return 1u;
			}

		private:
			FFTWavesComponent const & m_component;
			OceanUboConfiguration m_config;
			c3d::RawUniquePtr< OceanUbo > m_ubo;
			c3d::RawUniquePtr< OceanFFT > m_oceanFFT;
			ashes::SamplerPtr m_linearWrapSampler;
			ashes::SamplerPtr m_pointClampSampler;
			c3d::PreciseTimer m_timer;
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
				return c3d::makeUniqueDerived< c3d::SubmeshComponent, FFTWavesComponent >( submesh );
			}

			c3d::shader::SubmeshRasterSurfaceShaderPtr createRasterSurfaceShader()const override
			{
				return c3d::makeRawUnique< SurfaceShader >();
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
		explicit FFTWavesComponent( c3d::Submesh & submesh );
		/**
		 *\copydoc		c3d::SubmeshComponent::clone
		 */
		c3d::SubmeshComponentUPtr clone( c3d::Submesh & submesh )const override;
		/**
		 *\copydoc		c3d::SubmeshComponent::clone
		 */
		void accept( c3d::ConfigurationVisitorBase & vis )override;

		void setFftConfig( OceanFFTConfig config )noexcept
		{
			m_fftConfig = c3d::move( config );
		}

		OceanFFTConfig const & getFftConfig()const noexcept
		{
			return m_fftConfig;
		}

	public:
		static c3d::String const TypeName;
		static c3d::MbString const FullName;

	private:
		OceanFFTConfig m_fftConfig;
	};

	CU_DeclareSmartPtr( ocean_fft, FFTWavesComponent, );
}

#endif
