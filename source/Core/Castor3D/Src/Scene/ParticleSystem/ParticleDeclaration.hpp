/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ParticleDeclaration_H___
#define ___C3D_ParticleDeclaration_H___

#include "ParticleElementDeclaration.hpp"

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.6.1.0
	\date		03/01/2011
	\~english
	\brief		Description of all elements in a vertex buffer
	\~french
	\brief		Description de tous les éléments dans un tampon de sommets
	*/
	class ParticleDeclaration
	{
	public:
		CU_DeclareVector( ParticleElementDeclaration, ParticleElementDeclaration );
		using iterator = ParticleElementDeclarationArrayIt;
		using const_iterator = ParticleElementDeclarationArrayConstIt;

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\~french
		 *\brief		Constructeur.
		 */
		C3D_API ParticleDeclaration();
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	elements	The elements array
		 *\param[in]	count		Elements array size
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	elements	Tableau d'éléments
		 *\param[in]	count		Taille du tableau d'éléments
		 */
		C3D_API ParticleDeclaration( ParticleElementDeclaration const * elements
			, uint32_t count );
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	elements	The elements array
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	elements	Tableau des éléments
		 */
		template< uint32_t N >
		inline explicit ParticleDeclaration( ParticleElementDeclaration const( & elements )[N] )
			: ParticleDeclaration( elements, N )
		{
		}
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	elements	The elements array
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	elements	Tableau d'éléments
		 */
		inline explicit ParticleDeclaration( std::vector< ParticleElementDeclaration > const & elements )
			: ParticleDeclaration( elements.data(), uint32_t( elements.size() ) )
		{
		}
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~ParticleDeclaration();
		/**
		 *\~english
		 *\return		The elements count.
		 *\~french
		 *\return		Le compte des éléments.
		 */
		inline uint32_t size()const
		{
			return uint32_t( m_elements.size() );
		}
		/**
		 *\~english
		 *\return		The byte size of the declaration.
		 *\~french
		 *\return		La taille en octets de la déclaration.
		 */
		inline uint32_t stride()const
		{
			return m_stride;
		}
		/**
		 *\~english
		 *\return		An iterator to the beginning of the elements.
		 *\~french
		 *\return		Un itérateur sur le début des éléments.
		 */
		inline iterator begin()
		{
			return m_elements.begin();
		}
		/**
		 *\~english
		 *\return		An iterator to the beginning of the elements.
		 *\~french
		 *\return		Un itérateur sur le début des éléments.
		 */
		inline const_iterator begin()const
		{
			return m_elements.begin();
		}
		/**
		 *\~english
		 *\return		An iterator to the end of the elements.
		 *\~french
		 *\return		Un itérateur sur la fin des éléments.
		 */
		inline iterator end()
		{
			return m_elements.end();
		}
		/**
		 *\~english
		 *\return		An iterator to the end of the elements.
		 *\~french
		 *\return		Un itérateur sur la fin des éléments.
		 */
		inline const_iterator end()const
		{
			return m_elements.end();
		}
		/**
		 *\~english
		 *\brief		Adds an element to the end of the list.
		 *\param[in]	element	The element.
		 *\~french
		 *\brief		Ajoute un élément à la fin de la liste.
		 *\param[in]	element	L'élément.
		 */
		inline void push_back( ParticleElementDeclaration const & element )
		{
			m_elements.push_back( element );
			m_stride += getSize( m_elements.back().m_dataType );
		}
		/**
		 *\~english
		 *\brief		Adds an element to the end of the list.
		 *\param[in]	params	The element construction parameters.
		 *\~french
		 *\brief		Ajoute un élément à la fin de la liste.
		 *\param[in]	params	Les paramètres de construction de l'élément.
		 */
		template< typename ... Params >
		inline void emplace_back( Params const & ... params )
		{
			m_elements.emplace_back( std::forward< Params >( params )... );
			m_stride += getSize( m_elements.back().m_dataType );
		}

	private:
		uint32_t getSize( ParticleFormat format )
		{
			switch ( format )
			{
			case castor3d::ParticleFormat::eInt:
			case castor3d::ParticleFormat::eUInt:
			case castor3d::ParticleFormat::eFloat:
				return 4u;

			case castor3d::ParticleFormat::eVec2i:
			case castor3d::ParticleFormat::eVec2ui:
			case castor3d::ParticleFormat::eVec2f:
				return 8u;

			case castor3d::ParticleFormat::eVec3i:
			case castor3d::ParticleFormat::eVec3ui:
			case castor3d::ParticleFormat::eVec3f:
				return 12u;

			case castor3d::ParticleFormat::eVec4i:
			case castor3d::ParticleFormat::eVec4ui:
			case castor3d::ParticleFormat::eVec4f:
			case castor3d::ParticleFormat::eMat2f:
				return 16u;

			case castor3d::ParticleFormat::eMat3f:
				return 36u;

			case castor3d::ParticleFormat::eMat4f:
				return 64u;

			default:
				assert( false );
				return 16u;
			}
		}

	private:
		ParticleElementDeclarationArray m_elements;
		uint32_t m_stride;
	};
	/**
	 *\~english
	 *\brief		Equality operator.
	 *\param[in]	p_lhs, p_rhs	The 2 objects to compare.
	 *\~french
	 *\brief		Opérateur d'égalité.
	 *\param[in]	p_lhs, p_rhs	Les 2 objets à comparer
	 */
	C3D_API bool operator==( ParticleDeclaration const & lhs, ParticleDeclaration const & rhs );
	/**
	 *\~english
	 *\brief		Equality operator.
	 *\param[in]	p_lhs, p_rhs	The 2 objects to compare.
	 *\~french
	 *\brief		Opérateur d'égalité.
	 *\param[in]	p_lhs, p_rhs	Les 2 objets à comparer
	 */
	C3D_API bool operator!=( ParticleDeclaration const & lhs, ParticleDeclaration const & rhs );
}

#endif
