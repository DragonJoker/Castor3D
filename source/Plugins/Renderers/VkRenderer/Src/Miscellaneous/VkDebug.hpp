/*
This file belongs to RendererLib.
See LICENSE file in root folder
*/
#pragma once

#include <map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

namespace vk_renderer
{
	/**
	*\~french
	*\brief
	*	Structure permettant de nommer statiquement les types Vulkan.
	*\~english
	*\brief
	*	Allows to statically name Vulkan types.
	*/
	template< typename VkType >
	struct VkTypeNamer
	{
		static std::string const Name;
	};
	/**
	*\~french
	*\brief
	*	Classe permettant de tracer les valeurs des objets vulkan.
	*\~english
	*\brief
	*	Allows to trace values for Vulkan objects.
	*/
	class Debug
	{
	public:
		/**
		*\~french
		*\brief
		*	Ecrit le contenu du dump dans un flux.
		*\param[in,out] stream
		*	Le flux.
		*\~english
		*\brief
		*	Writes the trace content to a stream.
		*\param[in,out] stream
		*	The stream.
		*/
		static inline void write( std::ostream & stream )
		{
			for ( auto const & it : m_dump )
			{
				stream << it.first << ":" << std::endl;
				stream << it.second << std::endl;
			}
		}
		/**
		*\~french
		*\brief
		*	Dump une variable Vulkan.
		*\param[in] value
		*	La valeur à dumper.
		*\~english
		*\brief
		*	Dumps a Vulkan variable.
		*\param[in] value
		*	The value to dump.
		*/
		template< typename VkType >
		static inline void dump( VkType const & value )
		{
			using Namer = VkTypeNamer< VkType >;
			m_dump.emplace( Namer::Name, subDump( value, std::string{} ) );
		}
		/**
		*\~french
		*\brief
		*	Dump un tableau de variables Vulkan.
		*\param[in] value
		*	Le vecteur de valeurs à dumper.
		*\~english
		*\brief
		*	Dumps a Vulkan variables array.
		*\param[in] value
		*	The values array to dump.
		*/
		template< typename VkType >
		static inline void dump( std::vector< VkType > const & value )
		{
			std::string dump;

			for ( auto const & l_value: value )
			{
				dump += subDump( l_value, "" );
			}

			using Namer = VkTypeNamer< VkType >;
			m_dump.emplace( Namer::Name + std::string{ " array" }, dump );
		}

	private:
		template< typename VkType >
		static inline std::string subDump( VkType const * const value, uint32_t count, std::string const & tabs )
		{
			std::stringstream dump;

			if ( value )
			{
				dump << tabs << "{" << std::endl;

				for ( auto it = value; it != value + count; ++it )
				{
					dump << subDump( *it, tabs + "\t" );
				}

				dump << tabs << "}" << std::endl;
			}
			else
			{
				dump << tabs << "\t" << "NULL" << std::endl;
			}

			return dump.str();
		}

		template< typename VkType >
		static inline std::string subDump( VkType const * const value, std::string const & tabs )
		{
			std::stringstream dump;

			if ( value )
			{
				dump << subDump( *value, tabs + "\t" ) << std::endl;
			}
			else
			{
				dump << tabs << "\t" << "NULL" << std::endl;
			}

			return dump.str();
		}

		static inline std::string subDump( char const * const value, std::string const & tabs )
		{
			std::stringstream dump;
			dump << tabs << value << std::endl;
			return dump.str();
		}

		template< typename VkType >
		static inline std::string subDump( VkType * value, std::string const & tabs )
		{
			std::stringstream dump;
			dump << tabs << value << std::endl;
			return dump.str();
		}

		static inline std::string subDump( int8_t value, std::string const & tabs )
		{
			std::stringstream dump;
			dump << tabs << value << std::endl;
			return dump.str();
		}

		static inline std::string subDump( uint8_t value, std::string const & tabs )
		{
			std::stringstream dump;
			dump << tabs << value << std::endl;
			return dump.str();
		}

		static inline std::string subDump( int16_t value, std::string const & tabs )
		{
			std::stringstream dump;
			dump << tabs << value << std::endl;
			return dump.str();
		}

		static inline std::string subDump( uint16_t value, std::string const & tabs )
		{
			std::stringstream dump;
			dump << tabs << value << std::endl;
			return dump.str();
		}

		static inline std::string subDump( int32_t value, std::string const & tabs )
		{
			std::stringstream dump;
			dump << tabs << value << std::endl;
			return dump.str();
		}

		static inline std::string subDump( uint32_t value, std::string const & tabs )
		{
			std::stringstream dump;
			dump << tabs << value << std::endl;
			return dump.str();
		}

		static inline std::string subDump( int64_t value, std::string const & tabs )
		{
			std::stringstream dump;
			dump << tabs << value << std::endl;
			return dump.str();
		}

		static inline std::string subDump( uint64_t value, std::string const & tabs )
		{
			std::stringstream dump;
			dump << tabs << value << std::endl;
			return dump.str();
		}

		static inline std::string subDump( float value, std::string const & tabs )
		{
			std::stringstream dump;
			dump << tabs << value << std::endl;
			return dump.str();
		}

		static inline std::string subDump( double value, std::string const & tabs )
		{
			std::stringstream dump;
			dump << tabs << value << std::endl;
			return dump.str();
		}

		static inline std::string subDump( VkDescriptorPoolCreateInfo const & value, std::string const & tabs )
		{
			std::stringstream dump;
			dump << tabs << "{" << std::endl;
			dump << tabs << "\t" << "sType: " << value.sType << std::endl;
			dump << tabs << "\t" << "pNext: " << value.pNext << std::endl;
			dump << tabs << "\t" << "flags: " << value.flags << std::endl;
			dump << tabs << "\t" << "maxSets: " << value.maxSets << std::endl;
			dump << tabs << "\t" << "poolSizeCount: " << value.poolSizeCount << std::endl;
			dump << tabs << "\t" << "pPoolSizes: " << std::endl << subDump( value.pPoolSizes, value.poolSizeCount, tabs + "\t" ) << std::endl;
			dump << tabs << "}" << std::endl;
			return dump.str();
		}

		static inline std::string subDump( VkWriteDescriptorSet const & value, std::string const & tabs )
		{
			std::stringstream dump;
			dump << tabs << "{" << std::endl;
			dump << tabs << "\t" << "sType: " << value.sType << std::endl;
			dump << tabs << "\t" << "pNext: " << value.pNext << std::endl;
			dump << tabs << "\t" << "dstSet: " << value.dstSet << std::endl;
			dump << tabs << "\t" << "dstBinding: " << value.dstBinding << std::endl;
			dump << tabs << "\t" << "dstArrayElement: " << value.dstArrayElement << std::endl;
			dump << tabs << "\t" << "descriptorCount: " << value.descriptorCount << std::endl;
			dump << tabs << "\t" << "descriptorType: " << value.descriptorType << std::endl;
			dump << tabs << "\t" << "pImageInfo: " << std::endl << subDump( value.pImageInfo, tabs + "\t" ) << std::endl;
			dump << tabs << "\t" << "pBufferInfo: " << std::endl << subDump( value.pBufferInfo, tabs + "\t" ) << std::endl;
			dump << tabs << "\t" << "pTexelBufferView: " << std::endl << subDump( value.pTexelBufferView, tabs + "\t" ) << std::endl;
			dump << tabs << "}" << std::endl;
			return dump.str();
		}

		static inline std::string subDump( VkDescriptorImageInfo const & value, std::string const & tabs )
		{
			std::stringstream dump;
			dump << tabs << "{" << std::endl;
			dump << tabs << "\t" << "sampler: " << value.sampler << std::endl;
			dump << tabs << "\t" << "imageView: " << value.imageView << std::endl;
			dump << tabs << "\t" << "imageLayout: " << value.imageLayout << std::endl;
			dump << tabs << "}" << std::endl;
			return dump.str();
		}

		static inline std::string subDump( VkDescriptorBufferInfo const & value, std::string const & tabs )
		{
			std::stringstream dump;
			dump << tabs << "{" << std::endl;
			dump << tabs << "\t" << "buffer: " << value.buffer << std::endl;
			dump << tabs << "\t" << "offset: " << value.offset << std::endl;
			dump << tabs << "\t" << "range: " << value.range << std::endl;
			dump << tabs << "}" << std::endl;
			return dump.str();
		}

		static inline std::string subDump( VkDescriptorSetAllocateInfo const & value, std::string const & tabs )
		{
			std::stringstream dump;
			dump << tabs << "{" << std::endl;
			dump << tabs << "\t" << "sType: " << value.sType << std::endl;
			dump << tabs << "\t" << "pNext: " << value.pNext << std::endl;
			dump << tabs << "\t" << "descriptorPool: " << value.descriptorPool << std::endl;
			dump << tabs << "\t" << "descriptorSetCount: " << value.descriptorSetCount << std::endl;
			dump << tabs << "\t" << "pSetLayouts: " << std::endl << subDump( value.pSetLayouts, value.descriptorSetCount, tabs + "\t" ) << std::endl;
			dump << tabs << "}" << std::endl;
			return dump.str();
		}

		static inline std::string subDump( VkDescriptorPoolSize const & value, std::string const & tabs )
		{
			std::stringstream dump;
			dump << tabs << "{" << std::endl;
			dump << tabs << "\t" << "type: " << value.type << std::endl;
			dump << tabs << "\t" << "descriptorCount: " << value.descriptorCount << std::endl;
			dump << tabs << "}" << std::endl;
			return dump.str();
		}

		static inline std::string subDump( VkDescriptorSetLayoutCreateInfo const & value, std::string const & tabs )
		{
			std::stringstream dump;
			dump << tabs << "{" << std::endl;
			dump << tabs << "\t" << "sType: " << value.sType << std::endl;
			dump << tabs << "\t" << "pNext: " << value.pNext << std::endl;
			dump << tabs << "\t" << "flags: " << value.flags << std::endl;
			dump << tabs << "\t" << "bindingCount: " << value.bindingCount << std::endl;
			dump << tabs << "\t" << "pBindings: " << std::endl << subDump( value.pBindings, value.bindingCount, tabs + "\t" ) << std::endl;
			dump << tabs << "}" << std::endl;
			return dump.str();
		}

