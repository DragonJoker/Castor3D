/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RenderTechniquePass_H___
#define ___C3D_RenderTechniquePass_H___

#include "RenderModule.hpp"

#include "Castor3D/Render/GlobalIllumination/LightPropagationVolumes/LightPropagationVolumesModule.hpp"
#include "Castor3D/Render/RenderTechniqueVisitor.hpp"
#include "Castor3D/Render/Ssao/SsaoConfig.hpp"
#include "Castor3D/Render/RenderNodesPass.hpp"

#include <ashespp/Sync/Semaphore.hpp>

namespace castor3d
{
	struct RenderTechniquePassDesc
	{
		RenderTechniquePassDesc( bool environment
			, SsaoConfig const & ssaoConfig )
			: m_ssaoConfig{ &ssaoConfig }
		{
			if ( environment )
			{
				addFlag( m_shaderFlags, ShaderFlag::eEnvironmentMapping );
			}
		}
		/**
		 *\~english
		 *\param[in]	value	The LPV configuration.
		 *\~french
		 *\param[in]	value	La configuration des LPV.
		 */
		RenderTechniquePassDesc & indirect( IndirectLightingData value )
		{
			m_indirectLighting = castor::move( value );
			return *this;
		}
		/**
		 *\~english
		 *\param[in]	value	The LPV result.
		 *\~french
		 *\param[in]	value	Le résultat du LPV.
		 */
		RenderTechniquePassDesc & ssao( Texture const & value )
		{
			m_ssao = &value;
			return *this;
		}
		/**
		 *\~english
		 *\param[in]	value	\p true if the pass writes to velocity texture.
		 *\~french
		 *\param[in]	value	\p true si la passe écrit dans la texture de vélocité.
		 */
		RenderTechniquePassDesc & hasVelocity( bool value )
		{
			if ( value )
			{
				addFlag( m_shaderFlags, ShaderFlag::eVelocity );
			}
			else
			{
				remFlag( m_shaderFlags, ShaderFlag::eVelocity );
			}

			return *this;
		}
		/**
		 *\~english
		 *\param[in]	value	\p true if the pass writes to velocity texture.
		 *\~french
		 *\param[in]	value	\p true si la passe écrit dans la texture de vélocité.
		 */
		RenderTechniquePassDesc & addShaderFlag( ShaderFlags value )
		{
			m_shaderFlags |= value;
			return *this;
		}
		/**
		 *\~english
		 *\param[in]	value	\p true if the pass writes to velocity texture.
		 *\~french
		 *\param[in]	value	\p true si la passe écrit dans la texture de vélocité.
		 */
		RenderTechniquePassDesc & shaderFlags( ShaderFlags value )
		{
			m_shaderFlags = value;
			return *this;
		}
		/**
		 *\~english
		 *\param[in]	value	The clusters configuration.
		 *\~french
		 *\param[in]	value	La configuration des clusters.
		 */
		RenderTechniquePassDesc & clustersConfig( ClustersConfig const * value )
		{
			m_clustersConfig = value;
			return *this;
		}
		/**
		 *\~english
		 *\param[in]	value	\p true to output light scattering.
		 *\~french
		 *\param[in]	value	\p true pour écrire le light scattering.
		 */
		RenderTechniquePassDesc & outputScattering( bool value = true )
		{
			m_outputScattering = value;
			return *this;
		}

		SsaoConfig const * m_ssaoConfig{};
		Texture const * m_ssao{};
		IndirectLightingData m_indirectLighting;
		ShaderFlags m_shaderFlags{ ShaderFlag::eWorldSpace
			| ShaderFlag::eViewSpace
			| ShaderFlag::eTangentSpace
			| ShaderFlag::eLighting
			| ShaderFlag::eOpacity
			| ShaderFlag::eColour };
		ClustersConfig const * m_clustersConfig{};
		bool m_outputScattering{};
	};

	class RenderTechniquePass
	{
	protected:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	parent				The parent technique.
		 *\param[in]	scene				The scene.
		 *\param[in]	outputScattering	Defines if the pass outputs scattering data.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	parent				La technique parente.
		 *\param[in]	scene				La scène.
		 *\param[in]	outputScattering	Définit si la passe sort des données de scattering.
		 */
		C3D_API RenderTechniquePass( RenderTechnique * parent
			, Scene const & scene
			, bool outputScattering );

