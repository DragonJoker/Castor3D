/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___C3D_GPU_INFORMATIONS_H___
#define ___C3D_GPU_INFORMATIONS_H___

#include "Render/Context.hpp"

#include <OwnedBy.hpp>

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
		Instancing = 0x00000001,
		//!\~english Tells whether or not the selected render API supports accumulation buffers	\~french Dit si l'API de rendu choisie supporte le buffer d'accumulation
		AccumulationBuffer = 0x00000002,
		//!\~english Tells whether or not the selected render API supports non power of two textures	\~french Dit si l'API de rendu choisie supporte les textures non puissance de 2
		NonPowerOfTwoTextures = 0x00000004,
		//!\~english Tells whether or not the selected render API supports stereo	\~french Dit si l'API de rendu choisie supporte la stéréographie.
		Stereo = 0x00000008,
		//!\~english Tells whether or not the selected render API supports constants buffers.	\~french Dit si l'API de rendu choisie supporte les tampons de constantes.
		ConstantsBuffers = 0x00000010,
		//!\~english Tells whether or not the selected render API supports texture buffers.	\~french Dit si l'API de rendu choisie supporte les tampons de textures.
		TextureBuffers = 0x00000020,
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
	enum class GpuMin
		: uint32_t
	{
		MapBufferAlignment,
		ProgramTexelOffset,

		CASTOR_ENUM_CLASS_BOUNDS( MapBufferAlignment )
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
		ShaderStorageBufferBindings,

		VertexShaderStorageBlocks,
		VertexAtomicCounters,
		VertexAttribs,
		VertexOutputComponents,
		VertexTextureImageUnits,
		VertexUniformComponents,
		VertexUniformBlocks,
		VertexUniformVectors,
		VertexCombinedUniformComponents,

		GeometryAtomicCounters,
		GeometryShaderStorageBlocks,
		GeometryInputComponents,
		GeometryOutputComponents,
		GeometryTextureImageUnits,
		GeometryUniformComponents,
		GeometryUniformBlocks,
		GeometryCombinedUniformComponents,

		TessControlAtomicCounters,
		TessControlShaderStorageBlocks,

		TessEvaluationAtomicCounters,
		TessEvaluationShaderStorageBlocks,

		ComputeAtomicCounters,
		ComputeAtomicCounterBuffers,
		ComputeShaderStorageBlocks,
		ComputeTextureImageUnits,
		ComputeUniformComponents,
		ComputeUniformBlocks,
		ComputeCombinedUniformComponents,
		ComputeWorkGroupInvocations,
		ComputeWorkGroupCount,
		ComputeWorkGroupSize,

		FragmentAtomicCounters,
		FragmentShaderStorageBlocks,
		FragmentInputComponents,
		FragmentUniformComponents,
		FragmentUniformBlocks,
		FragmentUniformVectors,
		FragmentCombinedUniformComponents,

		CombinedAtomicCounters,
		CombinedShaderStorageBlocks,

		DebugGroupStackDepth,
		LabelLength,
		IntegerSamples,
		ProgramTexelOffset,
		ServerWaitTimeout,
		SampleMaskWords,
		Samples,

		Texture3DSize,
		TextureRectangleSize,
		TextureCubeMapSize,
		TextureBufferSize,
		TextureSize,
		TextureImageUnits,
		TextureLodBias,

		ArrayTextureLayers,
		ClipDistances,
		ColourTextureSamples,
		DepthTextureSamples,

		CombinedTextureImageUnits,
		CombinedUniformBlocks,
		DrawBuffers,
		DualSourceDrawBuffers,
		ElementIndices,
		ElementVertices,

		FramebufferWidth,
		FramebufferHeight,
		FramebufferLayers,
		FramebufferSamples,
		RenderbufferSize,

		UniformBufferBindings,
		UniformBlockSize,
		UniformLocations,

		VaryingComponents,
		VaryingVectors,
		VaryingFloats,

		ViewportWidth,
		ViewportHeight,
		Viewports,

		CASTOR_ENUM_CLASS_BOUNDS( ShaderStorageBufferBindings )
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
			for ( auto i = 0u; i < uint32_t( GpuMax::Count ); ++i )
			{
				m_maxValues.insert( { GpuMax( i ), std::numeric_limits< int32_t >::lowest() } );
			}

			for ( auto i = 0u; i < uint32_t( GpuMin::Count ); ++i )
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
			return HasFeature( GpuFeature::Stereo );
		}
		/**
		 *\~english
		 *\return		The instanced draw calls support status.
		 *\~french
		 *\return		Le statut du support de l'instanciation.
		 */
		inline bool HasInstancing()const
		{
			return HasFeature( GpuFeature::Instancing );
		}
		/**
		 *\~english
		 *\return		The accumulation buffer support status.
		 *\~french
		 *\return		Le statut du support du buffer d'accumulation.
		 */
		inline bool HasAccumulationBuffer()const
		{
			return HasFeature( GpuFeature::AccumulationBuffer );
		}
		/**
		 *\~english
		 *\return		The non power of two textures support status.
		 *\~french
		 *\return		Le statut du support des textures non puissance de deux.
		 */
		inline bool HasNonPowerOfTwoTextures()const
		{
			return HasFeature( GpuFeature::NonPowerOfTwoTextures );
		}
		/**
		 *\~english
		 *\return		The constant buffers support status.
		 *\~french
		 *\return		Le statut du support des tampons de constantes.
		 */
		inline bool HasConstantsBuffers()const
		{
			return HasFeature( GpuFeature::ConstantsBuffers );
		}
		/**
		 *\~english
		 *\return		The texture buffers support status.
		 *\~french
		 *\return		Le statut du support des tampons de textures.
		 */
		inline bool HasTextureBuffers()const
		{
			return HasFeature( GpuFeature::TextureBuffers );
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
		inline bool CheckSupport( eSHADER_MODEL p_model )const
		{
			return p_model <= m_maxShaderModel;
		}
		/**
		 *\~english
		 *\return		The maximum supported shader model.
		 *\~french
		 *\return		Le modèle de shader maximal supporté.
		 */
		inline eSHADER_MODEL GetMaxShaderModel()const
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
			if ( m_useShader[eSHADER_TYPE_COMPUTE] )
			{
				m_maxShaderModel = eSHADER_MODEL_5;
			}
			else if ( m_useShader[eSHADER_TYPE_HULL] )
			{
				m_maxShaderModel = eSHADER_MODEL_4;
			}
			else if ( m_useShader[eSHADER_TYPE_GEOMETRY] )
			{
				m_maxShaderModel = eSHADER_MODEL_3;
			}
			else if ( m_useShader[eSHADER_TYPE_PIXEL] )
			{
				m_maxShaderModel = eSHADER_MODEL_2;
			}
			else
			{
				m_maxShaderModel = eSHADER_MODEL_1;
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
		inline bool HasShaderType( eSHADER_TYPE p_type )const
		{
			return m_useShader[p_type];
		}
		/**
		 *\~english
		 *\brief		Defines the support for given shader type.
		 *\param[in]	p_type	The shader type.
		 *\~french
		 *\brief		Définit le support du type de shader donné.
		 *\param[in]	p_type	Le type de shader.
		 */
		inline void UseShaderType( eSHADER_TYPE p_type, bool p_value )
		{
			m_useShader[p_type] = p_value;
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
		eSHADER_MODEL m_maxShaderModel{ eSHADER_MODEL_MIN };
		//!\~english The shader language version.	\~french La version du langage de shader.
		uint32_t m_shaderLanguageVersion{ 0 };
		//!\~english Tells which types of shaders are supported	\~french Dit quel type de shaders sont supportés
		std::array< bool, eSHADER_TYPE_COUNT > m_useShader;
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