		static inline std::string subDump( VkDescriptorSetLayoutBinding const & value, std::string const & tabs )
		{
			std::stringstream dump;
			dump << tabs << "{" << std::endl;
			dump << tabs << "\t" << "binding: " << value.binding << std::endl;
			dump << tabs << "\t" << "descriptorType: " << value.descriptorType << std::endl;
			dump << tabs << "\t" << "descriptorCount: " << value.descriptorCount << std::endl;
			dump << tabs << "\t" << "stageFlags: " << value.stageFlags << std::endl;
			dump << tabs << "\t" << "pImmutableSamplers: " << value.pImmutableSamplers << std::endl;
			dump << tabs << "}" << std::endl;
			return dump.str();
		}

		static inline std::string subDump( VkImageCreateInfo const & value, std::string const & tabs )
		{
			std::stringstream dump;
			dump << tabs << "{" << std::endl;
			dump << tabs << "\t" << "sType: " << value.sType << std::endl;
			dump << tabs << "\t" << "pNext: " << value.pNext << std::endl;
			dump << tabs << "\t" << "flags: " << value.flags << std::endl;
			dump << tabs << "\t" << "imageType: " << value.imageType << std::endl;
			dump << tabs << "\t" << "format: " << value.format << std::endl;
			dump << tabs << "\t" << "extent: " << std::endl << subDump( value.extent, tabs + "\t" ) << std::endl;
			dump << tabs << "\t" << "mipLevels: " << value.mipLevels << std::endl;
			dump << tabs << "\t" << "arrayLayers: " << value.arrayLayers << std::endl;
			dump << tabs << "\t" << "samples: " << value.samples << std::endl;
			dump << tabs << "\t" << "tiling: " << value.tiling << std::endl;
			dump << tabs << "\t" << "usage: " << value.usage << std::endl;
			dump << tabs << "\t" << "sharingMode: " << value.sharingMode << std::endl;
			dump << tabs << "\t" << "queueFamilyIndexCount: " << value.queueFamilyIndexCount << std::endl;
			dump << tabs << "\t" << "pQueueFamilyIndices: " << std::endl << subDump( value.pQueueFamilyIndices, value.queueFamilyIndexCount, tabs + "\t" ) << std::endl;
			dump << tabs << "\t" << "initialLayout: " << value.initialLayout << std::endl;
			dump << tabs << "}" << std::endl;
			return dump.str();
		}

		static inline std::string subDump( VkSamplerCreateInfo const & value, std::string const & tabs )
		{
			std::stringstream dump;
			dump << tabs << "{" << std::endl;
			dump << tabs << "\t" << "sType: " << value.sType << std::endl;
			dump << tabs << "\t" << "pNext: " << value.pNext << std::endl;
			dump << tabs << "\t" << "flags: " << value.flags << std::endl;
			dump << tabs << "\t" << "magFilter: " << value.magFilter << std::endl;
			dump << tabs << "\t" << "minFilter: " << value.minFilter << std::endl;
			dump << tabs << "\t" << "extent: " << value.mipmapMode << std::endl;
			dump << tabs << "\t" << "addressModeU: " << value.addressModeU << std::endl;
			dump << tabs << "\t" << "addressModeV: " << value.addressModeV << std::endl;
			dump << tabs << "\t" << "addressModeW: " << value.addressModeW << std::endl;
			dump << tabs << "\t" << "mipLodBias: " << value.mipLodBias << std::endl;
			dump << tabs << "\t" << "anisotropyEnable: " << value.anisotropyEnable << std::endl;
			dump << tabs << "\t" << "maxAnisotropy: " << value.maxAnisotropy << std::endl;
			dump << tabs << "\t" << "compareEnable: " << value.compareEnable << std::endl;
			dump << tabs << "\t" << "pQueueFamilyIndices: " << value.compareOp << std::endl;
			dump << tabs << "\t" << "minLod: " << value.minLod << std::endl;
			dump << tabs << "\t" << "maxLod: " << value.maxLod << std::endl;
			dump << tabs << "\t" << "borderColor: " << value.borderColor << std::endl;
			dump << tabs << "\t" << "unnormalizedCoordinates: " << value.unnormalizedCoordinates << std::endl;
			dump << tabs << "}" << std::endl;
			return dump.str();
		}

		static inline std::string subDump( VkBufferImageCopy const & value, std::string const & tabs )
		{
			std::stringstream dump;
			dump << tabs << "{" << std::endl;
			dump << tabs << "\t" << "bufferOffset: " << value.bufferOffset << std::endl;
			dump << tabs << "\t" << "bufferRowLength: " << value.bufferRowLength << std::endl;
			dump << tabs << "\t" << "bufferImageHeight: " << value.bufferImageHeight << std::endl;
			dump << tabs << "\t" << "imageSubresource: " << std::endl << subDump( value.imageSubresource, tabs + "\t" ) << std::endl;
			dump << tabs << "\t" << "imageOffset: " << std::endl << subDump( value.imageOffset, tabs + "\t" ) << std::endl;
			dump << tabs << "\t" << "imageExtent: " << std::endl << subDump( value.imageExtent, tabs + "\t" ) << std::endl;
			dump << tabs << "}" << std::endl;
			return dump.str();
		}

		static inline std::string subDump( VkImageCopy const & value, std::string const & tabs )
		{
			std::stringstream dump;
			dump << tabs << "{" << std::endl;
			dump << tabs << "\t" << "dstOffset: " << std::endl << subDump( value.dstOffset, tabs + "\t" ) << std::endl;
			dump << tabs << "\t" << "dstSubresource: " << std::endl << subDump( value.dstSubresource, tabs + "\t" ) << std::endl;
			dump << tabs << "\t" << "srcOffset: " << std::endl << subDump( value.srcOffset, tabs + "\t" ) << std::endl;
			dump << tabs << "\t" << "srcSubresource: " << std::endl << subDump( value.srcSubresource, tabs + "\t" ) << std::endl;
			dump << tabs << "\t" << "extent: " << std::endl << subDump( value.extent, tabs + "\t" ) << std::endl;
			dump << tabs << "}" << std::endl;
			return dump.str();
		}

		static inline std::string subDump( VkImageBlit const & value, std::string const & tabs )
		{
			std::stringstream dump;
			dump << tabs << "{" << std::endl;
			dump << tabs << "\t" << "dstOffset: " << std::endl << subDump( value.dstOffsets, tabs + "\t" ) << std::endl;
			dump << tabs << "\t" << "dstSubresource: " << std::endl << subDump( value.dstSubresource, tabs + "\t" ) << std::endl;
			dump << tabs << "\t" << "srcOffset: " << std::endl << subDump( value.srcOffsets, tabs + "\t" ) << std::endl;
			dump << tabs << "\t" << "srcSubresource: " << std::endl << subDump( value.srcSubresource, tabs + "\t" ) << std::endl;
			dump << tabs << "}" << std::endl;
			return dump.str();
		}

		static inline std::string subDump( VkImageSubresourceLayers const & value, std::string const & tabs )
		{
			std::stringstream dump;
			dump << tabs << "{" << std::endl;
			dump << tabs << "\t" << "aspectMask: " << value.aspectMask << std::endl;
			dump << tabs << "\t" << "mipLevel: " << value.mipLevel << std::endl;
			dump << tabs << "\t" << "baseArrayLayer: " << value.baseArrayLayer << std::endl;
			dump << tabs << "\t" << "layerCount: " << value.layerCount << std::endl;
			dump << tabs << "}" << std::endl;
			return dump.str();
		}

		static inline std::string subDump( VkBufferCreateInfo const & value, std::string const & tabs )
		{
			std::stringstream dump;
			dump << tabs << "{" << std::endl;
			dump << tabs << "\t" << "sType: " << value.sType << std::endl;
			dump << tabs << "\t" << "pNext: " << value.pNext << std::endl;
			dump << tabs << "\t" << "flags: " << value.flags << std::endl;
			dump << tabs << "\t" << "size: " << value.size << std::endl;
			dump << tabs << "\t" << "usage: " << value.usage << std::endl;
			dump << tabs << "\t" << "sharingMode: " << value.sharingMode << std::endl;
			dump << tabs << "\t" << "queueFamilyIndexCount: " << value.queueFamilyIndexCount << std::endl;
			dump << tabs << "\t" << "pQueueFamilyIndices: " << std::endl << subDump( value.pQueueFamilyIndices, value.queueFamilyIndexCount, tabs + "\t" ) << std::endl;
			dump << tabs << "}" << std::endl;
			return dump.str();
		}

		static inline std::string subDump( VkBufferCopy const & value, std::string const & tabs )
		{
			std::stringstream dump;
			dump << tabs << "{" << std::endl;
			dump << tabs << "\t" << "srcOffset: " << value.srcOffset << std::endl;
			dump << tabs << "\t" << "dstOffset: " << value.dstOffset << std::endl;
			dump << tabs << "\t" << "size: " << value.size << std::endl;
			dump << tabs << "}" << std::endl;
			return dump.str();
		}

		static inline std::string subDump( VkBufferMemoryBarrier const & value, std::string const & tabs )
		{
			std::stringstream dump;
			dump << tabs << "{" << std::endl;
			dump << tabs << "\t" << "sType: " << value.sType << std::endl;
			dump << tabs << "\t" << "pNext: " << value.pNext << std::endl;
			dump << tabs << "\t" << "srcAccessMask: " << value.srcAccessMask << std::endl;
			dump << tabs << "\t" << "dstAccessMask: " << value.dstAccessMask << std::endl;
			dump << tabs << "\t" << "srcQueueFamilyIndex: " << value.srcQueueFamilyIndex << std::endl;
			dump << tabs << "\t" << "dstQueueFamilyIndex: " << value.dstQueueFamilyIndex << std::endl;
			dump << tabs << "\t" << "buffer: " << value.buffer << std::endl;
			dump << tabs << "\t" << "offset: " << value.offset << std::endl;
			dump << tabs << "\t" << "size: " << value.size << std::endl;
			dump << tabs << "}" << std::endl;
			return dump.str();
		}

