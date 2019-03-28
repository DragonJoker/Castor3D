/*
See LICENSE file in root folder
*/
#ifndef ___C3D_PATTERN_H___
#define ___C3D_PATTERN_H___

#include "Castor3D/Castor3DPrerequisites.hpp"

namespace castor3d
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.1
	\date		09/02/2010
	\~english
	\brief		Pattern handler class
	\remark		A pattern is a collection of consecutive points
	\~french
	\brief		Classe de gestion de chemin
	\remark		Un chemin est une collection de points consécutifs
	*/
	template< typename T >
	class Pattern
	{
	private:
		typedef T TObj;
		typedef T & TObjRef;
		typedef T const & TObjConstRef;
		typedef typename std::list< TObj > TObjList;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		Pattern()
		{
		}
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		~Pattern()
		{
			m_elements.clear();
		}
		/**
		 *\~english
		 *\brief		Builds the mirror pattern
		 *\return		The built pattern
		 *\~french
		 *\brief		Constuir le chemin miroir
		 *\return		Le chemin construit
		 */
		Pattern< T > getReversed()const
		{
			Pattern< T > result;

			for ( auto & element : m_elements )
			{
				result.m_elements.push_front( element );
			}

			return result;
		}
		/**
		 *\~english
		 *\brief		Tells if the pattern is closed (first element is also the last)
		 *\return		\p true if closed, \p false if not
		 *\~french
		 *\brief		Dit si le chemin est fermé (le premier élément est aussi le dernier)
		 *\return		\p true si fermé, \p false sinon
		 */
		bool isClosed()const
		{
			TObj const & t1 = **m_elements.begin();
			TObj const & t2 = **m_elements.rbegin();
			return t1 == t2;
		}
		/**
		 *\~english
		 *\brief		adds an element to the list, at a given index
		 *\param[in]	element	The element to add
		 *\param[in]	index	The index at which the insertion must be done
		 *\~french
		 *\brief		Ajoute un sommet, à l'indice donné
		 *\param[in]	element	L'élément à ajouter
		 *\param[in]	index	L'indice souhaité
		 */
		void addElement( TObjConstRef element
			, uint32_t index = 0xFFFFFFFF )
		{
			if ( index >= m_elements.size() )
			{
				m_elements.push_back( element );
			}
			else
			{
				auto it = m_elements.begin();

				for ( uint32_t i = 0; i < index; i++ )
				{
					it++;
				}

				m_elements.insert( it, element );
			}
		}
		/**
		 *\~english
		 *\return		The number of elements constituting the pattern
		 *\~french
		 *\return		Le nombre d'éléments dans le chemin
		 */
		inline uint32_t getSize()const
		{
			return uint32_t( m_elements.size() );
		}
		/**
		 *\~english
		 *\brief		Accessor to an element
		 *\param[in]	index	the index of the wanted element
		 *\return		Modifiable element reference
		 *\~french
		 *\brief		Accesseur sur les éléments
		 *\param[in]	index	Index de l'élément voulu
		 *\return		Une référence modifiable sur l'élément
		 */
		TObjRef operator[]( uint32_t index )
		{
			CU_Require( index < m_elements.size() );
			auto it = m_elements.begin();

			for ( uint32_t i = 0; i < index; i++ )
			{
				it++;
			}

			return * it;
		}
		/**
		 *\~english
		 *\brief		Constant accessor to an element
		 *\param[in]	index	the index of the wanted element
		 *\return		Constant element reference
		 *\~french
		 *\brief		Accesseur sur les éléments
		 *\param[in]	index	Index de l'élément voulu
		 *\return		Une référence constante sur l'élément
		 */
		TObjConstRef operator[]( uint32_t index )const
		{
			CU_Require( index < m_elements.size() );
			auto it = m_elements.begin();

			for ( uint32_t i = 0; i < index; i++ )
			{
				it++;
			}

			return * it;
		}
		/**
		 *\~english
		 *\brief		Accessor to an element
		 *\param[in]	index	the index of the wanted element
		 *\return		Modifiable element
		 *\~french
		 *\brief		Accesseur sur les éléments
		 *\param[in]	index	Index de l'élément voulu
		 *\return		Une référence modifiable sur l'élément
		 */
		TObjRef getElement( uint32_t index )
		{
			CU_Require( index < m_elements.size() );
			auto it = m_elements.begin();

			for ( uint32_t i = 0; i < index; i++ )
			{
				it++;
			}

			return * it;
		}
		/**
		 *\~english
		 *\brief		Constant accessor to an element
		 *\param[in]	index	the index of the wanted element
		 *\return		Constant element
		 *\~french
		 *\brief		Accesseur sur les éléments
		 *\param[in]	index	Index de l'élément voulu
		 *\return		Une référence constante sur l'élément
		 */
		TObjConstRef getElement( uint32_t index )const
		{
			CU_Require( index < m_elements.size() );
			auto it = m_elements.begin();

			for ( uint32_t i = 0; i < index; i++ )
			{
				it++;
			}

			return * it;
		}

	private:
		//!\~english	The elements.
		//!\~french		Les éléments.
		TObjList m_elements;
	};
}

#endif
