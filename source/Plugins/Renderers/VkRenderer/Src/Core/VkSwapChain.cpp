#include "Core/VkSwapChain.hpp"

#include "Command/VkCommandBuffer.hpp"
#include "Command/VkCommandPool.hpp"
#include "Command/VkQueue.hpp"
#include "Core/VkBackBuffer.hpp"
#include "Core/VkDevice.hpp"
#include "Core/VkPhysicalDevice.hpp"
#include "Core/VkRenderer.hpp"
#include "Image/VkTexture.hpp"
#include "RenderPass/VkFrameBuffer.hpp"
#include "RenderPass/VkRenderPass.hpp"
#include "Sync/VkImageMemoryBarrier.hpp"
#include "Sync/VkSemaphore.hpp"

#include <RenderPass/FrameBufferAttachment.hpp>

namespace vk_renderer
{
	SwapChain::SwapChain( Device const & device
		, renderer::Extent2D const & size )
		: renderer::SwapChain{ device, size }
		, m_device{ device }
		, m_surface{ device.getPresentSurface() }
	{
		m_surfaceCapabilities = m_device.getSurfaceCapabilities();

		// On choisit le format de la surface.
		doSelectFormat( static_cast< PhysicalDevice const & >( m_device.getPhysicalDevice() ) );

		// On crée la swap chain.
		doCreateSwapChain();

		// Puis les tampons d'images.
		doCreateBackBuffers();

		m_renderingResources.resize( 3 );

		for ( auto & resource : m_renderingResources )
		{
			resource = std::make_unique< RenderingResources >( device );
		}
	}

	SwapChain::~SwapChain()
	{
		m_backBuffers.clear();
		m_device.vkDestroySwapchainKHR( m_device, m_swapChain, nullptr );
	}

	void SwapChain::reset( renderer::Extent2D const & size )
	{
		m_dimensions = size;
		doResetSwapChain();
	}

	void SwapChain::createDepthStencil( renderer::Format format )
	{
		m_depthStencil = m_device.createTexture(
			{
				0u,
				renderer::TextureType::e2D,
				format,
				renderer::Extent3D{ getDimensions().width, getDimensions().height, 1u },
				1u,
				1u,
				renderer::SampleCountFlag::e1,
				renderer::ImageTiling::eOptimal,
				renderer::ImageUsageFlag::eDepthStencilAttachment,
				renderer::SharingMode::eExclusive,
				{},
				renderer::ImageLayout::eUndefined,
			} );
		m_depthStencil->bindMemory( m_device.allocateMemory( m_depthStencil->getMemoryRequirements()
			, renderer::MemoryPropertyFlag::eDeviceLocal ) );
		m_depthStencilView = m_depthStencil->createView( renderer::TextureViewType::e2D
			, format );
	}

	renderer::FrameBufferAttachmentArray SwapChain::doPrepareAttaches( uint32_t backBuffer
		, renderer::AttachmentDescriptionArray const & attaches )const
	{
		renderer::FrameBufferAttachmentArray result;

		for ( auto & attach : attaches )
		{
			if ( !renderer::isDepthOrStencilFormat( attach.format ) )
			{
				result.emplace_back( attach, m_backBuffers[backBuffer]->getView() );
			}
			else
			{
				assert( m_depthStencilView );
				result.emplace_back( attach, *m_depthStencilView );
			}
		}

		return result;
	}

	renderer::FrameBufferPtrArray SwapChain::createFrameBuffers( renderer::RenderPass const & renderPass )const
	{
		renderer::FrameBufferPtrArray result;
		result.resize( m_backBuffers.size() );

		for ( size_t i = 0u; i < result.size(); ++i )
		{
			auto attaches = doPrepareAttaches( uint32_t( i ), renderPass.getAttachments() );
			result[i] = static_cast< RenderPass const & >( renderPass ).createFrameBuffer( m_dimensions
				, std::move( attaches ) );
		}

		return result;
	}