		static inline std::string subDump( VkMemoryAllocateInfo const & value, std::string const & tabs )
		{
			std::stringstream dump;
			dump << tabs << "{" << std::endl;
			dump << tabs << "\t" << "sType: " << value.sType << std::endl;
			dump << tabs << "\t" << "pNext: " << value.pNext << std::endl;
			dump << tabs << "\t" << "allocationSize: " << value.allocationSize << std::endl;
			dump << tabs << "\t" << "memoryTypeIndex: " << value.memoryTypeIndex << std::endl;
			dump << tabs << "}" << std::endl;
			return dump.str();
		}

		static inline std::string subDump( VkMappedMemoryRange const & value, std::string const & tabs )
		{
			std::stringstream dump;
			dump << tabs << "{" << std::endl;
			dump << tabs << "\t" << "sType: " << value.sType << std::endl;
			dump << tabs << "\t" << "pNext: " << value.pNext << std::endl;
			dump << tabs << "\t" << "memory: " << value.memory << std::endl;
			dump << tabs << "\t" << "offset: " << value.offset << std::endl;
			dump << tabs << "\t" << "size: " << value.size << std::endl;
			dump << tabs << "}" << std::endl;
			return dump.str();
		}

		static inline std::string subDump( VkLayerProperties const & value, std::string const & tabs )
		{
			std::stringstream dump;
			dump << tabs << "{" << std::endl;
			dump << tabs << "\t" << "layerName: " << value.layerName << std::endl;
			dump << tabs << "\t" << "specVersion: " << value.specVersion << std::endl;
			dump << tabs << "\t" << "implementationVersion: " << value.implementationVersion << std::endl;
			dump << tabs << "\t" << "description: " << value.description << std::endl;
			dump << tabs << "}" << std::endl;
			return dump.str();
		}

		static inline std::string subDump( VkExtensionProperties const & value, std::string const & tabs )
		{
			std::stringstream dump;
			dump << tabs << "{" << std::endl;
			dump << tabs << "\t" << "extensionName: " << value.extensionName << std::endl;
			dump << tabs << "\t" << "specVersion: " << value.specVersion << std::endl;
			dump << tabs << "}" << std::endl;
			return dump.str();
		}

		static inline std::string subDump( VkApplicationInfo const & value, std::string const & tabs )
		{
			std::stringstream dump;
			dump << tabs << "{" << std::endl;
			dump << tabs << "\t" << "sType: " << value.sType << std::endl;
			dump << tabs << "\t" << "pNext: " << value.pNext << std::endl;
			dump << tabs << "\t" << "pApplicationName: " << value.pApplicationName << std::endl;
			dump << tabs << "\t" << "applicationVersion:" << value.applicationVersion << std::endl;
			dump << tabs << "\t" << "pEngineName: " << value.pEngineName << std::endl;
			dump << tabs << "\t" << "engineVersion: " << value.engineVersion << std::endl;
			dump << tabs << "\t" << "apiVersion: " << value.apiVersion << std::endl;
			dump << tabs << "}" << std::endl;
			return dump.str();
		}

		static inline std::string subDump( VkInstanceCreateInfo const & value, std::string const & tabs )
		{
			std::stringstream dump;
			dump << tabs << "{" << std::endl;
			dump << tabs << "\t" << "sType: " << value.sType << std::endl;
			dump << tabs << "\t" << "pNext:" << value.pNext << std::endl;
			dump << tabs << "\t" << "flags: " << value.flags << std::endl;
			dump << tabs << "\t" << "pApplicationInfo: " << std::endl << subDump( value.pApplicationInfo, tabs + "\t" ) << std::endl;
			dump << tabs << "\t" << "enabledLayerCount: " << value.enabledLayerCount << std::endl;
			dump << tabs << "\t" << "ppEnabledLayerNames: " << std::endl << subDump( value.ppEnabledLayerNames, value.enabledLayerCount, tabs + "\t" ) << std::endl;
			dump << tabs << "\t" << "enabledExtensionCount: " << value.enabledExtensionCount << std::endl;
			dump << tabs << "\t" << "ppEnabledExtensionNames: " << std::endl << subDump( value.ppEnabledExtensionNames, value.enabledExtensionCount, tabs + "\t" ) << std::endl;
			dump << tabs << "}" << std::endl;
			return dump.str();
		}

		static inline std::string subDump( VkDeviceQueueCreateInfo const & value, std::string const & tabs )
		{
			std::stringstream dump;
			dump << tabs << "{" << std::endl;
			dump << tabs << "\t" << "sType: " << value.sType << std::endl;
			dump << tabs << "\t" << "pNext:" << value.pNext << std::endl;
			dump << tabs << "\t" << "flags: " << value.flags << std::endl;
			dump << tabs << "\t" << "queueFamilyIndex: " << value.queueFamilyIndex << std::endl;
			dump << tabs << "\t" << "queueCount: " << value.queueCount << std::endl;
			dump << tabs << "\t" << "pQueuePriorities: " << std::endl << subDump( value.pQueuePriorities, value.queueCount, tabs + "\t" ) << std::endl;
			dump << tabs << "}" << std::endl;
			return dump.str();
		}

		static inline std::string subDump( VkPhysicalDeviceFeatures const & value, std::string const & tabs )
		{
			std::stringstream dump;
			dump << tabs << "{" << std::endl;
			dump << tabs << "\t" << "robustBufferAccess: " << value.robustBufferAccess << std::endl;
			dump << tabs << "\t" << "fullDrawIndexUint32: " << value.fullDrawIndexUint32 << std::endl;
			dump << tabs << "\t" << "imageCubeArray: " << value.imageCubeArray << std::endl;
			dump << tabs << "\t" << "independentBlend: " << value.independentBlend << std::endl;
			dump << tabs << "\t" << "geometryShader: " << value.geometryShader << std::endl;
			dump << tabs << "\t" << "tessellationShader: " << value.tessellationShader << std::endl;
			dump << tabs << "\t" << "sampleRateShading: " << value.sampleRateShading << std::endl;
			dump << tabs << "\t" << "dualSrcBlend: " << value.dualSrcBlend << std::endl;
			dump << tabs << "\t" << "logicOp: " << value.logicOp << std::endl;
			dump << tabs << "\t" << "multiDrawIndirect: " << value.multiDrawIndirect << std::endl;
			dump << tabs << "\t" << "drawIndirectFirstInstance: " << value.drawIndirectFirstInstance << std::endl;
			dump << tabs << "\t" << "depthClamp: " << value.depthClamp << std::endl;
			dump << tabs << "\t" << "depthBiasClamp: " << value.depthBiasClamp << std::endl;
			dump << tabs << "\t" << "fillModeNonSolid: " << value.fillModeNonSolid << std::endl;
			dump << tabs << "\t" << "depthBounds: " << value.depthBounds << std::endl;
			dump << tabs << "\t" << "wideLines: " << value.wideLines << std::endl;
			dump << tabs << "\t" << "largePoints: " << value.largePoints << std::endl;
			dump << tabs << "\t" << "alphaToOne: " << value.alphaToOne << std::endl;
			dump << tabs << "\t" << "multiViewport: " << value.multiViewport << std::endl;
			dump << tabs << "\t" << "samplerAnisotropy: " << value.samplerAnisotropy << std::endl;
			dump << tabs << "\t" << "textureCompressionETC2: " << value.textureCompressionETC2 << std::endl;
			dump << tabs << "\t" << "textureCompressionASTC_LDR: " << value.textureCompressionASTC_LDR << std::endl;
			dump << tabs << "\t" << "textureCompressionBC: " << value.textureCompressionBC << std::endl;
			dump << tabs << "\t" << "occlusionQueryPrecise: " << value.occlusionQueryPrecise << std::endl;
			dump << tabs << "\t" << "pipelineStatisticsQuery: " << value.pipelineStatisticsQuery << std::endl;
			dump << tabs << "\t" << "vertexPipelineStoresAndAtomics: " << value.vertexPipelineStoresAndAtomics << std::endl;
			dump << tabs << "\t" << "fragmentStoresAndAtomics: " << value.fragmentStoresAndAtomics << std::endl;
			dump << tabs << "\t" << "shaderTessellationAndGeometryPointSize: " << value.shaderTessellationAndGeometryPointSize << std::endl;
			dump << tabs << "\t" << "shaderImageGatherExtended: " << value.shaderImageGatherExtended << std::endl;
			dump << tabs << "\t" << "shaderStorageImageExtendedFormats: " << value.shaderStorageImageExtendedFormats << std::endl;
			dump << tabs << "\t" << "shaderStorageImageMultisample: " << value.shaderStorageImageMultisample << std::endl;
			dump << tabs << "\t" << "shaderStorageImageReadWithoutFormat: " << value.shaderStorageImageReadWithoutFormat << std::endl;
			dump << tabs << "\t" << "shaderStorageImageWriteWithoutFormat: " << value.shaderStorageImageWriteWithoutFormat << std::endl;
			dump << tabs << "\t" << "shaderUniformBufferArrayDynamicIndexing: " << value.shaderUniformBufferArrayDynamicIndexing << std::endl;
			dump << tabs << "\t" << "shaderSampledImageArrayDynamicIndexing: " << value.shaderSampledImageArrayDynamicIndexing << std::endl;
			dump << tabs << "\t" << "shaderStorageBufferArrayDynamicIndexing: " << value.shaderStorageBufferArrayDynamicIndexing << std::endl;
			dump << tabs << "\t" << "shaderStorageImageArrayDynamicIndexing: " << value.shaderStorageImageArrayDynamicIndexing << std::endl;
			dump << tabs << "\t" << "shaderClipDistance: " << value.shaderClipDistance << std::endl;
			dump << tabs << "\t" << "shaderCullDistance: " << value.shaderCullDistance << std::endl;
			dump << tabs << "\t" << "shaderFloat64: " << value.shaderFloat64 << std::endl;
			dump << tabs << "\t" << "shaderInt64: " << value.shaderInt64 << std::endl;
			dump << tabs << "\t" << "shaderInt16: " << value.shaderInt16 << std::endl;
			dump << tabs << "\t" << "shaderResourceResidency: " << value.shaderResourceResidency << std::endl;
			dump << tabs << "\t" << "shaderResourceMinLod: " << value.shaderResourceMinLod << std::endl;
			dump << tabs << "\t" << "sparseBinding: " << value.sparseBinding << std::endl;
			dump << tabs << "\t" << "sparseResidencyBuffer: " << value.sparseResidencyBuffer << std::endl;
			dump << tabs << "\t" << "sparseResidencyImage2D: " << value.sparseResidencyImage2D << std::endl;
			dump << tabs << "\t" << "sparseResidencyImage3D: " << value.sparseResidencyImage3D << std::endl;
			dump << tabs << "\t" << "sparseResidency2Samples: " << value.sparseResidency2Samples << std::endl;
			dump << tabs << "\t" << "sparseResidency4Samples: " << value.sparseResidency4Samples << std::endl;
			dump << tabs << "\t" << "sparseResidency8Samples: " << value.sparseResidency8Samples << std::endl;
			dump << tabs << "\t" << "sparseResidency16Samples: " << value.sparseResidency16Samples << std::endl;
			dump << tabs << "\t" << "sparseResidencyAliased: " << value.sparseResidencyAliased << std::endl;
			dump << tabs << "\t" << "variableMultisampleRate: " << value.variableMultisampleRate << std::endl;
			dump << tabs << "\t" << "inheritedQueries: " << value.inheritedQueries << std::endl;
			dump << tabs << "}" << std::endl;
			return dump.str();
		}

