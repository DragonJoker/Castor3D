/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_SpecialisationInfo_HPP___
#define ___Renderer_SpecialisationInfo_HPP___
#pragma once

#include "SpecialisationMapEntry.hpp"

namespace renderer
{
	/**
	*\~english
	*\brief
	*	Specifies a constants specialisation info.
	*\~french
	*\brief
	*	Définit les informations de spécialisation des constantes.
	*/
	class SpecialisationInfoBase
	{
	public:
		/**
		*\~english
		*\brief
		*	Constructor.
		*\param[in] entries
		*	The constants contained in the buffer.
		*\~french
		*\brief
		*	Constructeur.
		*\param[in] entries
		*	Les constantes contenues dans le tampon.
		*/
		SpecialisationInfoBase( SpecialisationMapEntryArray const & entries );
		/**
		*\~english
		*\return
		*	The data size.
		*\~french
		*\return
		*	La taille des données.
		*/
		inline uint32_t getSize()const
		{
			return uint32_t( m_data.size() );
		}
		/**
		*\~english
		*\brief
		*	A pointer to the buffer data.
		*\~french
		*\brief
		*	Un pointeur sur les données du tampon.
		*/
		inline uint8_t const * getData()const
		{
			return m_data.data();
		}
		/**
		*\~english
		*\brief
		*	A pointer to the buffer data.
		*\~french
		*\brief
		*	Un pointeur sur les données du tampon.
		*/
		inline uint8_t * getData()
		{
			return m_data.data();
		}
		/**
		*\~english
		*\return
		*	The beginning of the constants array.
		*\~french
		*\return
		*	Le début du tableau de constantes.
		*/
		inline SpecialisationMapEntryArray::const_iterator begin()const
		{
			return m_entries.begin();
		}
		/**
		*\~english
		*\return
		*	The end of the constants array.
		*\~french
		*\return
		*	La fin du tableau de constantes.
		*/
		inline SpecialisationMapEntryArray::const_iterator end()const
		{
			return m_entries.end();
		}

	private:
		SpecialisationMapEntryArray m_entries;
		renderer::ByteArray m_data;
	};
	/**
	*\~english
	*\brief
	*	Template class wrapping a SpecialisationInfoBase.
	*\~french
	*\brief
	*	Classe template wrappant un SpecialisationInfoBase.
	*/
	template< typename T >
	class SpecialisationInfo
		: public SpecialisationInfoBase
	{
	public:
		/**
		*\~english
		*\brief
		*	Constructor.
		*\param[in] entries
		*	The constants contained in the buffer.
		*\~french
		*\brief
		*	Constructeur.
		*\param[in] entries
		*	Les constantes contenues dans le tampon.
		*/
		SpecialisationInfo( SpecialisationMapEntryArray const & entries
			, T const & value = T{} )
			: SpecialisationInfoBase{ entries }
		{
			getData() = value;
		}
		/**
		*\~english
		*\brief
		*	A pointer to the buffer data.
		*\~french
		*\brief
		*	Un pointeur sur les données du tampon.
		*/
		inline T const & getData()const
		{
			return *reinterpret_cast< T const * >( SpecialisationInfoBase::getData() );
		}
		/**
		*\~english
		*\brief
		*	A pointer to the buffer data.
		*\~french
		*\brief
		*	Un pointeur sur les données du tampon.
		*/
		inline T & getData()
		{
			return *reinterpret_cast< T * >( SpecialisationInfoBase::getData() );
		}
	};
	/**
	*\~french
	*\brief
	*	SpecialisationInfo creation helper function.
	*\param[in] entries
	*	The constants contained in the buffer.
	*\param[in] value
	*	The constants value.
	*\~french
	*\brief
	*	Fonction d'aide à la création d'un SpecialisationInfo.
	*\param[in] entries
	*	Les constantes contenues dans le tampon.
	*\param[in] value
	*	La valeur des constantes.
	*/
	template< typename T >
	inline SpecialisationInfoPtr< T > makeSpecialisationInfo( SpecialisationMapEntryArray const & entries
		, T const & value )
	{
		return std::make_unique< SpecialisationInfo< T > >( entries, value );
	}
}

#endif