	renderer::CommandBufferPtrArray SwapChain::createCommandBuffers()const
	{
		renderer::CommandBufferPtrArray result;
		result.resize( m_backBuffers.size() );

		for ( auto & commandBuffer : result )
		{
			commandBuffer = std::make_unique< CommandBuffer >( m_device
				, static_cast< CommandPool const & >( m_device.getGraphicsCommandPool() )
				, true );
		}

		return result;
	}

	renderer::RenderingResources * SwapChain::getResources()
	{
		auto & resources = *m_renderingResources[m_resourceIndex];
		m_resourceIndex = ( m_resourceIndex + 1 ) % m_renderingResources.size();

		if ( resources.waitRecord( renderer::FenceTimeout ) )
		{
			uint32_t backBuffer{ 0u };
			auto res = m_device.vkAcquireNextImageKHR( m_device
				, m_swapChain
				, std::numeric_limits< uint64_t >::max()
				, static_cast< Semaphore const & >( resources.getImageAvailableSemaphore() )
				, VK_NULL_HANDLE
				, &backBuffer );
			checkError( res );

			if ( doCheckNeedReset( res, true, "Swap chain image acquisition" ) )
			{
				resources.setBackBuffer( backBuffer );
				return &resources;
			}

			return nullptr;
		}

		std::cerr << "Can't render: " << getLastError() << std::endl;
		return nullptr;
	}

	void SwapChain::present( renderer::RenderingResources & resources )
	{
		auto res = static_cast< Queue const & >( m_device.getPresentQueue() ).presentBackBuffer( { *this }
			, { resources.getBackBuffer() }
			, { static_cast< Semaphore const & >( resources.getRenderingFinishedSemaphore() ) } );
		checkError( res );
		doCheckNeedReset( res, false, "Image presentation" );
		resources.setBackBuffer( ~0u );
	}

	uint32_t SwapChain::doGetImageCount()
	{
		uint32_t desiredNumberOfSwapChainImages{ m_surfaceCapabilities.minImageCount + 1 };

		if ( ( m_surfaceCapabilities.maxImageCount > 0 ) &&
			( desiredNumberOfSwapChainImages > m_surfaceCapabilities.maxImageCount ) )
		{
			// L'application doit s'initialiser avec moins d'images que voulu.
			desiredNumberOfSwapChainImages = m_surfaceCapabilities.maxImageCount;
		}

		return desiredNumberOfSwapChainImages;
	}


	void SwapChain::doSelectFormat( VkPhysicalDevice gpu )
	{
		// On récupère la liste de VkFormat supportés par la surface.
		uint32_t formatCount{ 0u };
		auto res = m_device.getRenderer().vkGetPhysicalDeviceSurfaceFormatsKHR( gpu
			, m_surface
			, &formatCount
			, nullptr );

		if ( !checkError( res ) )
		{
			throw std::runtime_error{ "Surface formats enumeration failed: " + getLastError() };
		}

		std::vector< VkSurfaceFormatKHR > surfFormats{ formatCount };
		res = m_device.getRenderer().vkGetPhysicalDeviceSurfaceFormatsKHR( gpu, m_surface
			, &formatCount
			, surfFormats.data() );

		if ( !checkError( res ) )
		{
			throw std::runtime_error{ "Surface formats enumeration failed: " + getLastError() };
		}

		// Si la liste de formats ne contient qu'une entrée VK_FORMAT_UNDEFINED,
		// la surface n'a pas de format préféré. Sinon, au moins un format supporté
		// sera renvoyé.
		if ( formatCount == 1u && surfFormats[0].format == VK_FORMAT_UNDEFINED )
		{
			m_format = convert( VK_FORMAT_R8G8B8A8_UNORM );
		}
		else
		{
			assert( formatCount >= 1u );
			m_format = convert( surfFormats[0].format );
		}

		m_colorSpace = surfFormats[0].colorSpace;
	}