		static inline std::string subDump( VkDeviceCreateInfo const & value, std::string const & tabs )
		{
			std::stringstream dump;
			dump << tabs << "{" << std::endl;
			dump << tabs << "\t" << "sType: " << value.sType << std::endl;
			dump << tabs << "\t" << "pNext:" << value.pNext << std::endl;
			dump << tabs << "\t" << "flags: " << value.flags << std::endl;
			dump << tabs << "\t" << "queueCreateInfoCount: " << value.queueCreateInfoCount;
			dump << tabs << "\t" << "ppEnabledLayerNames: " << std::endl << subDump( value.pQueueCreateInfos, value.queueCreateInfoCount, tabs + "\t" ) << std::endl;
			dump << tabs << "\t" << "enabledLayerCount: " << value.enabledLayerCount << std::endl;
			dump << tabs << "\t" << "ppEnabledLayerNames: " << std::endl << subDump( value.ppEnabledLayerNames, value.enabledLayerCount, tabs + "\t" ) << std::endl;
			dump << tabs << "\t" << "enabledExtensionCount: " << value.enabledExtensionCount << std::endl;
			dump << tabs << "\t" << "ppEnabledExtensionNames: " << std::endl << subDump( value.ppEnabledExtensionNames, value.enabledExtensionCount, tabs + "\t" ) << std::endl;
			dump << tabs << "\t" << "pEnabledFeatures: " << std::endl << subDump( value.pEnabledFeatures, tabs + "\t" ) << std::endl;
			dump << tabs << "}" << std::endl;
			return dump.str();
		}

		static inline std::string subDump( VkPushConstantRange const & value, std::string const & tabs )
		{
			std::stringstream dump;
			dump << tabs << "{" << std::endl;
			dump << tabs << "\t" << "stageFlags: " << value.stageFlags << std::endl;
			dump << tabs << "\t" << "offset: " << value.offset << std::endl;
			dump << tabs << "\t" << "size: " << value.size << std::endl;
			dump << tabs << "}" << std::endl;
			return dump.str();
		}

		static inline std::string subDump( VkPipelineLayoutCreateInfo const & value, std::string const & tabs )
		{
			std::stringstream dump;
			dump << tabs << "{" << std::endl;
			dump << tabs << "\t" << "sType: " << value.sType << std::endl;
			dump << tabs << "\t" << "pNext:" << value.pNext << std::endl;
			dump << tabs << "\t" << "flags: " << value.flags << std::endl;
			dump << tabs << "\t" << "setLayoutCount: " << value.setLayoutCount << std::endl;
			dump << tabs << "\t" << "pSetLayouts: " << std::endl << subDump( value.pSetLayouts, value.setLayoutCount, tabs + "\t" ) << std::endl;
			dump << tabs << "\t" << "pushConstantRangeCount: " << value.pushConstantRangeCount << std::endl;
			dump << tabs << "\t" << "pPushConstantRanges: " << std::endl << subDump( value.pPushConstantRanges, value.pushConstantRangeCount, tabs + "\t" ) << std::endl;
			dump << tabs << "}" << std::endl;
			return dump.str();
		}

		static inline std::string subDump( VkSpecializationMapEntry const & value, std::string const & tabs )
		{
			std::stringstream dump;
			dump << tabs << "{" << std::endl;
			dump << tabs << "\t" << "constantID: " << value.constantID << std::endl;
			dump << tabs << "\t" << "offset: " << value.offset << std::endl;
			dump << tabs << "\t" << "size: " << value.size << std::endl;
			dump << tabs << "}" << std::endl;
			return dump.str();
		}

		static inline std::string subDump( VkSpecializationInfo const & value, std::string const & tabs )
		{
			std::stringstream dump;
			dump << tabs << "{" << std::endl;
			dump << tabs << "\t" << "mapEntryCount: " << value.mapEntryCount << std::endl;
			dump << tabs << "\t" << "pMapEntries: " << std::endl << subDump( value.pMapEntries, value.mapEntryCount, tabs + "\t" ) << std::endl;
			dump << tabs << "\t" << "dataSize: " << value.dataSize << std::endl;
			dump << tabs << "\t" << "pData: " << value.pData << std::endl;
			dump << tabs << "}" << std::endl;
			return dump.str();
		}

		static inline std::string subDump( VkPipelineShaderStageCreateInfo const & value, std::string const & tabs )
		{
			std::stringstream dump;
			dump << tabs << "{" << std::endl;
			dump << tabs << "\t" << "sType: " << value.sType << std::endl;
			dump << tabs << "\t" << "pNext: " << value.pNext << std::endl;
			dump << tabs << "\t" << "flags: " << value.flags << std::endl;
			dump << tabs << "\t" << "stage: " << value.stage << std::endl;
			dump << tabs << "\t" << "module: " << value.module << std::endl;
			dump << tabs << "\t" << "pName: " << value.pName << std::endl;
			dump << tabs << "\t" << "pSpecializationInfo: " << std::endl << subDump( value.pSpecializationInfo, tabs + "\t" ) << std::endl;
			dump << tabs << "}" << std::endl;
			return dump.str();
		}

		static inline std::string subDump( VkVertexInputBindingDescription const & value, std::string const & tabs )
		{
			std::stringstream dump;
			dump << tabs << "{" << std::endl;
			dump << tabs << "\t" << "binding: " << value.binding << std::endl;
			dump << tabs << "\t" << "stride: " << value.stride << std::endl;
			dump << tabs << "\t" << "inputRate: " << value.inputRate << std::endl;
			dump << tabs << "}" << std::endl;
			return dump.str();
		}

		static inline std::string subDump( VkVertexInputAttributeDescription const & value, std::string const & tabs )
		{
			std::stringstream dump;
			dump << tabs << "{" << std::endl;
			dump << tabs << "\t" << "location: " << value.location << std::endl;
			dump << tabs << "\t" << "binding: " << value.binding << std::endl;
			dump << tabs << "\t" << "format: " << value.format << std::endl;
			dump << tabs << "\t" << "offset: " << value.offset << std::endl;
			dump << tabs << "}" << std::endl;
			return dump.str();
		}

		static inline std::string subDump( VkPipelineVertexInputStateCreateInfo const & value, std::string const & tabs )
		{
			std::stringstream dump;
			dump << tabs << "{" << std::endl;
			dump << tabs << "\t" << "sType: " << value.sType << std::endl;
			dump << tabs << "\t" << "pNext: " << value.pNext << std::endl;
			dump << tabs << "\t" << "flags: " << value.flags << std::endl;
			dump << tabs << "\t" << "vertexBindingDescriptionCount: " << value.vertexBindingDescriptionCount << std::endl;
			dump << tabs << "\t" << "pVertexBindingDescriptions: " << std::endl << subDump( value.pVertexBindingDescriptions, value.vertexBindingDescriptionCount, tabs + "\t" ) << std::endl;
			dump << tabs << "\t" << "vertexAttributeDescriptionCount: " << value.vertexAttributeDescriptionCount << std::endl;
			dump << tabs << "\t" << "pVertexAttributeDescriptions: " << std::endl << subDump( value.pVertexAttributeDescriptions, value.vertexAttributeDescriptionCount, tabs + "\t" ) << std::endl;
			dump << tabs << "}" << std::endl;
			return dump.str();
		}

		static inline std::string subDump( VkPipelineInputAssemblyStateCreateInfo const & value, std::string const & tabs )
		{
			std::stringstream dump;
			dump << tabs << "{" << std::endl;
			dump << tabs << "\t" << "sType: " << value.sType << std::endl;
			dump << tabs << "\t" << "pNext: " << value.pNext << std::endl;
			dump << tabs << "\t" << "flags: " << value.flags << std::endl;
			dump << tabs << "\t" << "topology: " << value.topology << std::endl;
			dump << tabs << "\t" << "primitiveRestartEnable: " << value.primitiveRestartEnable << std::endl;
			dump << tabs << "}" << std::endl;
			return dump.str();
		}

