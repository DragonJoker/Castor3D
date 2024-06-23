/*
See LICENSE file in root folder
*/
#ifndef ___C3D_NodesPass_H___
#define ___C3D_NodesPass_H___

#include "RenderModule.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/InstantiationComponent.hpp"
#include "Castor3D/Render/RenderInfo.hpp"
#include "Castor3D/Render/Clustered/ClusteredModule.hpp"
#include "Castor3D/Render/Culling/CullingModule.hpp"
#include "Castor3D/Render/Node/RenderNodeModule.hpp"
#include "Castor3D/Scene/SceneModule.hpp"
#include "Castor3D/Scene/Animation/AnimationModule.hpp"
#include "Castor3D/Shader/Ubos/UbosModule.hpp"

#include <CastorUtils/Design/Named.hpp>
#include <CastorUtils/Graphics/Size.hpp>

#include <optional>

namespace castor3d
{
	struct NodesPassDesc
	{
	public:
		NodesPassDesc( VkExtent3D size
			, CameraUbo const & cameraUbo
			, SceneUbo const * sceneUbo
			, RenderFilters filters )
			: m_size{ castor::move( size ) }
			, m_cameraUbo{ cameraUbo }
			, m_sceneUbo{ sceneUbo }
			, m_filters{ filters }
		{
		}
		/**
		 *\~english
		 *\param[in]	value	The scene node to ignore during rendering.
		 *\~french
		 *\param[in]	value	Le scene node à ignorer pendant le rendu.
		 */
		NodesPassDesc & safeBand( bool value )
		{
			m_safeBand = value;
			return *this;
		}
		/**
		 *\~english
		 *\param[in]	value	\p true if the pass is for static nodes.
		 *\~french
		 *\param[in]	value	\p true si la passe est pour les noeuds statiques.
		 */
		NodesPassDesc & isStatic( bool value )
		{
			m_handleStatic = value;
			return *this;
		}
		/**
		 *\~english
		 *\param[in]	value	The nodes pass components flags.
		 *\~french
		 *\param[in]	value	Les indicateurs de composants de la passe de noeuds.
		 */
		NodesPassDesc & componentModeFlags( ComponentModeFlags value )
		{
			m_componentModeFlags = castor::move( value );
			return *this;
		}

		VkExtent3D m_size;
		CameraUbo const & m_cameraUbo;
		SceneUbo const * m_sceneUbo{};
		RenderFilters m_filters;
		bool m_safeBand{};
		uint32_t m_index{ 0u };
		std::optional< bool > m_handleStatic{ std::nullopt };
		ComponentModeFlags m_componentModeFlags{ ComponentModeFlag::eAll };
	};

