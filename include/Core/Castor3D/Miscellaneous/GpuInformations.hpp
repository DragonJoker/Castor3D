/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GpuInformations_H___
#define ___C3D_GpuInformations_H___

#include "MiscellaneousModule.hpp"

namespace c3d
{
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
		GpuInformations()
		{
			for ( auto i = 0u; i < uint32_t( GpuMax::eCount ); ++i )
			{
				m_maxValues.emplace( GpuMax( i ), std::numeric_limits< int32_t >::lowest() );
			}
		}
		/**
		 *\~english
		 *\brief		Adds a supported feature.
		 *\~french
		 *\brief		Ajoute une caractéristique supportée.
		 */
		void addFeature( GpuFeature feature )noexcept
		{
			addFlag( m_features, feature );
		}
		/**
		 *\~english
		 *\brief		Removes a supported feature.
		 *\~french
		 *\brief		Enlève une caractéristique supportée.
		 */
		void removeFeature( GpuFeature feature )noexcept
		{
			remFlag( m_features, feature );
		}
		/**
		 *\~english
		 *\brief		Updates the support for a feature.
		 *\~french
		 *\brief		Met à jour le support d'une caractéristique.
		 */
		void updateFeature( GpuFeature feature, bool supported )noexcept
		{
			supported
				? addFeature( feature )
				: removeFeature( feature );
		}
		/**
		 *\~english
		 *\brief		Tells if the feature is supported supports stereo
		 *\~french
		 *\brief		Dit si la caractéristique est supportée.
		 */
		bool hasFeature( GpuFeature feature )const noexcept
		{
			return checkFlag( m_features, feature );
		}
		/**
		 *\~english
		 *\return		The stereo support status.
		 *\~french
		 *\return		Le statut du support de la stéréo.
		 */
		bool hasStereoRendering()const noexcept
		{
			return hasFeature( GpuFeature::eStereoRendering );
		}
		/**
		 *\~english
		 *\return		The SSBO support status.
		 *\~french
		 *\return		Le statut du support des SSBO.
		 */
		bool hasShaderStorageBuffers()const noexcept
		{
			return hasFeature( GpuFeature::eShaderStorageBuffers );
		}
		/**
		 *\~english
		 *\param[in]	type	The shader type.
		 *\return		The shader type support status.
		 *\~french
		 *\param[in]	type	Le type de shader.
		 *\return		Le statut du support du type de shader.
		 */
		bool hasShaderType( VkShaderStageFlagBits type )const noexcept
		{
			return m_useShader.at( type );
		}
		/**
		 *\~english
		 *\brief		Defines the support for given shader type.
		 *\param[in]	type	The shader type.
		 *\param[in]	value	The new value.
		 *\~french
		 *\brief		Définit le support du type de shader donné.
		 *\param[in]	type	Le type de shader.
		 *\param[in]	value	La nouvelle valeur.
		 */
		void useShaderType( VkShaderStageFlagBits type, bool value )noexcept
		{
			m_useShader[type] = value;
		}
		/**
		 *\~english
		 *\param[in]	index	The index.
		 *\return		The minimum value for given index.
		 *\~french
		 *\param[in]	index	L'index.
		 *\return		La valeur minimale pour l'index défini.
		 */
		uint32_t getValue( GpuMin index )const noexcept
		{
			return m_minValues.find( index )->second;
		}
		/**
		 *\~english
		 *\param[in]	index	The index.
		 *\param[in]	value	The minimum value for given index.
		 *\~french
		 *\param[in]	index	L'index.
		 *\param[in]	value	La valeur minimale pour l'index défini.
		 */
		void setValue( GpuMin index, uint32_t value )noexcept
		{
			m_minValues[index] = value;
		}
		/**
		 *\~english
		 *\param[in]	index	The index.
		 *\return		The maximum value for given index.
		 *\~french
		 *\param[in]	index	L'index.
		 *\return		La valeur maximale pour l'index défini.
		 */
		uint32_t getValue( GpuMax index )const noexcept
		{
			return m_maxValues.find( index )->second;
		}
		/**
		 *\~english
		 *\param[in]	index	The index.
		 *\param[in]	value	The maximum value for given index.
		 *\~french
		 *\param[in]	index	L'index.
		 *\param[in]	value	La valeur maximale pour l'index défini.
		 */
		void setValue( GpuMax index, uint32_t value )noexcept
		{
			m_maxValues[index] = value;
		}
		/**
		 *\~english
		 *\return		The total VRAM size.
		 *\~french
		 *\return		La taille totale de la VRAM.
		 */
		uint32_t getTotalMemorySize()const noexcept
		{
			return m_totalMemorySize;
		}
		/**
		 *\~english
		 *\param[in]	value	The total VRAM size.
		 *\~french
		 *\param[in]	value	La taille totale de la VRAM.
		 */
		void setTotalMemorySize( uint32_t value )noexcept
		{
			m_totalMemorySize = value;
		}
		/**
		 *\~english
		 *\return		The GPU vendor name.
		 *\~french
		 *\return		Le nom du vendeur du GPU.
		 */
		StringView getVendor()const noexcept
		{
			return m_vendor;
		}
		/**
		 *\~english
		 *\param[in]	value	The GPU vendor name.
		 *\~french
		 *\param[in]	value	Le nom du vendeur du GPU.
		 */
		void setVendor( StringView value )noexcept
		{
			m_vendor = value;
		}
		/**
		 *\~english
		 *\return		The GPU platform.
		 *\~french
		 *\return		Le type de GPU.
		 */
		StringView getRenderer()const noexcept
		{
			return m_renderer;
		}
		/**
		 *\~english
		 *\param[in]	value	The GPU platform.
		 *\~french
		 *\param[in]	value	Le type de GPU.
		 */
		void setRenderer( StringView value )noexcept
		{
			m_renderer = value;
		}
		/**
		 *\~english
		 *\return		The rendering API version.
		 *\~french
		 *\return		La version de l'API de rendu.
		 */
		StringView getVersion()const noexcept
		{
			return m_version;
		}
		/**
		 *\~english
		 *\param[in]	value	The rendering API version.
		 *\~french
		 *\param[in]	value	La version de l'API de rendu.
		 */
		void setVersion( StringView value )noexcept
		{
			m_version = value;
		}
		/**
		 *\~english
		 *\return		\p true fi the GPU vendor is NVIDIA.
		 *\~french
		 *\return		\ true si le vendeur du GPU est NVIDIA.
		 */
		bool isNVIDIA()const noexcept
		{
			return m_vendor.find( "NVIDIA" ) != String::npos;
		}