		static inline std::string subDump( VkPipelineTessellationStateCreateInfo const & value, std::string const & tabs )
		{
			std::stringstream dump;
			dump << tabs << "{" << std::endl;
			dump << tabs << "\t" << "sType: " << value.sType << std::endl;
			dump << tabs << "\t" << "pNext: " << value.pNext << std::endl;
			dump << tabs << "\t" << "flags: " << value.flags << std::endl;
			dump << tabs << "\t" << "patchControlPoints: " << value.patchControlPoints << std::endl;
			dump << tabs << "}" << std::endl;
			return dump.str();
		}

		static inline std::string subDump( VkPipelineViewportStateCreateInfo const & value, std::string const & tabs )
		{
			std::stringstream dump;
			dump << tabs << "{" << std::endl;
			dump << tabs << "\t" << "sType: " << value.sType << std::endl;
			dump << tabs << "\t" << "pNext: " << value.pNext << std::endl;
			dump << tabs << "\t" << "flags: " << value.flags << std::endl;
			dump << tabs << "\t" << "viewportCount: " << value.viewportCount << std::endl;
			dump << tabs << "\t" << "pViewports: " << std::endl << subDump( value.pViewports, value.viewportCount, tabs + "\t" ) << std::endl;
			dump << tabs << "\t" << "scissorCount: " << value.scissorCount << std::endl;
			dump << tabs << "\t" << "pScissors: " << std::endl << subDump( value.pScissors, value.scissorCount, tabs + "\t" ) << std::endl;
			dump << tabs << "}" << std::endl;
			return dump.str();
		}

		static inline std::string subDump( VkPipelineRasterizationStateCreateInfo const & value, std::string const & tabs )
		{
			std::stringstream dump;
			dump << tabs << "{" << std::endl;
			dump << tabs << "\t" << "sType: " << value.sType << std::endl;
			dump << tabs << "\t" << "pNext: " << value.pNext << std::endl;
			dump << tabs << "\t" << "flags: " << value.flags << std::endl;
			dump << tabs << "\t" << "depthClampEnable: " << value.depthClampEnable << std::endl;
			dump << tabs << "\t" << "rasterizerDiscardEnable: " << value.rasterizerDiscardEnable << std::endl;
			dump << tabs << "\t" << "polygonMode: " << value.polygonMode << std::endl;
			dump << tabs << "\t" << "cullMode: " << value.cullMode << std::endl;
			dump << tabs << "\t" << "frontFace: " << value.frontFace << std::endl;
			dump << tabs << "\t" << "depthBiasEnable: " << value.depthBiasEnable << std::endl;
			dump << tabs << "\t" << "depthBiasConstantFactor: " << value.depthBiasConstantFactor << std::endl;
			dump << tabs << "\t" << "depthBiasClamp: " << value.depthBiasClamp << std::endl;
			dump << tabs << "\t" << "depthBiasSlopeFactor: " << value.depthBiasSlopeFactor << std::endl;
			dump << tabs << "\t" << "lineWidth: " << value.lineWidth << std::endl;
			dump << tabs << "}" << std::endl;
			return dump.str();
		}

		static inline std::string subDump( VkPipelineMultisampleStateCreateInfo const & value, std::string const & tabs )
		{
			std::stringstream dump;
			dump << tabs << "{" << std::endl;
			dump << tabs << "\t" << "sType: " << value.sType << std::endl;
			dump << tabs << "\t" << "pNext: " << value.pNext << std::endl;
			dump << tabs << "\t" << "flags: " << value.flags << std::endl;
			dump << tabs << "\t" << "rasterizationSamples: " << value.rasterizationSamples << std::endl;
			dump << tabs << "\t" << "sampleShadingEnable: " << value.sampleShadingEnable << std::endl;
			dump << tabs << "\t" << "minSampleShading: " << value.minSampleShading << std::endl;
			dump << tabs << "\t" << "pSampleMask: " << std::endl <<  subDump( value.pSampleMask, tabs + "\t" ) << std::endl;
			dump << tabs << "\t" << "alphaToCoverageEnable: " << value.alphaToCoverageEnable << std::endl;
			dump << tabs << "\t" << "alphaToOneEnable: " << value.alphaToOneEnable << std::endl;
			dump << tabs << "}" << std::endl;
			return dump.str();
		}

		static inline std::string subDump( VkStencilOpState const & value, std::string const & tabs )
		{
			std::stringstream dump;
			dump << tabs << "{" << std::endl;
			dump << tabs << "\t" << "failOp: " << value.failOp << std::endl;
			dump << tabs << "\t" << "passOp: " << value.passOp << std::endl;
			dump << tabs << "\t" << "depthFailOp: " << value.depthFailOp << std::endl;
			dump << tabs << "\t" << "compareOp: " << value.compareOp << std::endl;
			dump << tabs << "\t" << "compareMask: " << value.compareMask << std::endl;
			dump << tabs << "\t" << "writeMask: " << value.writeMask << std::endl;
			dump << tabs << "\t" << "reference: " << value.reference << std::endl;
			dump << tabs << "}" << std::endl;
			return dump.str();
		}

		static inline std::string subDump( VkPipelineDepthStencilStateCreateInfo const & value, std::string const & tabs )
		{
			std::stringstream dump;
			dump << tabs << "{" << std::endl;
			dump << tabs << "\t" << "sType: " << value.sType << std::endl;
			dump << tabs << "\t" << "pNext: " << value.pNext << std::endl;
			dump << tabs << "\t" << "flags: " << value.flags << std::endl;
			dump << tabs << "\t" << "depthTestEnable: " << value.depthTestEnable << std::endl;
			dump << tabs << "\t" << "depthWriteEnable: " << value.depthWriteEnable << std::endl;
			dump << tabs << "\t" << "depthCompareOp: " << value.depthCompareOp << std::endl;
			dump << tabs << "\t" << "depthBoundsTestEnable: " << value.depthBoundsTestEnable << std::endl;
			dump << tabs << "\t" << "stencilTestEnable: " << value.stencilTestEnable << std::endl;
			dump << tabs << "\t" << "front: " << std::endl << subDump( value.front, tabs + "\t" ) << std::endl;
			dump << tabs << "\t" << "back: " << std::endl << subDump( value.back, tabs + "\t" ) << std::endl;
			dump << tabs << "\t" << "minDepthBounds: " << value.minDepthBounds << std::endl;
			dump << tabs << "\t" << "maxDepthBounds: " << value.maxDepthBounds << std::endl;
			dump << tabs << "}" << std::endl;
			return dump.str();
		}

		static inline std::string subDump( VkPipelineColorBlendAttachmentState const & value, std::string const & tabs )
		{
			std::stringstream dump;
			dump << tabs << "{" << std::endl;
			dump << tabs << "\t" << "blendEnable: " << value.blendEnable << std::endl;
			dump << tabs << "\t" << "srcColorBlendFactor: " << value.srcColorBlendFactor << std::endl;
			dump << tabs << "\t" << "dstColorBlendFactor: " << value.dstColorBlendFactor << std::endl;
			dump << tabs << "\t" << "colorBlendOp: " << value.colorBlendOp << std::endl;
			dump << tabs << "\t" << "srcAlphaBlendFactor: " << value.srcAlphaBlendFactor << std::endl;
			dump << tabs << "\t" << "dstAlphaBlendFactor: " << value.dstAlphaBlendFactor << std::endl;
			dump << tabs << "\t" << "alphaBlendOp: " << value.alphaBlendOp << std::endl;
			dump << tabs << "\t" << "colorWriteMask: " << value.colorWriteMask << std::endl;
			dump << tabs << "}" << std::endl;
			return dump.str();
		}

		static inline std::string subDump( VkPipelineColorBlendStateCreateInfo const & value, std::string const & tabs )
		{
			std::stringstream dump;
			dump << tabs << "{" << std::endl;
			dump << tabs << "\t" << "sType: " << value.sType << std::endl;
			dump << tabs << "\t" << "pNext: " << value.pNext << std::endl;
			dump << tabs << "\t" << "flags: " << value.flags << std::endl;
			dump << tabs << "\t" << "logicOpEnable: " << value.logicOpEnable << std::endl;
			dump << tabs << "\t" << "logicOp: " << value.logicOp << std::endl;
			dump << tabs << "\t" << "attachmentCount: " << value.attachmentCount << std::endl;
			dump << tabs << "\t" << "pAttachments: " << std::endl << subDump( value.pAttachments, value.attachmentCount, tabs + "\t" ) << std::endl;
			dump << tabs << "\t" << "blendConstants: " << std::endl << subDump( value.blendConstants, 4u, tabs + "\t" ) << std::endl;
			dump << tabs << "}" << std::endl;
			return dump.str();
		}

		static inline std::string subDump( VkPipelineDynamicStateCreateInfo const & value, std::string const & tabs )
		{
			std::stringstream dump;
			dump << tabs << "{" << std::endl;
			dump << tabs << "\t" << "sType: " << value.sType << std::endl;
			dump << tabs << "\t" << "pNext: " << value.pNext << std::endl;
			dump << tabs << "\t" << "flags: " << value.flags << std::endl;
			dump << tabs << "\t" << "dynamicStateCount: " << value.dynamicStateCount << std::endl;
			dump << tabs << "\t" << "pDynamicStates: " << std::endl << subDump( value.pDynamicStates, value.dynamicStateCount, tabs + "\t" ) << std::endl;
			dump << tabs << "}" << std::endl;
			return dump.str();
		}