	public:
		C3D_API virtual ~RenderTechniquePass()noexcept = default;
		/**
		*\~english
		*\brief
		*	Visitor acceptance function.
		*\param visitor
		*	The ... visitor.
		*\~french
		*\brief
		*	Fonction d'acceptation de visiteur.
		*\param visitor
		*	Le ... visiteur.
		*/
		C3D_API virtual void accept( RenderTechniqueVisitor & visitor )
		{
			doAccept( visitor );
		}
		/**
		 *\copydoc	castor3d::RenderNodesPass::update
		 */
		C3D_API virtual void update( CpuUpdater & updater )
		{
		}
		/**
		 *\copydoc	castor3d::RenderNodesPass::createPipelineFlags
		 */
		C3D_API virtual PipelineFlags createPipelineFlags( PassComponentCombine const & passComponents
			, SubmeshComponentCombine const & submeshComponents
			, BlendMode colourBlendMode
			, BlendMode alphaBlendMode
			, RenderPassTypeID renderPassTypeId
			, LightingModelID lightingModelId
			, BackgroundModelID backgroundModelId
			, VkCompareOp alphaFunc
			, VkCompareOp blendAlphaFunc
			, TextureCombine const & textures
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags
			, VkPrimitiveTopology topology
			, bool isFrontCulled
			, uint32_t passLayerIndex
			, GpuBufferOffsetT< castor::Point4f > const & morphTargets
			, SubmeshRenderData * submeshData )const = 0;
		/**
		 *\copydoc	castor3d::RenderNodesPass::areValidPassFlags
		 */
		C3D_API virtual bool areValidPassFlags( PassComponentCombine const & passFlags )const noexcept = 0;
		/**
		 *\copydoc	castor3d::RenderNodesPass::getShaderFlags
		 */
		C3D_API virtual ShaderFlags getShaderFlags()const noexcept = 0;
		/**
		 *\copydoc	castor3d::RenderNodesPass::countNodes
		 */
		C3D_API virtual void countNodes( RenderInfo & info )const noexcept = 0;
		/**
		 *\copydoc	castor3d::RenderNodesPass::isPassEnabled
		 */
		C3D_API virtual bool isPassEnabled()const noexcept = 0;
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		C3D_API Engine * getEngine()const noexcept;
		C3D_API IndirectLightingData const & getIndirectLighting()const noexcept;
		C3D_API DebugConfig & getDebugConfig()const noexcept;
		C3D_API bool areDebugTargetsEnabled()const noexcept;
		C3D_API virtual ClustersConfig const * getClustersConfig()const noexcept = 0;
		C3D_API virtual bool hasSsao()const noexcept = 0;

		Scene const & getScene()noexcept
		{
			return m_scene;
		}

		Scene const & getScene()const noexcept
		{
			return m_scene;
		}

		RenderTechnique const & getTechnique()const noexcept
		{
			return *m_parent;
		}
		/**@}*/

	protected:
		/**
		 *\copydoc		castor3d::RenderTechniquePass::accept
		 */
		C3D_API virtual void doAccept( castor3d::RenderTechniqueVisitor & visitor ) = 0;

	protected:
		RenderTechnique * m_parent{};
		Scene const & m_scene;
		uint32_t m_drawCalls{};
		bool m_outputScattering{};
	};