	private:
		GpuFeatures m_features{ 0u };
		Map< VkShaderStageFlagBits, bool > m_useShader
		{
			{ VK_SHADER_STAGE_VERTEX_BIT, false },
			{ VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT, false },
			{ VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT, false },
			{ VK_SHADER_STAGE_GEOMETRY_BIT, false },
			{ VK_SHADER_STAGE_FRAGMENT_BIT, false },
			{ VK_SHADER_STAGE_COMPUTE_BIT, false },
		};
		Map< GpuMin, uint32_t > m_minValues;
		Map< GpuMax, uint32_t > m_maxValues;
		uint32_t m_totalMemorySize{};
		String m_vendor;
		String m_renderer;
		String m_version;

	private:
		static void print( OutputStream & stream, Map< GpuMin, uint32_t > const & object )
		{
			static Array< String, size_t( GpuMin::eCount ) > const names
			{
				cuT( "Min buffer map size" ),
				cuT( "Min uniform buffer offset alignment" ),
			};

			for ( auto const & [key, value] : object )
			{
				stream << "    " << names[size_t( key )] << ": " << value << "\n";
			}
		}

		static void print( OutputStream & stream, Map< GpuMax, uint32_t > const & object )
		{
			static Array< String, size_t( GpuMax::eCount ) > const names
			{
				cuT( "Max image 1D size" ),
				cuT( "Max image 2D size" ),
				cuT( "Max image 3D size" ),
				cuT( "Max image cube size" ),
				cuT( "Max image layers count" ),
				cuT( "Max sampler lod bias" ),
				cuT( "Max clip distances" ),
				cuT( "Max framebuffer width" ),
				cuT( "Max framebuffer height" ),
				cuT( "Max framebuffer layers" ),
				cuT( "Max framebuffer samples" ),
				cuT( "Max texel buffer range" ),
				cuT( "Max uniform buffer range" ),
				cuT( "Max storage buffer range" ),
				cuT( "Max viewport width" ),
				cuT( "Max viewport height" ),
				cuT( "Max viewports count" ),
				cuT( "Max work group count X" ),
				cuT( "Max work group count Y" ),
				cuT( "Max work group count Z" ),
				cuT( "Max work group size X" ),
				cuT( "Max work group size Y" ),
				cuT( "Max work group size Z" ),
				cuT( "Max work group invocations" ),
				cuT( "Max mesh work group invocations" ),
				cuT( "Max mesh work group size X" ),
				cuT( "Max mesh work group size Y" ),
				cuT( "Max mesh work group size Z" ),
				cuT( "Max mesh output vertices" ),
				cuT( "Max mesh output primitives" ),
				cuT( "Max task work group invocations" ),
				cuT( "Max task work group size X" ),
				cuT( "Max task work group size Y" ),
				cuT( "Max task work group size Z" ),
			};

			for ( auto const & [key, value] : object )
			{
				stream << "    " << names[size_t( key )] << ": " << value << "\n";
			}
		}