		static inline std::string subDump( VkGraphicsPipelineCreateInfo const & value, std::string const & tabs )
		{
			std::stringstream dump;
			dump << tabs << "{" << std::endl;
			dump << tabs << "\t" << "sType: " << value.sType << std::endl;
			dump << tabs << "\t" << "pNext:" << value.pNext << std::endl;
			dump << tabs << "\t" << "flags: " << value.flags << std::endl;
			dump << tabs << "\t" << "stageCount: " << value.stageCount << std::endl;
			dump << tabs << "\t" << "pStages: " << std::endl << subDump( value.pStages, value.stageCount, tabs + "\t" ) << std::endl;
			dump << tabs << "\t" << "pVertexInputState: " << std::endl << subDump( value.pVertexInputState, tabs + "\t" ) << std::endl;
			dump << tabs << "\t" << "pInputAssemblyState: " << std::endl << subDump( value.pInputAssemblyState, tabs + "\t" ) << std::endl;
			dump << tabs << "\t" << "pTessellationState: " << std::endl << subDump( value.pTessellationState, tabs + "\t" ) << std::endl;
			dump << tabs << "\t" << "pViewportState: " << std::endl << subDump( value.pViewportState, tabs + "\t" ) << std::endl;
			dump << tabs << "\t" << "pRasterizationState: " << std::endl << subDump( value.pRasterizationState, tabs + "\t" ) << std::endl;
			dump << tabs << "\t" << "pMultisampleState: " << std::endl << subDump( value.pMultisampleState, tabs + "\t" ) << std::endl;
			dump << tabs << "\t" << "pDepthStencilState: " << std::endl << subDump( value.pDepthStencilState, tabs + "\t" ) << std::endl;
			dump << tabs << "\t" << "pColorBlendState: " << std::endl << subDump( value.pColorBlendState, tabs + "\t" ) << std::endl;
			dump << tabs << "\t" << "pDynamicState: " << std::endl << subDump( value.pDynamicState, tabs + "\t" ) << std::endl;
			dump << tabs << "\t" << "layout: " << value.layout << std::endl;
			dump << tabs << "\t" << "renderPass: " << value.renderPass << std::endl;
			dump << tabs << "\t" << "subpass: " << value.subpass << std::endl;
			dump << tabs << "\t" << "basePipelineHandle: " << value.basePipelineHandle << std::endl;
			dump << tabs << "\t" << "basePipelineIndex: " << value.basePipelineIndex << std::endl;
			dump << tabs << "}" << std::endl;
			return dump.str();
		}

		static inline std::string subDump( VkComputePipelineCreateInfo const & value, std::string const & tabs )
		{
			std::stringstream dump;
			dump << tabs << "{" << std::endl;
			dump << tabs << "\t" << "sType: " << value.sType << std::endl;
			dump << tabs << "\t" << "pNext:" << value.pNext << std::endl;
			dump << tabs << "\t" << "flags: " << value.flags << std::endl;
			dump << tabs << "\t" << "stage: " << std::endl << subDump( value.stage, tabs + "\t" ) << std::endl;
			dump << tabs << "\t" << "layout: " << value.layout << std::endl;
			dump << tabs << "\t" << "basePipelineHandle: " << value.basePipelineHandle << std::endl;
			dump << tabs << "\t" << "basePipelineIndex: " << value.basePipelineIndex << std::endl;
			dump << tabs << "}" << std::endl;
			return dump.str();
		}

		static inline std::string subDump( VkCommandBufferAllocateInfo const & value, std::string const & tabs )
		{
			std::stringstream dump;
			dump << tabs << "{" << std::endl;
			dump << tabs << "\t" << "sType: " << value.sType << std::endl;
			dump << tabs << "\t" << "pNext:" << value.pNext << std::endl;
			dump << tabs << "\t" << "commandPool:" << value.commandPool << std::endl;
			dump << tabs << "\t" << "level:" << value.level << std::endl;
			dump << tabs << "\t" << "commandBufferCount:" << value.commandBufferCount << std::endl;
			dump << tabs << "}" << std::endl;
			return dump.str();
		}

		static inline std::string subDump( VkCommandBufferInheritanceInfo const & value, std::string const & tabs )
		{
			std::stringstream dump;
			dump << tabs << "{" << std::endl;
			dump << tabs << "\t" << "sType: " << value.sType << std::endl;
			dump << tabs << "\t" << "pNext: " << value.pNext << std::endl;
			dump << tabs << "\t" << "renderPass: " << value.renderPass << std::endl;
			dump << tabs << "\t" << "subpass: " << value.subpass << std::endl;
			dump << tabs << "\t" << "framebuffer: " << value.framebuffer << std::endl;
			dump << tabs << "\t" << "occlusionQueryEnable: " << value.occlusionQueryEnable << std::endl;
			dump << tabs << "\t" << "queryFlags: " << value.queryFlags << std::endl;
			dump << tabs << "\t" << "pipelineStatistics: " << value.pipelineStatistics << std::endl;
			dump << tabs << "}" << std::endl;
			return dump.str();
		}

		static inline std::string subDump( VkCommandBufferBeginInfo const & value, std::string const & tabs )
		{
			std::stringstream dump;
			dump << tabs << "{" << std::endl;
			dump << tabs << "\t" << "sType: " << value.sType << std::endl;
			dump << tabs << "\t" << "pNext:" << value.pNext << std::endl;
			dump << tabs << "\t" << "flags: " << value.flags << std::endl;
			dump << tabs << "\t" << "pInheritanceInfo: " << std::endl << subDump( value.pInheritanceInfo, tabs + "\t" ) << std::endl;
			dump << tabs << "}" << std::endl;
			return dump.str();
		}

		static inline std::string subDump( VkSubmitInfo const & value, std::string const & tabs )
		{
			std::stringstream dump;
			dump << tabs << "{" << std::endl;
			dump << tabs << "\t" << "sType: " << value.sType << std::endl;
			dump << tabs << "\t" << "pNext:" << value.pNext << std::endl;
			dump << tabs << "\t" << "waitSemaphoreCount:" << value.waitSemaphoreCount << std::endl;
			dump << tabs << "\t" << "pWaitSemaphores:" << std::endl << subDump( value.pWaitSemaphores, value.waitSemaphoreCount, tabs + "\t" ) << std::endl;
			dump << tabs << "\t" << "pWaitDstStageMask:" << value.pWaitDstStageMask << std::endl;
			dump << tabs << "\t" << "commandBufferCount:" << value.commandBufferCount << std::endl;
			dump << tabs << "\t" << "pCommandBuffers:" << std::endl << subDump( value.pCommandBuffers, value.commandBufferCount, tabs + "\t" ) << std::endl;
			dump << tabs << "\t" << "signalSemaphoreCount:" << value.signalSemaphoreCount << std::endl;
			dump << tabs << "\t" << "pSignalSemaphores:" << std::endl << subDump( value.pSignalSemaphores, value.signalSemaphoreCount, tabs + "\t" ) << std::endl;
			dump << tabs << "}" << std::endl;
			return dump.str();
		}

		static inline std::string subDump( VkPresentInfoKHR const & value, std::string const & tabs )
		{
			std::stringstream dump;
			dump << tabs << "{" << std::endl;
			dump << tabs << "\t" << "sType: " << value.sType << std::endl;
			dump << tabs << "\t" << "pNext:" << value.pNext << std::endl;
			dump << tabs << "\t" << "waitSemaphoreCount:" << value.waitSemaphoreCount << std::endl;
			dump << tabs << "\t" << "pWaitSemaphores:" << std::endl << subDump( value.pWaitSemaphores, value.waitSemaphoreCount, tabs + "\t" ) << std::endl;
			dump << tabs << "\t" << "swapchainCount:" << value.swapchainCount << std::endl;
			dump << tabs << "\t" << "pSwapchains:" << std::endl << subDump( value.pSwapchains, value.swapchainCount, tabs + "\t" ) << std::endl;
			dump << tabs << "\t" << "pImageIndices:" << value.pImageIndices << std::endl;
			dump << tabs << "\t" << "pResults:" << value.pResults << std::endl;
			dump << tabs << "}" << std::endl;
			return dump.str();
		}

		static inline std::string subDump( VkAttachmentReference const & value, std::string const & tabs )
		{
			std::stringstream dump;
			dump << tabs << "{" << std::endl;
			dump << tabs << "\t" << "attachment: " << value.attachment << std::endl;
			dump << tabs << "\t" << "layout: " << value.layout << std::endl;
			dump << tabs << "}" << std::endl;
			return dump.str();
		}

		static inline std::string subDump( VkSubpassDescription const & value, std::string const & tabs )
		{
			std::stringstream dump;
			dump << tabs << "{" << std::endl;
			dump << tabs << "\t" << "flags:" << value.flags << std::endl;
			dump << tabs << "\t" << "pipelineBindPoint:" << value.pipelineBindPoint << std::endl;
			dump << tabs << "\t" << "inputAttachmentCount:" << value.inputAttachmentCount << std::endl;
			dump << tabs << "\t" << "pInputAttachments:" << std::endl << subDump( value.pInputAttachments, value.inputAttachmentCount, tabs + "\t" ) << std::endl;
			dump << tabs << "\t" << "colorAttachmentCount:" << value.colorAttachmentCount << std::endl;
			dump << tabs << "\t" << "pColorAttachments:" << std::endl << subDump( value.pColorAttachments, value.colorAttachmentCount, tabs + "\t" ) << std::endl;
			dump << tabs << "\t" << "pResolveAttachments:" << std::endl << subDump( value.pResolveAttachments, value.colorAttachmentCount, tabs + "\t" ) << std::endl;
			dump << tabs << "\t" << "pDepthStencilAttachment:" << std::endl << subDump( value.pDepthStencilAttachment, tabs + "\t" ) << std::endl;
			dump << tabs << "\t" << "preserveAttachmentCount:" << value.preserveAttachmentCount << std::endl;
			dump << tabs << "\t" << "pPreserveAttachments:" << std::endl << subDump( value.pPreserveAttachments, value.preserveAttachmentCount, tabs + "\t" ) << std::endl;
			dump << tabs << "}" << std::endl;
			return dump.str();
		}