	class RenderTechniqueNodesPass
		: public RenderNodesPass
		, public RenderTechniquePass
	{
	protected:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	parent				The parent technique.
		 *\param[in]	pass				The parent frame pass.
		 *\param[in]	context				The rendering context.
		 *\param[in]	graph				The runnable graph.
		 *\param[in]	device				The GPU device.
		 *\param[in]	typeName			The pass type name.
		 *\param[in]	targetImage			The image this pass renders to.
		 *\param[in]	targetDepth			The depth image this pass renders to.
		 *\param[in]	renderPassDesc		The scene render pass construction data.
		 *\param[in]	techniquePassDesc	The technique render pass construction data.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	parent				La technique parente.
		 *\param[in]	pass				La frame pass parente.
		 *\param[in]	context				Le contexte de rendu.
		 *\param[in]	graph				Le runnable graph.
		 *\param[in]	device				Le device GPU.
		 *\param[in]	typeName			Le nom du type de la passe.
		 *\param[in]	targetImage			L'image dans laquelle cette passe fait son rendu.
		 *\param[in]	targetDepth			L'image de profondeur dans laquelle cette passe fait son rendu.
		 *\param[in]	renderPassDesc		Les données de construction de passe de rendu de scène.
		 *\param[in]	techniquePassDesc	Les données de construction de passe de rendu de technique.
		 */
		C3D_API RenderTechniqueNodesPass( RenderTechnique * parent
			, crg::FramePass const & pass
			, crg::GraphContext & context
			, crg::RunnableGraph & graph
			, RenderDevice const & device
			, castor::String const & typeName
			, crg::ImageViewIdArray targetImage
			, crg::ImageViewIdArray targetDepth
			, RenderNodesPassDesc const & renderPassDesc
			, RenderTechniquePassDesc const & techniquePassDesc );

	public:
		/**
		 *\copydoc	castor3d::RenderTechniquePass::accept
		 */
		C3D_API void accept( RenderTechniqueVisitor & visitor )override;
		/**
		 *\copydoc	castor3d::RenderNodesPass::update
		 */
		C3D_API void update( CpuUpdater & updater )override;
		/**
		 *\copydoc	castor3d::RenderNodesPass::createPipelineFlags
		 */
		C3D_API PipelineFlags createPipelineFlags( PassComponentCombine const & passComponents
			, SubmeshComponentCombine const & submeshComponents
			, BlendMode colourBlendMode
			, BlendMode alphaBlendMode
			, RenderPassTypeID renderPassTypeId
			, LightingModelID lightingModelId
			, BackgroundModelID backgroundModelId
			, VkCompareOp alphaFunc
			, VkCompareOp blendAlphaFunc
			, TextureCombine const & textures
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags
			, VkPrimitiveTopology topology
			, bool isFrontCulled
			, uint32_t passLayerIndex
			, GpuBufferOffsetT< castor::Point4f > const & morphTargets
			, SubmeshRenderData * submeshData )const override;
		/**
		 *\copydoc	castor3d::RenderNodesPass::getShaderFlags
		 */
		C3D_API ShaderFlags getShaderFlags()const noexcept override
		{
			return m_shaderFlags;
		}
		/**
		 *\copydoc	castor3d::RenderNodesPass::countNodes
		 */
		C3D_API void countNodes( RenderInfo & info )const noexcept override
		{
			RenderNodesPass::countNodes( info );
		}
		/**
		 *\copydoc	castor3d::RenderNodesPass::areValidPassFlags
		 */
		C3D_API bool areValidPassFlags( PassComponentCombine const & passFlags )const noexcept override
		{
			return RenderNodesPass::areValidPassFlags( passFlags );
		}
		/**
		 *\copydoc	castor3d::RenderNodesPass::isPassEnabled
		 */
		C3D_API bool isPassEnabled()const noexcept override
		{
			return RenderNodesPass::isPassEnabled();
		}
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		Engine * getEngine()const noexcept
		{
			return RenderTechniquePass::getEngine();
		}

		Scene const & getScene()const noexcept
		{
			return RenderTechniquePass::getScene();
		}

		RenderTechnique const & getTechnique()const noexcept
		{
			return RenderTechniquePass::getTechnique();
		}

		DebugConfig & getDebugConfig()const noexcept
		{
			return RenderTechniquePass::getDebugConfig();
		}

		ClustersConfig const * getClustersConfig()const noexcept override
		{
			return m_clustersConfig;
		}

		bool hasSsao()const noexcept override
		{
			return m_ssao && m_ssaoConfig && m_ssaoConfig->enabled;
		}

		bool areDebugTargetsEnabled()const noexcept override
		{
			return RenderTechniquePass::areDebugTargetsEnabled();
		}
		/**@}*/

	protected:
		C3D_API void doAccept( castor3d::RenderTechniqueVisitor & visitor )override;
		C3D_API ProgramFlags doAdjustProgramFlags( ProgramFlags flags )const override;
		C3D_API SceneFlags doAdjustSceneFlags( SceneFlags flags )const override;
		C3D_API void doAddEnvBindings( ashes::VkDescriptorSetLayoutBindingArray & bindings
			, uint32_t & index )const;
		C3D_API void doAddGIBindings( ashes::VkDescriptorSetLayoutBindingArray & bindings
			, uint32_t & index )const;
		C3D_API void doAddPassSpecificsBindings( ashes::VkDescriptorSetLayoutBindingArray & bindings
			, uint32_t & index )const;
		C3D_API void doAddEnvDescriptor( ashes::WriteDescriptorSetArray & descriptorWrites
			, uint32_t & index )const;
		C3D_API void doAddGIDescriptor( ashes::WriteDescriptorSetArray & descriptorWrites
			, uint32_t & index )const;
		C3D_API void doAddPassSpecificsDescriptor( ashes::WriteDescriptorSetArray & descriptorWrites
			, uint32_t & index )const;

	private:
		using RenderNodesPass::update;

		void doFillAdditionalBindings( PipelineFlags const & flags
			, ashes::VkDescriptorSetLayoutBindingArray & bindings )const override;
		void doFillAdditionalDescriptor( PipelineFlags const & flags
			, ashes::WriteDescriptorSetArray & descriptorWrites
			, ShadowMapLightTypeArray const & shadowMaps
			, ShadowBuffer const * shadowBuffer )override;
		ashes::PipelineDepthStencilStateCreateInfo doCreateDepthStencilState( PipelineFlags const & flags )const override;
		ashes::PipelineColorBlendStateCreateInfo doCreateBlendState( PipelineFlags const & flags )const override;

	protected:
		Camera * m_camera{ nullptr };
		ShaderFlags m_shaderFlags{};
		SsaoConfig const * m_ssaoConfig{};
		Texture const * m_ssao{};
		IndirectLightingData m_indirectLighting;
		ClustersConfig const * m_clustersConfig{};
	};
}

#endif