		friend OutputStream & operator<<( OutputStream & stream, GpuInformations const & object )
		{
			auto support = []( bool supported )
			{
				return ( supported ? "supported" : "not supported" );
			};
			stream << "GPU informations:" << "\n";
			stream << "    Vendor: " << object.getVendor() << "\n";
			stream << "    API name: " << object.getRenderer() << "\n";
			stream << "    API version: " << object.getVersion() << "\n";
			stream << "    Storage Buffers: " << support( object.hasFeature( GpuFeature::eShaderStorageBuffers ) ) << "\n";
			stream << "    Stereo Rendering: " << support( object.hasFeature( GpuFeature::eStereoRendering ) ) << "\n";
			stream << "    Vertex shaders: " << support( object.hasShaderType( VK_SHADER_STAGE_VERTEX_BIT ) ) << "\n";
			stream << "    Tessellation Control shaders: " << support( object.hasShaderType( VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT ) ) << "\n";
			stream << "    Tessellation Evaluation shaders: " << support( object.hasShaderType( VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT ) ) << "\n";
			stream << "    Geometry shaders: " << support( object.hasShaderType( VK_SHADER_STAGE_GEOMETRY_BIT ) ) << "\n";
			stream << "    Fragment shaders: " << support( object.hasShaderType( VK_SHADER_STAGE_FRAGMENT_BIT ) ) << "\n";
			stream << "    Compute shaders: " << support( object.hasShaderType( VK_SHADER_STAGE_COMPUTE_BIT ) ) << "\n";
			stream << "    Mesh shaders: " << support( object.hasShaderType( VK_SHADER_STAGE_MESH_BIT_NV ) ) << "\n";
			stream << "    Task shaders: " << support( object.hasShaderType( VK_SHADER_STAGE_TASK_BIT_NV ) ) << "\n";
			stream << "    Ray Generation shaders: " << support( object.hasShaderType( VK_SHADER_STAGE_RAYGEN_BIT_KHR ) ) << "\n";
			stream << "    Any Hit shaders: " << support( object.hasShaderType( VK_SHADER_STAGE_ANY_HIT_BIT_KHR ) ) << "\n";
			stream << "    Closest Hit shaders: " << support( object.hasShaderType( VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR ) ) << "\n";
			stream << "    Miss shaders: " << support( object.hasShaderType( VK_SHADER_STAGE_MISS_BIT_KHR ) ) << "\n";
			stream << "    Intersection shaders: " << support( object.hasShaderType( VK_SHADER_STAGE_INTERSECTION_BIT_KHR ) ) << "\n";
			stream << "    Callable shaders: " << support( object.hasShaderType( VK_SHADER_STAGE_CALLABLE_BIT_KHR ) ) << "\n";

			print( stream, object.m_minValues );
			print( stream, object.m_maxValues );

			stream << std::flush;
			return stream;
		}
	};
}

#endif