		static inline std::string subDump( VkAttachmentDescription const & value, std::string const & tabs )
		{
			std::stringstream dump;
			dump << tabs << "{" << std::endl;
			dump << tabs << "\t" << "flags:" << value.flags << std::endl;
			dump << tabs << "\t" << "format:" << value.format << std::endl;
			dump << tabs << "\t" << "samples:" << value.samples << std::endl;
			dump << tabs << "\t" << "loadOp:" << value.loadOp << std::endl;
			dump << tabs << "\t" << "storeOp:" << value.storeOp << std::endl;
			dump << tabs << "\t" << "stencilLoadOp:" << value.stencilLoadOp << std::endl;
			dump << tabs << "\t" << "stencilStoreOp:" << value.stencilStoreOp << std::endl;
			dump << tabs << "\t" << "initialLayout:" << value.initialLayout << std::endl;
			dump << tabs << "\t" << "finalLayout:" << value.finalLayout << std::endl;
			dump << tabs << "}" << std::endl;
			return dump.str();
		}

		static inline std::string subDump( VkSubpassDependency const & value, std::string const & tabs )
		{
			std::stringstream dump;
			dump << tabs << "{" << std::endl;
			dump << tabs << "\t" << "srcSubpass:" << value.srcSubpass << std::endl;
			dump << tabs << "\t" << "dstSubpass:" << value.dstSubpass << std::endl;
			dump << tabs << "\t" << "srcStageMask:" << value.srcStageMask << std::endl;
			dump << tabs << "\t" << "dstStageMask:" << value.dstStageMask << std::endl;
			dump << tabs << "\t" << "srcAccessMask:" << value.srcAccessMask << std::endl;
			dump << tabs << "\t" << "dstAccessMask:" << value.dstAccessMask << std::endl;
			dump << tabs << "\t" << "dependencyFlags:" << value.dependencyFlags << std::endl;
			dump << tabs << "}" << std::endl;
			return dump.str();
		}

		static inline std::string subDump( VkRenderPassCreateInfo const & value, std::string const & tabs )
		{
			std::stringstream dump;
			dump << tabs << "{" << std::endl;
			dump << tabs << "\t" << "sType: " << value.sType << std::endl;
			dump << tabs << "\t" << "pNext:" << value.pNext << std::endl;
			dump << tabs << "\t" << "flags:" << value.flags << std::endl;
			dump << tabs << "\t" << "attachmentCount:" << value.attachmentCount << std::endl;
			dump << tabs << "\t" << "pAttachments:" << std::endl << subDump( value.pAttachments, value.attachmentCount, tabs + "\t" ) << std::endl;
			dump << tabs << "\t" << "subpassCount:" << value.subpassCount << std::endl;
			dump << tabs << "\t" << "pSubpasses:" << std::endl << subDump( value.pSubpasses, value.subpassCount, tabs + "\t" ) << std::endl;
			dump << tabs << "\t" << "dependencyCount:" << value.dependencyCount << std::endl;
			dump << tabs << "\t" << "pDependencies:" << std::endl << subDump( value.pDependencies, value.dependencyCount, tabs + "\t" ) << std::endl;
			dump << tabs << "}" << std::endl;
			return dump.str();
		}

		static inline std::string subDump( VkClearColorValue const & value, std::string const & tabs )
		{
			std::stringstream dump;
			dump << tabs << "{" << std::endl;
			dump << tabs << "\t" << "r: " << value.float32[0] << std::endl;
			dump << tabs << "\t" << "g: " << value.float32[1] << std::endl;
			dump << tabs << "\t" << "b: " << value.float32[2] << std::endl;
			dump << tabs << "\t" << "a: " << value.float32[3] << std::endl;
			dump << tabs << "}" << std::endl;
			return dump.str();
		}

		static inline std::string subDump( VkClearDepthStencilValue const & value, std::string const & tabs )
		{
			std::stringstream dump;
			dump << tabs << "{" << std::endl;
			dump << tabs << "\t" << "depth: " << value.depth << std::endl;
			dump << tabs << "\t" << "stencil: " << value.stencil << std::endl;
			dump << tabs << "}" << std::endl;
			return dump.str();
		}

		static inline std::string subDump( VkClearValue const & value, std::string const & tabs )
		{
			std::stringstream dump;
			dump << tabs << "{" << std::endl;
			dump << tabs << "\t" << "color: " << std::endl << subDump( value.color, tabs + "\t" ) << std::endl;
			dump << tabs << "\t" << "depthStencil: " << std::endl << subDump( value.depthStencil, tabs + "\t" ) << std::endl;
			dump << tabs << "}" << std::endl;
			return dump.str();
		}

		static inline std::string subDump( VkRenderPassBeginInfo const & value, std::string const & tabs )
		{
			std::stringstream dump;
			dump << tabs << "{" << std::endl;
			dump << tabs << "\t" << "sType: " << value.sType << std::endl;
			dump << tabs << "\t" << "pNext:" << value.pNext << std::endl;
			dump << tabs << "\t" << "renderPass:" << value.renderPass << std::endl;
			dump << tabs << "\t" << "framebuffer:" << value.framebuffer << std::endl;
			dump << tabs << "\t" << "renderArea:" << std::endl << subDump( value.renderArea, tabs + "\t" ) << std::endl;
			dump << tabs << "\t" << "clearValueCount:" << value.clearValueCount << std::endl;
			dump << tabs << "\t" << "pClearValues:" << std::endl << subDump( value.pClearValues, value.clearValueCount, tabs + "\t" ) << std::endl;
			dump << tabs << "}" << std::endl;
			return dump.str();
		}

		static inline std::string subDump( VkSemaphoreCreateInfo const & value, std::string const & tabs )
		{
			std::stringstream dump;
			dump << tabs << "{" << std::endl;
			dump << tabs << "\t" << "sType: " << value.sType << std::endl;
			dump << tabs << "\t" << "pNext:" << value.pNext << std::endl;
			dump << tabs << "\t" << "flags:" << value.flags << std::endl;
			dump << tabs << "}" << std::endl;
			return dump.str();
		}

		static inline std::string subDump( VkShaderModuleCreateInfo const & value, std::string const & tabs )
		{
			std::stringstream dump;
			dump << tabs << "{" << std::endl;
			dump << tabs << "\t" << "sType: " << value.sType << std::endl;
			dump << tabs << "\t" << "pNext:" << value.pNext << std::endl;
			dump << tabs << "\t" << "flags:" << value.flags << std::endl;
			dump << tabs << "\t" << "codeSize:" << value.codeSize << std::endl;
			dump << tabs << "\t" << "pCode:" << value.pCode << std::endl;
			dump << tabs << "}" << std::endl;
			return dump.str();
		}

		static inline std::string subDump( VkSwapchainCreateInfoKHR const & value, std::string const & tabs )
		{
			std::stringstream dump;
			dump << tabs << "{" << std::endl;
			dump << tabs << "\t" << "sType: " << value.sType << std::endl;
			dump << tabs << "\t" << "pNext:" << value.pNext << std::endl;
			dump << tabs << "\t" << "flags:" << value.flags << std::endl;
			dump << tabs << "\t" << "surface:" << value.surface << std::endl;
			dump << tabs << "\t" << "minImageCount:" << value.minImageCount << std::endl;
			dump << tabs << "\t" << "imageFormat:" << value.imageFormat << std::endl;
			dump << tabs << "\t" << "imageColorSpace:" << value.imageColorSpace << std::endl;
			dump << tabs << "\t" << "imageExtent:" << std::endl << subDump( value.imageExtent, tabs + "\t" ) << std::endl;
			dump << tabs << "\t" << "imageArrayLayers:" << value.imageArrayLayers << std::endl;
			dump << tabs << "\t" << "imageUsage:" << value.imageUsage << std::endl;
			dump << tabs << "\t" << "imageSharingMode:" << value.imageSharingMode << std::endl;
			dump << tabs << "\t" << "queueFamilyIndexCount:" << value.queueFamilyIndexCount << std::endl;
			dump << tabs << "\t" << "pQueueFamilyIndices:" << std::endl << subDump( value.pQueueFamilyIndices, value.queueFamilyIndexCount, tabs + "\t" ) << std::endl;
			dump << tabs << "\t" << "preTransform:" << value.preTransform << std::endl;
			dump << tabs << "\t" << "compositeAlpha:" << value.compositeAlpha << std::endl;
			dump << tabs << "\t" << "presentMode:" << value.presentMode << std::endl;
			dump << tabs << "\t" << "clipped:" << value.clipped << std::endl;
			dump << tabs << "\t" << "oldSwapchain:" << value.oldSwapchain << std::endl;
			dump << tabs << "}" << std::endl;
			return dump.str();
		}

		static inline std::string subDump( VkViewport const & value, std::string const & tabs )
		{
			std::stringstream dump;
			dump << tabs << "{" << std::endl;
			dump << tabs << "\t" << "width: " << value.width << std::endl;
			dump << tabs << "\t" << "height: " << value.height << std::endl;
			dump << tabs << "\t" << "x: " << value.x << std::endl;
			dump << tabs << "\t" << "y: " << value.y << std::endl;
			dump << tabs << "\t" << "minDepth: " << value.minDepth << std::endl;
			dump << tabs << "\t" << "maxDepth: " << value.maxDepth << std::endl;
			dump << tabs << "}" << std::endl;
			return dump.str();
		}

		static inline std::string subDump( VkExtent2D const & value, std::string const & tabs )
		{
			std::stringstream dump;
			dump << tabs << "{" << std::endl;
			dump << tabs << "\t" << "width: " << value.width << std::endl;
			dump << tabs << "\t" << "height: " << value.height << std::endl;
			dump << tabs << "}" << std::endl;
			return dump.str();
		}

		static inline std::string subDump( VkOffset2D const & value, std::string const & tabs )
		{
			std::stringstream dump;
			dump << tabs << "{" << std::endl;
			dump << tabs << "\t" << "x: " << value.x << std::endl;
			dump << tabs << "\t" << "y: " << value.y << std::endl;
			dump << tabs << "}" << std::endl;
			return dump.str();
		}