	VkPresentModeKHR SwapChain::doSelectPresentMode()
	{
		// On récupère la liste de VkPresentModeKHR supportés par la surface.
		uint32_t presentModeCount{};
		auto res = m_device.getRenderer().vkGetPhysicalDeviceSurfacePresentModesKHR( static_cast< PhysicalDevice const & >( m_device.getPhysicalDevice() )
			, m_surface
			, &presentModeCount
			, nullptr );

		if ( !checkError( res ) )
		{
			throw std::runtime_error{ "Surface present modes enumeration failed: " + getLastError() };
		}

		std::vector< VkPresentModeKHR > presentModes{ presentModeCount };
		res = m_device.getRenderer().vkGetPhysicalDeviceSurfacePresentModesKHR( static_cast< PhysicalDevice const & >( m_device.getPhysicalDevice() )
			, m_surface
			, &presentModeCount
			, presentModes.data() );

		if ( !checkError( res ) )
		{
			throw std::runtime_error{ "Surface present modes enumeration failed: " + getLastError() };
		}

		// Si le mode bo�te aux lettres est disponible, on utilise celui-là, car c'est celui avec le
		// minimum de latence dans tearing.
		// Sinon, on essaye le mode IMMEDIATE, qui est normalement disponible, et est le plus rapide
		// (bien qu'il y ait du tearing). Sinon on utilise le mode FIFO qui est toujours disponible.
		VkPresentModeKHR swapchainPresentMode{ VK_PRESENT_MODE_FIFO_KHR };

		for ( auto mode : presentModes )
		{
			if ( mode == VK_PRESENT_MODE_MAILBOX_KHR )
			{
				swapchainPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
				break;
			}

			if ( ( swapchainPresentMode != VK_PRESENT_MODE_MAILBOX_KHR ) &&
				( mode == VK_PRESENT_MODE_IMMEDIATE_KHR ) )
			{
				swapchainPresentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
			}
		}

		return swapchainPresentMode;
	}

	void SwapChain::doCreateSwapChain()
	{
		VkExtent2D swapChainExtent{};

		// width et height valent soient tous les deux -1 ou tous les deux autre chose que -1.
		if ( m_surfaceCapabilities.currentExtent.width == uint32_t( -1 ) )
		{
			// Si les dimensions de la surface sont indéfinies, elles sont initialisées
			// aux dimensions des images requises.
			swapChainExtent = convert( m_dimensions );
		}
		else
		{
			// Si les dimensions de la surface sont définies, alors les dimensions de la swap chain
			// doivent correspondre.
			swapChainExtent = m_surfaceCapabilities.currentExtent;
		}

		// Parfois, les images doivent être transformées avant d'être présentées (lorsque l'orientation
		// du périphérique est différente de l'orientation par défaut, par exemple).
		// Si la transformation spécifiée est différente de la transformation par défaut, le moteur de 
		// présentation va transformer l'image lors de la présentation. Cette opération peut avoir un
		// impact sur les performances sur certaines plateformes.
		// Ici, nous ne voulons aucune transformation, donc si la transformation identité est supportée,
		// nous l'utilisons, sinon nous utiliserons la même transformation que la transformation courante.
		VkSurfaceTransformFlagBitsKHR preTransform{};

		if ( m_surfaceCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR )
		{
			preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
		}
		else
		{
			preTransform = m_surfaceCapabilities.currentTransform;
		}

		VkSwapchainKHR oldSwapChain{ VK_NULL_HANDLE };

		VkSwapchainCreateInfoKHR createInfo
		{
			VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
			nullptr,
			0,                                                                        // flags
			m_surface,                                                                // surface
			doGetImageCount(),                                                        // minImageCount
			convert( m_format ),                                                      // imageFormat
			m_colorSpace,                                                             // imageColorSpace
			swapChainExtent,                                                          // imageExtent
			1,                                                                        // imageArrayLayers
			VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,                                      // imageUsage
			VK_SHARING_MODE_EXCLUSIVE,                                                // imageSharingMode
			0,                                                                        // queueFamilyIndexCount
			nullptr,                                                                  // pQueueFamilyIndices
			preTransform,                                                             // preTransform
			VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,                                        // compositeAlpha
			doSelectPresentMode(),                                                    // presentMode
			true,                                                                     // clipped
			oldSwapChain,                                                             // oldSwapchain
		};
		DEBUG_DUMP( createInfo );
		auto res = m_device.vkCreateSwapchainKHR( m_device
			, &createInfo
			, nullptr
			, &m_swapChain );

		if ( !checkError( res ) )
		{
			throw std::runtime_error{ "Swap chain creation failed: " + getLastError() };
		}

		// On supprime la précédente swap chain au cas où il y en avait une.
		if ( oldSwapChain != VK_NULL_HANDLE )
		{
			m_device.vkDestroySwapchainKHR( m_device
				, oldSwapChain
				, nullptr );
		}
	}