	class NodesPass
		: public castor::OwnedBy< Engine >
		, public castor::Named
	{
	protected:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	device		The GPU device.
		 *\param[in]	fullName	The pass name.
		 *\param[in]	targetImage	The image this pass renders to.
		 *\param[in]	targetDepth	The depth image this pass renders to.
		 *\param[in]	desc		The construction data.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	device		Le device GPU.
		 *\param[in]	fullName	Le nom de la passe.
		 *\param[in]	targetImage	L'image dans laquelle cette passe fait son rendu.
		 *\param[in]	targetDepth	L'image de profondeur dans laquelle cette passe fait son rendu.
		 *\param[in]	desc		Les données de construction.
		 */
		C3D_API NodesPass( RenderDevice const & device
			, castor::String const & categoryName
			, castor::String const & typeName
			, castor::String const & fullName
			, crg::ImageViewIdArray targetImage
			, crg::ImageViewIdArray targetDepth
			, NodesPassDesc const & desc );

	public:
		virtual ~NodesPass() noexcept = default;
		/**
		 *\~english
		 *\brief			Updates the render pass, CPU wise.
		 *\param[in, out]	updater	The update data.
		 *\~french
		 *\brief			Met à jour la passe de rendu, au niveau CPU.
		 *\param[in, out]	updater	Les données d'update.
		 */
		C3D_API void update( CpuUpdater & updater );
		/**
		 *\~english
		 *\brief		Adjusts given flags to match the render pass requirements.
		 *\param[in]	submeshCombine	The flags.
		 *\~french
		 *\brief		Ajuste les flags donnés pour qu'ils correspondent aux pré-requis de la passe.
		 *\param[in]	submeshCombine	Les flags.
		 */
		C3D_API SubmeshComponentCombine adjustFlags( SubmeshComponentCombine const & submeshCombine )const;
		/**
		 *\~english
		 *\brief		Adjusts given flags to match the render pass requirements.
		 *\param[in]	passCombine	The flags.
		 *\~french
		 *\brief		Ajuste les flags donnés pour qu'ils correspondent aux pré-requis de la passe.
		 *\param[in]	passCombine	Les flags.
		 */
		C3D_API PassComponentCombine adjustFlags( PassComponentCombine const & passCombine )const;
		/**
		 *\~english
		 *\brief		Filters the given textures flags using this pass needed textures.
		 *\param[in]	textureCombine	The textures flags.
		 *\return		The filtered flags.
		 *\~french
		 *\brief		Filtre les indicateurs de textures donnés en utilisant ceux voulus par cette passe.
		 *\param[in]	textureCombine	Les indicateurs de textures.
		 *\return		Les indicateurs filtrés.
		 */
		C3D_API TextureCombine adjustFlags( TextureCombine const & textureCombine )const;
		/**
		 *\~english
		 *\brief		Adjusts given flags to match the render pass requirements.
		 *\param[in]	flags	The flags.
		 *\~french
		 *\brief		Ajuste les flags donnés pour qu'ils correspondent aux pré-requis de la passe.
		 *\param[in]	flags	Les flags.
		 */
		C3D_API SceneFlags adjustFlags( SceneFlags flags )const;
		/**
		 *\~english
		 *\brief			Adds shadow maps descriptor layout bindings to given list.
		 *\param[in]		sceneFlags		Used to define what shadow maps need to be bound.
		 *\param[in,out]	bindings		Receives the bindings.
		 *\param[in]		shaderStages	The impacted shader stages.
		 *\param[in,out]	index			The current binding index.
		 *\~french
		 *\brief			Ajoute les bindings de descriptor layout des shadow maps à la liste donnée.
		 *\param[in]		sceneFlags		Le mode de mélange couleurs.
		 *\param[in,out]	bindings		Reçoit les bindings.
		 *\param[in]		shaderStages	Les shader stages impactés.
		 *\param[in,out]	index			L'index de binding actuel.
		 */
		C3D_API static void addShadowBindings( SceneFlags const & sceneFlags
			, ashes::VkDescriptorSetLayoutBindingArray & bindings
			, VkShaderStageFlags shaderStages
			, uint32_t & index );
		/**
		 *\~english
		 *\brief			Adds shadow maps descriptor layout bindings to given list.
		 *\param[in,out]	bindings		Receives the bindings.
		 *\param[in]		shaderStages	The impacted shader stages.
		 *\param[in,out]	index			The current binding index.
		 *\~french
		 *\brief			Ajoute les bindings de descriptor layout des shadow maps à la liste donnée.
		 *\param[in,out]	bindings		Reçoit les bindings.
		 *\param[in]		shaderStages	Les shader stages impactés.
		 *\param[in,out]	index			L'index de binding actuel.
		 */
		C3D_API static void addShadowBindings( ashes::VkDescriptorSetLayoutBindingArray & bindings
			, VkShaderStageFlags shaderStages
			, uint32_t & index );
		/**
		 *\~english
		 *\brief			Adds background descriptor layout bindings to given list.
		 *\param[in]		background		The background.
		 *\param[in,out]	bindings		Receives the bindings.
		 *\param[in]		shaderStages	The impacted shader stages.
		 *\param[in,out]	index			The current binding index.
		 *\~french
		 *\brief			Ajoute les bindings de descriptor layout du background à la liste donnée.
		 *\param[in]		background		Le fond.
		 *\param[in,out]	bindings		Reçoit les bindings.
		 *\param[in]		shaderStages	Les shader stages impactés.
		 *\param[in,out]	index			L'index de binding actuel.
		 */
		C3D_API static void addBackgroundBindings( SceneBackground const & background
			, ashes::VkDescriptorSetLayoutBindingArray & bindings
			, VkShaderStageFlags shaderStages
			, uint32_t & index );
		/**
		 *\~english
		 *\brief			Adds indirect lighting descriptor layout bindings to given list.
		 *\param[in]		flags				The scene flags.
		 *\param[in]		indirectLighting	The indirect lighting data.
		 *\param[in,out]	bindings			Receives the bindings.
		 *\param[in]		shaderStages		The impacted shader stages.
		 *\param[in,out]	index				The current binding index.
		 *\~french
		 *\brief			Ajoute les bindings de descriptor layout de l'éclairage indirect à la liste donnée.
		 *\param[in]		flags				Les indicateurs de scène.
		 *\param[in]		indirectLighting	Les données d'indirect lighting.
		 *\param[in,out]	bindings			Reçoit les bindings.
		 *\param[in]		shaderStages		Les shader stages impactés.
		 *\param[in,out]	index				L'index de binding actuel.
		 */
		C3D_API static void addGIBindings( SceneFlags flags
			, IndirectLightingData const & indirectLighting
			, ashes::VkDescriptorSetLayoutBindingArray & bindings
			, VkShaderStageFlags shaderStages
			, uint32_t & index );
		/**
		 *\~english
		 *\brief			Adds clusters descriptor layout bindings to given list.
		 *\param[in]		frustumClusters	The clusters.
		 *\param[in,out]	bindings		Receives the bindings.
		 *\param[in]		shaderStages	The impacted shader stages.
		 *\param[in,out]	index			The current binding index.
		 *\~french
		 *\brief			Ajoute les bindings de descriptor layout des clusters à la liste donnée.
		 *\param[in]		frustumClusters	Les clusters.
		 *\param[in,out]	bindings		Reçoit les bindings.
		 *\param[in]		shaderStages	Les shader stages impactés.
		 *\param[in,out]	index			L'index de binding actuel.
		 */
		C3D_API static void addClusteredLightingBindings( FrustumClusters const & frustumClusters
			, ashes::VkDescriptorSetLayoutBindingArray & bindings
			, VkShaderStageFlags shaderStages
			, uint32_t & index );
		C3D_API static void addShadowDescriptor( RenderSystem const & renderSystem
			, crg::RunnableGraph & graph
			, ashes::WriteDescriptorSetArray & descriptorWrites
			, ShadowMapLightTypeArray const & shadowMaps
			, ShadowBuffer const & shadowBuffer
			, uint32_t & index );
		C3D_API static void addShadowDescriptor( RenderSystem const & renderSystem
			, crg::RunnableGraph & graph
			, SceneFlags const & sceneFlags
			, ashes::WriteDescriptorSetArray & descriptorWrites
			, ShadowMapLightTypeArray const & shadowMaps
			, ShadowBuffer const & shadowBuffer
			, uint32_t & index );
		C3D_API static void addBackgroundDescriptor( SceneBackground const & background
			, ashes::WriteDescriptorSetArray & descriptorWrites
			, crg::ImageViewIdArray const & targetImage
			, uint32_t & index );
		C3D_API static void addGIDescriptor( SceneFlags sceneFlags
			, IndirectLightingData const & indirectLighting
			, ashes::WriteDescriptorSetArray & descriptorWrites
			, uint32_t & index );
		C3D_API static void addClusteredLightingDescriptor( FrustumClusters const & frustumClusters
			, ashes::WriteDescriptorSetArray & descriptorWrites
			, uint32_t & index );
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		C3D_API bool areValidPassFlags( PassComponentCombine const & passFlags )const noexcept;
		C3D_API virtual bool isPassEnabled()const noexcept;
		C3D_API bool isValidPass( Pass const & pass )const noexcept;
		C3D_API bool isValidRenderable( RenderedObject const & object )const noexcept;
		C3D_API bool isValidNode( SceneNode const & node )const noexcept;

		ComponentModeFlags getComponentsMask()const noexcept
		{
			return m_componentsMask;
		}

		bool isDirty()const noexcept
		{
			return m_isDirty;
		}

		RenderFilters getRenderFilters()const noexcept
		{
			return m_filters;
		}

		castor::String const & getTypeName()const noexcept
		{
			return m_typeName;
		}

		RenderPassTypeID getTypeID()const noexcept
		{
			return m_typeID;
		}

		bool filtersStatic()const noexcept
		{
			return handleStatic()
				&& !*m_handleStatic;
		}

		bool filtersNonStatic()const noexcept
		{
			return handleStatic()
				&& *m_handleStatic;
		}

		bool handleStatic()const noexcept
		{
			return m_handleStatic != std::nullopt;
		}
		/**@}*/

		mutable NodesPassChangeSignal onSortNodes;

	protected:
		/**
		 *\~english
		 *\brief			Updates the render pass, CPU wise.
		 *\param[in, out]	updater	The update data.
		 *\~french
		 *\brief			Met à jour la passe de rendu, au niveau CPU.
		 *\param[in, out]	updater	Les données d'update.
		 */
		C3D_API virtual void doUpdate( CpuUpdater & updater ) = 0;
		/**
		 *\~english
		 *\param[in]	pass	The material pass.
		 *\return		\p true if the pass is rendered through this nodes pass.
		 *\~french
		 *\param[in]	pass	La passe de matériau.
		 *\return		\p true si la passe est rendue via cette passe de noeuds.
		 */
		C3D_API virtual bool doIsValidPass( Pass const & pass )const noexcept;
		/**
		 *\~english
		 *\param[in]	object	The rendered object.
		 *\return		\p true if the object is rendered through this nodes pass.
		 *\~french
		 *\param[in]	object	L'objet rendu.
		 *\return		\p true si l'objet est rendu via cette passe de noeuds.
		 */
		C3D_API virtual bool doIsValidRenderable( RenderedObject const & object )const noexcept;
		C3D_API virtual SubmeshComponentCombine doAdjustSubmeshComponents( SubmeshComponentCombine submeshCombine )const;
		C3D_API virtual SceneFlags doAdjustSceneFlags( SceneFlags flags )const;
		C3D_API virtual bool doAreValidPassFlags( PassComponentCombine const & passFlags )const noexcept = 0;
		C3D_API virtual bool doIsValidNode( SceneNode const & node )const noexcept = 0;

	protected:
		RenderDevice const & m_device;
		RenderSystem & m_renderSystem;
		CameraUbo const & m_cameraUbo;
		crg::ImageViewIdArray m_targetImage;
		crg::ImageViewIdArray m_targetDepth;
		castor::String m_typeName;
		RenderPassTypeID m_typeID{};
		RenderFilters m_filters{ RenderFilter::eNone };
		castor::String m_category;
		castor::Size m_size;
		bool m_safeBand{ false };
		bool m_isDirty{ true };
		SceneUbo const * m_sceneUbo{};
		uint32_t m_index{ 0u };
		std::optional< bool > m_handleStatic{ std::nullopt };
		ComponentModeFlags m_componentsMask{};
	};

	struct IsNodesPassEnabled
	{
		explicit IsNodesPassEnabled( NodesPass const & pass )
			: m_pass{ &pass }
		{
		}

		IsNodesPassEnabled()
			: m_pass{}
		{
		}

		void setPass( NodesPass const & pass )noexcept
		{
			m_pass = &pass;
		}

		bool operator()()const noexcept
		{
			return m_pass
				&& m_pass->isPassEnabled();
		}

		NodesPass const * m_pass;
	};
	CU_DeclareSmartPtr( castor3d, IsNodesPassEnabled, C3D_API );
}

#endif