		static inline std::string subDump( VkExtent3D const & value, std::string const & tabs )
		{
			std::stringstream dump;
			dump << tabs << "{" << std::endl;
			dump << tabs << "\t" << "width: " << value.width << std::endl;
			dump << tabs << "\t" << "height: " << value.height << std::endl;
			dump << tabs << "\t" << "depth: " << value.depth << std::endl;
			dump << tabs << "}" << std::endl;
			return dump.str();
		}

		static inline std::string subDump( VkOffset3D const & value, std::string const & tabs )
		{
			std::stringstream dump;
			dump << tabs << "{" << std::endl;
			dump << tabs << "\t" << "x: " << value.x << std::endl;
			dump << tabs << "\t" << "y: " << value.y << std::endl;
			dump << tabs << "\t" << "z: " << value.z << std::endl;
			dump << tabs << "}" << std::endl;
			return dump.str();
		}

		static inline std::string subDump( VkRect2D const & value, std::string const & tabs )
		{
			std::stringstream dump;
			dump << tabs << "{" << std::endl;
			dump << tabs << "\t" << "extent: " << std::endl << subDump( value.extent, tabs + "\t" ) << std::endl;
			dump << tabs << "\t" << "offset: " << std::endl << subDump( value.offset, tabs + "\t" ) << std::endl;
			dump << tabs << "}" << std::endl;
			return dump.str();
		}

		static inline std::string subDump( VkImageSubresourceRange const & value, std::string const & tabs )
		{
			std::stringstream dump;
			dump << tabs << "{" << std::endl;
			dump << tabs << "\t" << "aspectMask: " << value.aspectMask << std::endl;
			dump << tabs << "\t" << "baseMipLevel: " << value.baseMipLevel << std::endl;
			dump << tabs << "\t" << "levelCount: " << value.levelCount << std::endl;
			dump << tabs << "\t" << "baseArrayLayer: " << value.baseArrayLayer << std::endl;
			dump << tabs << "\t" << "layerCount: " << value.layerCount << std::endl;
			dump << tabs << "}" << std::endl;
			return dump.str();
		}

		static inline std::string subDump( VkImageMemoryBarrier const & value, std::string const & tabs )
		{
			std::stringstream dump;
			dump << tabs << "{" << std::endl;
			dump << tabs << "\t" << "sType: " << value.sType << std::endl;
			dump << tabs << "\t" << "pNext: " << value.pNext << std::endl;
			dump << tabs << "\t" << "srcAccessMask: " << value.srcAccessMask << std::endl;
			dump << tabs << "\t" << "dstAccessMask: " << value.dstAccessMask << std::endl;
			dump << tabs << "\t" << "oldLayout: " << value.oldLayout << std::endl;
			dump << tabs << "\t" << "newLayout: " << value.newLayout << std::endl;
			dump << tabs << "\t" << "srcQueueFamilyIndex: " << value.srcQueueFamilyIndex << std::endl;
			dump << tabs << "\t" << "dstQueueFamilyIndex: " << value.dstQueueFamilyIndex << std::endl;
			dump << tabs << "\t" << "image: " << value.image << std::endl;
			dump << tabs << "\t" << "subresourceRange: " << std::endl << subDump( value.subresourceRange, tabs + "\t" ) << std::endl;
			dump << tabs << "}" << std::endl;
			return dump.str();
		}

		static inline std::string subDump( VkCommandPoolCreateInfo const & value, std::string const & tabs )
		{
			std::stringstream dump;
			dump << tabs << "{" << std::endl;
			dump << tabs << "\t" << "sType: " << value.sType << std::endl;
			dump << tabs << "\t" << "pNext: " << value.pNext << std::endl;
			dump << tabs << "\t" << "flags: " << value.flags << std::endl;
			dump << tabs << "\t" << "queueFamilyIndex: " << value.queueFamilyIndex << std::endl;
			dump << tabs << "}" << std::endl;
			return dump.str();
		}

		static inline std::string subDump( VkFenceCreateInfo const & value, std::string const & tabs )
		{
			std::stringstream dump;
			dump << tabs << "{" << std::endl;
			dump << tabs << "\t" << "sType: " << value.sType << std::endl;
			dump << tabs << "\t" << "pNext: " << value.pNext << std::endl;
			dump << tabs << "\t" << "flags: " << value.flags << std::endl;
			dump << tabs << "}" << std::endl;
			return dump.str();
		}

		static inline std::string subDump( VkFramebufferCreateInfo const & value, std::string const & tabs )
		{
			std::stringstream dump;
			dump << tabs << "{" << std::endl;
			dump << tabs << "\t" << "sType: " << value.sType << std::endl;
			dump << tabs << "\t" << "pNext: " << value.pNext << std::endl;
			dump << tabs << "\t" << "flags: " << value.flags << std::endl;
			dump << tabs << "\t" << "renderPass: " << value.renderPass << std::endl;
			dump << tabs << "\t" << "attachmentCount: " << value.attachmentCount << std::endl;
			dump << tabs << "\t" << "pAttachments: " << std::endl << subDump( value.pAttachments, value.attachmentCount, tabs + "\t" ) << std::endl;
			dump << tabs << "\t" << "width: " << value.width << std::endl;
			dump << tabs << "\t" << "height: " << value.height << std::endl;
			dump << tabs << "\t" << "layers: " << value.layers << std::endl;
			dump << tabs << "}" << std::endl;
			return dump.str();
		}

		static inline std::string subDump( VkComponentMapping const & value, std::string const & tabs )
		{
			std::stringstream dump;
			dump << tabs << "{" << std::endl;
			dump << tabs << "\t" << "r: " << value.r << std::endl;
			dump << tabs << "\t" << "g: " << value.g << std::endl;
			dump << tabs << "\t" << "b: " << value.b << std::endl;
			dump << tabs << "\t" << "a: " << value.a << std::endl;
			dump << tabs << "}" << std::endl;
			return dump.str();
		}

		static inline std::string subDump( VkImageViewCreateInfo const & value, std::string const & tabs )
		{
			std::stringstream dump;
			dump << tabs << "{" << std::endl;
			dump << tabs << "\t" << "sType: " << value.sType << std::endl;
			dump << tabs << "\t" << "pNext: " << value.pNext << std::endl;
			dump << tabs << "\t" << "flags: " << value.flags << std::endl;
			dump << tabs << "\t" << "image: " << value.image << std::endl;
			dump << tabs << "\t" << "viewType: " << value.viewType << std::endl;
			dump << tabs << "\t" << "format: " << value.format << std::endl;
			dump << tabs << "\t" << "components: " << std::endl << subDump( value.components, tabs + "\t" ) << std::endl;
			dump << tabs << "\t" << "subresourceRange: " << std::endl << subDump( value.subresourceRange, tabs + "\t" ) << std::endl;
			dump << tabs << "}" << std::endl;
			return dump.str();
		}

		static inline std::string subDump( VkBufferViewCreateInfo const & value, std::string const & tabs )
		{
			std::stringstream dump;
			dump << tabs << "{" << std::endl;
			dump << tabs << "\t" << "sType: " << value.sType << std::endl;
			dump << tabs << "\t" << "pNext: " << value.pNext << std::endl;
			dump << tabs << "\t" << "flags: " << value.flags << std::endl;
			dump << tabs << "\t" << "buffer: " << value.buffer << std::endl;
			dump << tabs << "\t" << "format: " << value.format << std::endl;
			dump << tabs << "\t" << "offset: " << value.offset << std::endl;
			dump << tabs << "\t" << "range: " << value.range << std::endl;
			dump << tabs << "}" << std::endl;
			return dump.str();
		}

		static inline std::string subDump( VkQueryPoolCreateInfo const & value, std::string const & tabs )
		{
			std::stringstream dump;
			dump << tabs << "{" << std::endl;
			dump << tabs << "\t" << "sType: " << value.sType << std::endl;
			dump << tabs << "\t" << "pNext: " << value.pNext << std::endl;
			dump << tabs << "\t" << "flags: " << value.flags << std::endl;
			dump << tabs << "\t" << "queryType: " << value.queryType << std::endl;
			dump << tabs << "\t" << "queryCount: " << value.queryCount << std::endl;
			dump << tabs << "\t" << "pipelineStatistics: " << value.pipelineStatistics << std::endl;
			dump << tabs << "}" << std::endl;
			return dump.str();
		}

#ifdef VK_USE_PLATFORM_WIN32_KHR

		static inline std::string subDump( VkWin32SurfaceCreateInfoKHR const & value, std::string const & tabs )
		{
			std::stringstream dump;
			dump << tabs << "{" << std::endl;
			dump << tabs << "\t" << "sType: " << value.sType << std::endl;
			dump << tabs << "\t" << "pNext: " << value.pNext << std::endl;
			dump << tabs << "\t" << "flags: " << value.flags << std::endl;
			dump << tabs << "\t" << "hinstance: " << value.hinstance << std::endl;
			dump << tabs << "\t" << "hwnd: " << value.hwnd << std::endl;
			dump << tabs << "}" << std::endl;
			return dump.str();
		}

#endif

	private:
		static std::multimap< std::string, std::string > m_dump;
	};
}

#if !defined( NDEBUG )

//!@~french		Dump une/des variable(s) Vulkan
//!@~english	Dumps Vulkan variable(s)
#	define DEBUG_DUMP( value )\
	vk_renderer::Debug::dump( value )

//!@~french		Ecrit le contenu du dump dans un flux.
//!@~english	Writes the trace content to a stream.
#	define DEBUG_WRITE( file )\
	std::ofstream debugDumpFile{ file };\
	vk_renderer::Debug::write( debugDumpFile )

#else

//!@~french		Dump une/des variable(s) Vulkan
//!@~english	Dumps Vulkan variable(s)
#	define DEBUG_DUMP( value )

//!@~french		Ecrit le contenu du dump dans un flux.
//!@~english	Writes the trace content to a stream.
#	define DEBUG_WRITE( file )

#endif
