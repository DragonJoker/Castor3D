/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GPU_INFORMATIONS_H___
#define ___C3D_GPU_INFORMATIONS_H___

#include "Render/Context.hpp"

#include <Design/OwnedBy.hpp>

#if defined( min )
#	undef min
#	undef max
#	undef abs
#endif

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		04/05/2016
	\version	0.9.0
	\~english
	\brief		All supported GPU feature flags.
	\~french
	\brief		Indicateurs de caractérisituqes du GPU.
	*/
	enum class GpuFeature
		: uint32_t
	{
		//!\~english	Tells whether or not the selected render API supports instanced draw calls.
		//!\~french		Dit si l'API de rendu choisie supporte le dessin instancié.
		eInstancing = 0x00000001,
		//!\~english	Tells whether or not the selected render API supports accumulation buffers.
		//!\~french		Dit si l'API de rendu choisie supporte le buffer d'accumulation.
		eAccumulationBuffer = 0x00000002,
		//!\~english	Tells whether or not the selected render API supports non power of two textures.
		//!\~french		Dit si l'API de rendu choisie supporte les textures non puissance de 2.
		eNonPowerOfTwoTextures = 0x00000004,
		//!\~english	Tells whether or not the selected render API supports stereo.
		//!\~french		Dit si l'API de rendu choisie supporte la stéréographie.
		eStereo = 0x00000008,
		//!\~english	Tells whether or not the selected render API supports constants buffers.
		//!\~french		Dit si l'API de rendu choisie supporte les tampons de constantes.
		eConstantsBuffers = 0x00000010,
		//!\~english	Tells whether or not the selected render API supports texture buffers.
		//!\~french		Dit si l'API de rendu choisie supporte les tampons de textures.
		eTextureBuffers = 0x00000020,
		//!\~english	Tells whether or not the selected render API supports shader stoarage buffers.
		//!\~french		Dit si l'API de rendu choisie supporte les tampons de stockage shader.
		eShaderStorageBuffers = 0x00000040,
		//!\~english	Tells whether or not the selected render API supports atomic counter buffers.
		//!\~french		Dit si l'API de rendu choisie supporte les tampons de compteurs atomiques.
		eAtomicCounterBuffers = 0x00000080,
		//!\~english	Tells whether or not the selected render API supports transform feedback.
		//!\~french		Dit si l'API de rendu choisie supporte le transform feedback.
		eTransformFeedback = 0x00000100,
		//!\~english	Tells whether or not the selected render API supports texture immutable storages.
		//!\~french		Dit si l'API de rendu choisie supporte les stockage immuables pour les textures.
		eImmutableTextureStorage = 0x00000200,
	};
	IMPLEMENT_FLAGS( GpuFeature )
	/*!
	\author 	Sylvain DOREMUS
	\date 		04/05/2016
	\version	0.9.0
	\~english
	\brief		All supported GPU feature flags.
	\~french
	\brief		Indicateurs de caractérisituqes du GPU.
	*/
	enum class GpuMin
		: uint32_t
	{
		eMapBufferAlignment,
		eProgramTexelOffset,

		CASTOR_SCOPED_ENUM_BOUNDS( eMapBufferAlignment )
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		04/05/2016
	\version	0.9.0
	\~english
	\brief		All supported GPU feature flags.
	\~french
	\brief		Indicateurs de caractérisituqes du GPU.
	*/
	enum class GpuMax
		: uint32_t
	{
		eShaderStorageBufferBindings,

		eVertexShaderStorageBlocks,
		eVertexAtomicCounters,
		eVertexAttribs,
		eVertexOutputComponents,
		eVertexTextureImageUnits,
		eVertexUniformComponents,
		eVertexUniformBlocks,
		eVertexUniformVectors,
		eVertexCombinedUniformComponents,

		eGeometryAtomicCounters,
		eGeometryShaderStorageBlocks,
		eGeometryInputComponents,
		eGeometryOutputComponents,
		eGeometryTextureImageUnits,
		eGeometryUniformComponents,
		eGeometryUniformBlocks,
		eGeometryCombinedUniformComponents,

		eTessControlAtomicCounters,
		eTessControlShaderStorageBlocks,

		eTessEvaluationAtomicCounters,
		eTessEvaluationShaderStorageBlocks,

		eComputeAtomicCounters,
		eComputeAtomicCounterBuffers,
		eComputeShaderStorageBlocks,
		eComputeTextureImageUnits,
		eComputeUniformComponents,
		eComputeUniformBlocks,
		eComputeCombinedUniformComponents,
		eComputeWorkGroupInvocations,
		eComputeWorkGroupCount,
		eComputeWorkGroupSize,

		eFragmentAtomicCounters,
		eFragmentShaderStorageBlocks,
		eFragmentInputComponents,
		eFragmentUniformComponents,
		eFragmentUniformBlocks,
		eFragmentUniformVectors,
		eFragmentCombinedUniformComponents,

		eCombinedAtomicCounters,
		eCombinedShaderStorageBlocks,

		eDebugGroupStackDepth,
		eLabelLength,
		eIntegerSamples,
		eProgramTexelOffset,
		eServerWaitTimeout,
		eSampleMaskWords,
		eSamples,

		eTexture3DSize,
		eTextureRectangleSize,
		eTextureCubeMapSize,
		eTextureBufferSize,
		eTextureSize,
		eTextureImageUnits,
		eTextureLodBias,

		eArrayTextureLayers,
		eClipDistances,
		eColourTextureSamples,
		eDepthTextureSamples,

		eCombinedTextureImageUnits,
		eCombinedUniformBlocks,
		eDrawBuffers,
		eDualSourceDrawBuffers,
		eElementIndices,
		eElementVertices,

		eFramebufferWidth,
		eFramebufferHeight,
		eFramebufferLayers,
		eFramebufferSamples,
		eRenderbufferSize,

		eUniformBufferBindings,
		eUniformBlockSize,
		eUniformLocations,

		eVaryingComponents,
		eVaryingVectors,
		eVaryingFloats,

		eViewportWidth,
		eViewportHeight,
		eViewports,

		CASTOR_SCOPED_ENUM_BOUNDS( eShaderStorageBufferBindings )
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		04/05/2016
	\version	0.9.0
	\~english
	\brief		Holds GPU informations.
	\~french
	\brief		Contient des informations sur le GPU.
	*/
	class GpuInformations
	{
		friend class Context;

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\~french
		 *\brief		Constructeur.
		 */
		inline GpuInformations()
			: m_useShader
			{
				{ renderer::ShaderStageFlag::eVertex, false },
				{ renderer::ShaderStageFlag::eTessellationControl, false },
				{ renderer::ShaderStageFlag::eTessellationEvaluation, false },
				{ renderer::ShaderStageFlag::eGeometry, false },
				{ renderer::ShaderStageFlag::eFragment, false },
				{ renderer::ShaderStageFlag::eCompute, false },
			}
		{
			for ( auto i = 0u; i < uint32_t( GpuMax::eCount ); ++i )
			{
				m_maxValues.insert( { GpuMax( i ), std::numeric_limits< int32_t >::lowest() } );
			}

			for ( auto i = 0u; i < uint32_t( GpuMin::eCount ); ++i )
			{
				m_minValues.insert( { GpuMin( i ), std::numeric_limits< int32_t >::max() } );
			}
		}
		/**
		 *\~english
		 *\brief		adds a supported feature.
		 *\~french
		 *\brief		Ajoute une caractéristique supportée.
		 */
		inline void addFeature( GpuFeature p_feature )
		{
			m_features |= uint32_t( p_feature );
		}
		/**
		 *\~english
		 *\brief		Removes a supported feature.
		 *\~french
		 *\brief		Enlève une caractéristique supportée.
		 */
		inline void removeFeature( GpuFeature p_feature )
		{
			m_features &= ~uint32_t( p_feature );
		}
		/**
		 *\~english
		 *\brief		Updates the support for a feature.
		 *\~french
		 *\brief		Met à jour le support d'une caractéristique.
		 */
		inline void updateFeature( GpuFeature p_feature, bool p_supported )
		{
			p_supported ? addFeature( p_feature ) : removeFeature( p_feature );
		}
		/**
		 *\~english
		 *\brief		Tells if the feature is supported supports stereo
		 *\~french
		 *\brief		Dit si la caractéristique est supportée.
		 */
		inline bool hasFeature( GpuFeature p_feature )const
		{
			return GpuFeature( m_features & uint32_t( p_feature ) ) == p_feature;
		}
		/**
		 *\~english
		 *\return		The stereo support status.
		 *\~french
		 *\return		Le statut du support de la stéréo.
		 */
		inline bool isStereoAvailable()const
		{
			return hasFeature( GpuFeature::eStereo );
		}
		/**
		 *\~english
		 *\return		The instanced draw calls support status.
		 *\~french
		 *\return		Le statut du support de l'instanciation.
		 */
		inline bool hasInstancing()const
		{
			return hasFeature( GpuFeature::eInstancing );
		}
		/**
		 *\~english
		 *\return		The accumulation buffer support status.
		 *\~french
		 *\return		Le statut du support du buffer d'accumulation.
		 */
		inline bool hasAccumulationBuffer()const
		{
			return hasFeature( GpuFeature::eAccumulationBuffer );
		}
		/**
		 *\~english
		 *\return		The non power of two textures support status.
		 *\~french
		 *\return		Le statut du support des textures non puissance de deux.
		 */
		inline bool hasNonPowerOfTwoTextures()const
		{
			return hasFeature( GpuFeature::eNonPowerOfTwoTextures );
		}
		/**
		 *\~english
		 *\return		The constant buffers support status.
		 *\~french
		 *\return		Le statut du support des tampons de constantes.
		 */
		inline bool hasConstantsBuffers()const
		{
			return hasFeature( GpuFeature::eConstantsBuffers );
		}
		/**
		 *\~english
		 *\return		The texture buffers support status.
		 *\~french
		 *\return		Le statut du support des tampons de textures.
		 */
		inline bool hasTextureBuffers()const
		{
			return hasFeature( GpuFeature::eTextureBuffers );
		}
		/**
		 *\~english
		 *\return		The SSBO support status.
		 *\~french
		 *\return		Le statut du support des SSBO.
		 */
		inline bool hasShaderStorageBuffers()const
		{
			return hasFeature( GpuFeature::eShaderStorageBuffers );
		}
		/**
		 *\~english
		 *\brief		Checks support for given shader model.
		 *\param[in]	p_model	The shader model.
		 *\return		\p false if the given model is not supported by current API.
		 *\~french
		 *\brief		Vérifie le support d'un modèle de shaders.
		 *\param[in]	p_model	Le modèle de shaders.
		 *\return		\p false si le modèle donné n'est pas supporté par l'API actuelle.
		 */
		inline bool checkSupport( ShaderModel p_model )const
		{
			return p_model <= m_maxShaderModel;
		}
		/**
		 *\~english
		 *\return		The maximum supported shader model.
		 *\~french
		 *\return		Le modèle de shader maximal supporté.
		 */
		inline ShaderModel getMaxShaderModel()const
		{
			return m_maxShaderModel;
		}
		/**
		 *\~english
		 *\return		The maximum supported shader model.
		 *\~french
		 *\return		Le modèle de shader maximal supporté.
		 */
		inline void updateMaxShaderModel()
		{
			if ( hasShaderType( renderer::ShaderStageFlag::eCompute ) )
			{
				m_maxShaderModel = ShaderModel::eModel5;
			}
			else if ( hasShaderType( renderer::ShaderStageFlag::eTessellationEvaluation ) )
			{
				m_maxShaderModel = ShaderModel::eModel4;
			}
			else if ( hasShaderType( renderer::ShaderStageFlag::eGeometry ) )
			{
				m_maxShaderModel = ShaderModel::eModel3;
			}
			else if ( hasShaderType( renderer::ShaderStageFlag::eFragment ) )
			{
				m_maxShaderModel = ShaderModel::eModel2;
			}
			else
			{
				m_maxShaderModel = ShaderModel::eModel1;
			}
		}
		/**
		 *\~english
		 *\param[in]	type	The shader type.
		 *\return		The shader type support status.
		 *\~french
		 *\param[in]	type	Le type de shader.
		 *\return		Le statut du support du type de shader.
		 */
		inline bool hasShaderType( renderer::ShaderStageFlag type )const
		{
			return m_useShader.at( type );
		}
		/**
		 *\~english
		 *\brief		Defines the support for given shader type.
		 *\param[in]	p_type	The shader type.
		 *\param[in]	p_value	The new value.
		 *\~french
		 *\brief		Définit le support du type de shader donné.
		 *\param[in]	p_type	Le type de shader.
		 *\param[in]	p_value	La nouvelle valeur.
		 */
		inline void useShaderType( renderer::ShaderStageFlag type, bool value )
		{
			m_useShader[type] = value;
		}
		/**
		 *\~english
		 *\return		The shader language version.
		 *\~french
		 *\return		La version du langage shader.
		 */
		inline uint32_t getShaderLanguageVersion()const
		{
			return m_shaderLanguageVersion;
		}
		/**
		 *\~english
		 *\brief		Defines the shader language version.
		 *\param[in]	p_value	The version.
		 *\~french
		 *\brief		Définit la version du langage shader.
		 *\param[in]	p_value	La version.
		 */
		inline void setShaderLanguageVersion( uint32_t p_value )
		{
			m_shaderLanguageVersion = p_value;
		}
		/**
		 *\~english
		 *\param[in]	p_index	The index.
		 *\return		The minimum value for given index.
		 *\~french
		 *\param[in]	p_index	L'index.
		 *\return		La valeur minimale pour l'index défini.
		 */
		inline int32_t getMinValue( GpuMin p_index )const
		{
			return m_minValues.find( p_index )->second;
		}
		/**
		 *\~english
		 *\param[in]	p_index	The index.
		 *\param[in]	p_value	The minimum value for given index.
		 *\~french
		 *\param[in]	p_index	L'index.
		 *\param[in]	p_value	La valeur minimale pour l'index défini.
		 */
		inline void setMinValue( GpuMin p_index, int32_t p_value )
		{
			m_minValues[p_index] = p_value;
		}
		/**
		 *\~english
		 *\param[in]	p_index	The index.
		 *\return		The maximum value for given index.
		 *\~french
		 *\param[in]	p_index	L'index.
		 *\return		La valeur maximale pour l'index défini.
		 */
		inline int32_t getMaxValue( GpuMax p_index )const
		{
			return m_maxValues.find( p_index )->second;
		}
		/**
		 *\~english
		 *\param[in]	p_index	The index.
		 *\param[in]	p_value	The maximum value for given index.
		 *\~french
		 *\param[in]	p_index	L'index.
		 *\param[in]	p_value	La valeur maximale pour l'index défini.
		 */
		inline void setMaxValue( GpuMax p_index, int32_t p_value )
		{
			m_maxValues[p_index] = p_value;
		}
		/**
		 *\~english
		 *\return		The total VRAM size.
		 *\~french
		 *\return		La taille totale de la VRAM.
		 */
		inline uint32_t getTotalMemorySize()const
		{
			return m_totalMemorySize;
		}
		/**
		 *\~english
		 *\param[in]	p_value	The total VRAM size.
		 *\~french
		 *\param[in]	p_value	La taille totale de la VRAM.
		 */
		inline void setTotalMemorySize( uint32_t p_value )
		{
			m_totalMemorySize = p_value;
		}
		/**
		 *\~english
		 *\return		The GPU vendor name.
		 *\~french
		 *\return		Le nom du vendeur du GPU.
		 */
		inline castor::String const & getVendor()const
		{
			return m_vendor;
		}
		/**
		 *\~english
		 *\param[in]	p_value	The GPU vendor name.
		 *\~french
		 *\param[in]	p_value	Le nom du vendeur du GPU.
		 */
		inline void setVendor( castor::String const & p_value )
		{
			m_vendor = p_value;
		}
		/**
		 *\~english
		 *\return		The GPU platform.
		 *\~french
		 *\return		Le type de GPU.
		 */
		inline castor::String const & getRenderer()const
		{
			return m_renderer;
		}
		/**
		 *\~english
		 *\param[in]	p_value	The GPU platform.
		 *\~french
		 *\param[in]	p_value	Le type de GPU.
		 */
		inline void setRenderer( castor::String const & p_value )
		{
			m_renderer = p_value;
		}
		/**
		 *\~english
		 *\return		The rendering API version.
		 *\~french
		 *\return		La version de l'API de rendu.
		 */
		inline castor::String const & getVersion()const
		{
			return m_version;
		}
		/**
		 *\~english
		 *\param[in]	p_value	The rendering API version.
		 *\~french
		 *\param[in]	p_value	La version de l'API de rendu.
		 */
		inline void setVersion( castor::String const & p_value )
		{
			m_version = p_value;
		}

	protected:
		//!\~english	Combination of GpuFeature.
		//!\~french		Combinaison de GpuFeature.
		uint32_t m_features{ false };
		//!\~english	The maximum supported shader model.
		//!\~french		Le modèle de shader maximum supporté.
		ShaderModel m_maxShaderModel{ ShaderModel::eMin };
		//!\~english	The shader language version.
		//!\~french		La version du langage de shader.
		uint32_t m_shaderLanguageVersion{ 0 };
		//!\~english	Tells which types of shaders are supported.
		//!\~french		Dit quel type de shaders sont supportés.
		std::map< renderer::ShaderStageFlag, bool > m_useShader;
		//!\~english	The minimum values.
		//!\~french		Les valeurs minimales.
		std::map< GpuMin, int32_t > m_minValues;
		//!\~english	The maximum values.
		//!\~french		Les valeurs maximales.
		std::map< GpuMax, int32_t > m_maxValues;
		//!\~english	The total VRAM size.
		//!\~french		La taille totale de la VRAM.
		uint32_t m_totalMemorySize;
		//!\~english	The GPU vendor name.
		//!\~french		Le nom du vendeur du GPU.
		castor::String m_vendor;
		//!\~english	The GPU platform.
		//!\~french		Le type de GPU.
		castor::String m_renderer;
		//!\~english	The rendering API version.
		//!\~french		La version de l'API de rendu.
		castor::String m_version;
	};
}

#endif
