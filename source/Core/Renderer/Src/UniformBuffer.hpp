/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_UniformBuffer_HPP___
#define ___Renderer_UniformBuffer_HPP___
#pragma once

#include "Buffer.hpp"
#include "BufferTarget.hpp"
#include "MemoryPropertyFlag.hpp"
#include "RenderingResources.hpp"

namespace renderer
{
	/**
	*\brief
	*	Représente un tampon de variables uniformes.
	*/
	class UniformBufferBase
	{
	protected:
		/**
		*\brief
		*	Constructeur.
		*\param[in] device
		*	Le périphérique logique.
		*\param[in] count
		*	Le nombre d'instance des données.
		*\param[in] size
		*	La taille d'une instance des données, en octets.
		*\param[in] target
		*	Les indicateurs d'utilisation du tampon.
		*\param[in] flags
		*	Les indicateurs de mémoire du tampon.
		*/
		UniformBufferBase( Device const & device
			, uint32_t count
			, uint32_t size
			, BufferTargets target
			, MemoryPropertyFlags flags );

	public:
		/**
		*\~english
		*\brief
		*	Destructor.
		*\~french
		*\brief
		*	Destructeur.
		*/
		virtual ~UniformBufferBase() = default;
		/**
		*\brief
		*	Récupère l'offset dans le buffer pour un nombre d'éléments donné.
		*\param[in] count
		*	Le nombre d'éléments.
		*\return
		*	L'offset réel.
		*/
		virtual uint32_t getOffset( uint32_t count )const = 0;
		/**
		*\return
		*	Le tampon GPU.
		*/
		inline BufferBase const & getBuffer()const
		{
			return *m_buffer;
		}
		/**
		*\return
		*	La taille d'une instance des données du tampon.
		*/
		inline uint32_t getSize()const
		{
			return m_size;
		}

	protected:
		/**
		*\brief
		*	Crée le tampon GPU.
		*\param[in] count
		*	Le nombre d'instance des données.
		*\param[in] target
		*	Les indicateurs d'utilisation du tampon.
		*\param[in] flags
		*	Les indicateurs de mémoire du tampon.
		*/
		virtual void doCreateBuffer( uint32_t count
			, BufferTargets target
			, MemoryPropertyFlags flags ) = 0;

	protected:
		Device const & m_device;
		uint32_t m_count;
		uint32_t m_size;
		BufferBasePtr m_buffer;
	};
	/**
	*\brief
	*	Classe template wrappant un UniformBufferBase.
	*/
	template< typename T >
	class UniformBuffer
	{
	public:
		/**
		*\brief
		*	Constructeur.
		*\param[in] device
		*	Le périphérique logique.
		*\param[in] count
		*	Le nombre d'instance des données.
		*\param[in] target
		*	Les indicateurs d'utilisation du tampon.
		*\param[in] flags
		*	Les indicateurs de mémoire du tampon.
		*/
		inline UniformBuffer( Device const & device
			, uint32_t count
			, BufferTargets target
			, MemoryPropertyFlags flags );
		/**
		*\return
		*	La n-ème instance des données.
		*/
		inline T const & getData( uint32_t index = 0 )const
		{
			return m_data[index];
		}
		/**
		*\return
		*	La n-ème instance des données.
		*/
		inline T & getData( uint32_t index = 0 )
		{
			return m_data[index];
		}
		/**
		*\return
		*	Les données.
		*/
		inline std::vector< T > const & getDatas( uint32_t index = 0 )const
		{
			return m_data;
		}
		/**
		*\return
		*	Les données.
		*/
		inline std::vector< T > & getDatas( uint32_t index = 0 )
		{
			return m_data;
		}
		/**
		*\brief
		*	Récupère l'offset dans le buffer pour un nombre d'éléments donné.
		*\param[in] count
		*	Le nombre d'éléments.
		*\return
		*	L'offset réel.
		*/
		inline uint32_t getOffset( uint32_t count )const
		{
			return m_ubo->getOffset( count );
		}
		/**
		*\return
		*	Le tampon GPU.
		*/
		inline UniformBufferBase const & getUbo()const
		{
			return *m_ubo;
		}

	private:
		UniformBufferBasePtr m_ubo;
		std::vector< T > m_data;
	};
}

#include "UniformBuffer.inl"

#endif
