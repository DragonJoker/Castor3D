/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
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

namespace Castor3D
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
		//!\~english Tells whether or not the selected render API supports instanced draw calls	\~french Dit si l'API de rendu choisie supporte le dessin instancié
		eInstancing = 0x00000001,
		//!\~english Tells whether or not the selected render API supports accumulation buffers	\~french Dit si l'API de rendu choisie supporte le buffer d'accumulation
		eAccumulationBuffer = 0x00000002,
		//!\~english Tells whether or not the selected render API supports non power of two textures	\~french Dit si l'API de rendu choisie supporte les textures non puissance de 2
		eNonPowerOfTwoTextures = 0x00000004,
		//!\~english Tells whether or not the selected render API supports stereo	\~french Dit si l'API de rendu choisie supporte la stéréographie.
		eStereo = 0x00000008,
		//!\~english Tells whether or not the selected render API supports constants buffers.	\~french Dit si l'API de rendu choisie supporte les tampons de constantes.
		eConstantsBuffers = 0x00000010,
		//!\~english Tells whether or not the selected render API supports texture buffers.	\~french Dit si l'API de rendu choisie supporte les tampons de textures.
		eTextureBuffers = 0x00000020,
	};
	IMPLEMENT_FLAGS( GpuFeature );
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
		{
			for ( auto i = 0u; i < uint32_t( GpuMax::eCount ); ++i )
			{
				m_maxValues.insert( { GpuMax( i ), std::numeric_limits< int32_t >::lowest() } );
			}

			for ( auto i = 0u; i < uint32_t( GpuMin::eCount ); ++i )
			{
				m_minValues.insert( { GpuMin( i ), std::numeric_limits< int32_t >::max() } );
			}

			for ( auto & l_value : m_useShader )
			{
				l_value = false;
			}
		}
		/**
		 *\~english
		 *\brief		Adds a supported feature.
		 *\~french
		 *\brief		Ajoute une caractéristique supportée.
		 */
		inline void AddFeature( GpuFeature p_feature )
		{
			m_features |= uint32_t( p_feature );
		}
		/**
		 *\~english
		 *\brief		Removes a supported feature.
		 *\~french
		 *\brief		Enlève une caractéristique supportée.
		 */
		inline void RemoveFeature( GpuFeature p_feature )
		{
			m_features |= uint32_t( p_feature );
		}
		/**
		 *\~english
		 *\brief		Updates the support for a feature.
		 *\~french
		 *\brief		Met à jour le support d'une caractéristique.
		 */
		inline void UpdateFeature( GpuFeature p_feature, bool p_supported )
		{
			p_supported ? AddFeature( p_feature ) : RemoveFeature( p_feature );
		}
		/**
		 *\~english
		 *\brief		Tells if the feature is supported supports stereo
		 *\~french
		 *\brief		Dit si la caractéristique est supportée.
		 */
		inline bool HasFeature( GpuFeature p_feature )const
		{
			return GpuFeature( m_features & uint32_t( p_feature ) ) == p_feature;
		}
		/**
		 *\~english
		 *\return		The stereo support status.
		 *\~french
		 *\return		Le statut du support de la stéréo.
		 */
		inline bool IsStereoAvailable()const
		{
			return HasFeature( GpuFeature::eStereo );
		}
		/**
		 *\~english
		 *\return		The instanced draw calls support status.
		 *\~french
		 *\return		Le statut du support de l'instanciation.
		 */
		inline bool HasInstancing()const
		{
			return HasFeature( GpuFeature::eInstancing );
		}
		/**
		 *\~english
		 *\return		The accumulation buffer support status.
		 *\~french
		 *\return		Le statut du support du buffer d'accumulation.
		 */
		inline bool HasAccumulationBuffer()const
		{
			return HasFeature( GpuFeature::eAccumulationBuffer );
		}
		/**
		 *\~english
		 *\return		The non power of two textures support status.
		 *\~french
		 *\return		Le statut du support des textures non puissance de deux.
		 */
		inline bool HasNonPowerOfTwoTextures()const
		{
			return HasFeature( GpuFeature::eNonPowerOfTwoTextures );
		}
		/**
		 *\~english
		 *\return		The constant buffers support status.
		 *\~french
		 *\return		Le statut du support des tampons de constantes.
		 */
		inline bool HasConstantsBuffers()const
		{
			return HasFeature( GpuFeature::eConstantsBuffers );
		}
		/**
		 *\~english
		 *\return		The texture buffers support status.
		 *\~french
		 *\return		Le statut du support des tampons de textures.
		 */
		inline bool HasTextureBuffers()const
		{
			return HasFeature( GpuFeature::eTextureBuffers );
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
		inline bool CheckSupport( ShaderModel p_model )const
		{
			return p_model <= m_maxShaderModel;
		}
		/**
		 *\~english
		 *\return		The maximum supported shader model.
		 *\~french
		 *\return		Le modèle de shader maximal supporté.
		 */
		inline ShaderModel GetMaxShaderModel()const
		{
			return m_maxShaderModel;
		}
		/**
		 *\~english
		 *\return		The maximum supported shader model.
		 *\~french
		 *\return		Le modèle de shader maximal supporté.
		 */
		inline void UpdateMaxShaderModel()
		{
			if ( m_useShader[size_t( ShaderType::eCompute )] )
			{
				m_maxShaderModel = ShaderModel::eModel5;
			}
			else if ( m_useShader[size_t( ShaderType::eHull )] )
			{
				m_maxShaderModel = ShaderModel::eModel4;
			}
			else if ( m_useShader[size_t( ShaderType::eGeometry )] )
			{
				m_maxShaderModel = ShaderModel::eModel3;
			}
			else if ( m_useShader[size_t( ShaderType::ePixel )] )
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
		 *\param[in]	p_type	The shader type.
		 *\return		The shader type support status.
		 *\~french
		 *\param[in]	p_type	Le type de shader.
		 *\return		Le statut du support du type de shader.
		 */
		inline bool HasShaderType( ShaderType p_type )const
		{
			return m_useShader[size_t( p_type )];
		}
		/**
		 *\~english
		 *\brief		Defines the support for given shader type.
		 *\param[in]	p_type	The shader type.
		 *\~french
		 *\brief		Définit le support du type de shader donné.
		 *\param[in]	p_type	Le type de shader.
		 */
		inline void UseShaderType( ShaderType p_type, bool p_value )
		{
			m_useShader[size_t( p_type )] = p_value;
		}
		/**
		 *\~english
		 *\return		The shader language version.
		 *\~french
		 *\return		La version du langage shader.
		 */
		inline uint32_t GetShaderLanguageVersion()const
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
		inline void SetShaderLanguageVersion( uint32_t p_value )
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
		inline int32_t GetMinValue( GpuMin p_index )const
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
		inline void SetMinValue( GpuMin p_index, int32_t p_value )
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
		inline int32_t GetMaxValue( GpuMax p_index )const
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
		inline void SetMaxValue( GpuMax p_index, int32_t p_value )
		{
			m_maxValues[p_index] = p_value;
		}
		/**
		 *\~english
		 *\return		The total VRAM size.
		 *\~french
		 *\return		La taille totale de la VRAM.
		 */
		inline uint32_t GetTotalMemorySize()const
		{
			return m_totalMemorySize;
		}
		/**
		 *\~english
		 *\param[in]	p_value	The total VRAM size.
		 *\~french
		 *\param[in]	p_value	La taille totale de la VRAM.
		 */
		inline void SetTotalMemorySize( uint32_t p_value )
		{
			m_totalMemorySize = p_value;
		}
		/**
		 *\~english
		 *\return		The GPU vendor name.
		 *\~french
		 *\return		Le nom du vendeur du GPU.
		 */
		inline Castor::String const & GetVendor()const
		{
			return m_vendor;
		}
		/**
		 *\~english
		 *\param[in]	p_value	The GPU vendor name.
		 *\~french
		 *\param[in]	p_value	Le nom du vendeur du GPU.
		 */
		inline void SetVendor( Castor::String const & p_value )
		{
			m_vendor = p_value;
		}
		/**
		 *\~english
		 *\return		The GPU platform.
		 *\~french
		 *\return		Le type de GPU.
		 */
		inline Castor::String const & GetRenderer()const
		{
			return m_renderer;
		}
		/**
		 *\~english
		 *\param[in]	p_value	The GPU platform.
		 *\~french
		 *\param[in]	p_value	Le type de GPU.
		 */
		inline void SetRenderer( Castor::String const & p_value )
		{
			m_renderer = p_value;
		}
		/**
		 *\~english
		 *\return		The rendering API version.
		 *\~french
		 *\return		La version de l'API de rendu.
		 */
		inline Castor::String const & GetVersion()const
		{
			return m_version;
		}
		/**
		 *\~english
		 *\param[in]	p_value	The rendering API version.
		 *\~french
		 *\param[in]	p_value	La version de l'API de rendu.
		 */
		inline void SetVersion( Castor::String const & p_value )
		{
			m_version = p_value;
		}

	protected:
		//!\~english Combination of GpuFeature.	\~french Combinaisond e GpuFeature.
		uint32_t m_features{ false };
		//!\~english The maximum supported shader model.	\~french Le modèle de shader maximum supporté.
		ShaderModel m_maxShaderModel{ ShaderModel::eMin };
		//!\~english The shader language version.	\~french La version du langage de shader.
		uint32_t m_shaderLanguageVersion{ 0 };
		//!\~english Tells which types of shaders are supported	\~french Dit quel type de shaders sont supportés
		std::array< bool, size_t( ShaderType::eCount ) > m_useShader;
		//!\~english The minimum values.	\~french Les valeurs minimales.
		std::map< GpuMin, int32_t > m_minValues;
		//!\~english The maximum values.	\~french Les valeurs maximales.
		std::map< GpuMax, int32_t > m_maxValues;
		//!\~english The total VRAM size.	\~french La taille totale de la VRAM.
		uint32_t m_totalMemorySize;
		//!\~english The GPU vendor name.	\~french Le nom du vendeur du GPU.
		Castor::String m_vendor;
		//!\~english The GPU platform.	\~french Le type de GPU.
		Castor::String m_renderer;
		//!\~english The rendering API version.	\~french La version de l'API de rendu.
		Castor::String m_version;
	};
}

#endif