	void SwapChain::doCreateBackBuffers()
	{
		// On récupère les images de la swapchain.
		uint32_t imageCount{ 0u };
		auto res = m_device.vkGetSwapchainImagesKHR( m_device
			, m_swapChain
			, &imageCount
			, nullptr );

		if ( !checkError( res ) || ( imageCount == 0 ) )
		{
			throw std::runtime_error{ "Swap chain images count retrieval failed: " + getLastError() };
		}

		std::vector< VkImage > swapChainImages( imageCount );
		res = m_device.vkGetSwapchainImagesKHR( m_device
			, m_swapChain
			, &imageCount
			, &swapChainImages[0] );

		if ( !checkError( res ) )
		{
			throw std::runtime_error{ "Swap chain images retrieval failed: " + getLastError() };
		}

		// Et on crée des BackBuffers à partir de ces images.
		m_backBuffers.reserve( imageCount );
		uint32_t index{ 0u };

		for ( auto image : swapChainImages )
		{
			auto texture = std::make_unique< Texture >( m_device
				, m_format
				, m_dimensions
				, image );
			auto & ref = *texture;
			m_backBuffers.emplace_back( std::make_unique< BackBuffer >( m_device
				, std::move( texture )
				, index++
				, m_format
				, ref ) );
		}
	}

	bool SwapChain::doCheckNeedReset( VkResult errCode
		, bool acquisition
		, char const * const action )
	{
		bool result{ false };

		switch ( errCode )
		{
		case VK_SUCCESS:
			result = true;
			break;

		case VK_ERROR_OUT_OF_DATE_KHR:
			if ( !acquisition )
			{
				doResetSwapChain();
			}
			else
			{
				result = true;
			}
			break;

		case VK_SUBOPTIMAL_KHR:
			doResetSwapChain();
			break;

		default:
			std::cerr << action << " failed: " << getLastError() << std::endl;
			break;
		}

		return result;
	}

	void SwapChain::doResetSwapChain()
	{
		m_device.waitIdle();
		auto colour = m_clearColour;
		m_backBuffers.clear();
		m_device.vkDestroySwapchainKHR( m_device, m_swapChain, nullptr );
		m_renderingResources.clear();
		m_surfaceCapabilities = m_device.getSurfaceCapabilities();
		// On choisit le format de la surface.
		doSelectFormat( static_cast< PhysicalDevice const & >( m_device.getPhysicalDevice() ) );
		// On crée la swap chain.
		doCreateSwapChain();
		// Puis les tampons d'images.
		doCreateBackBuffers();

		m_renderingResources.resize( 3 );

		for ( auto & resource : m_renderingResources )
		{
			resource = std::make_unique< RenderingResources >( m_device );
		}

		onReset();
	}
}
