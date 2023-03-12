/*
See LICENSE file in root folder
*/
#ifndef ___CASTOR_SIZE_H___
#define ___CASTOR_SIZE_H___

#include "CastorUtils/Graphics/GraphicsModule.hpp"

#include "CastorUtils/Math/Coords.hpp"

namespace castor
{
	class Size
		: public Coords< uint32_t, 2 >
	{
	private:
		typedef Coords< uint32_t, 2 > BaseType;

	private:
		union Datas
		{
			struct Mbr
			{
				uint32_t x;
				uint32_t y;
			} size;
			uint32_t buffer[2];
		}	m_data;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		CU_API Size()noexcept;
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	width, height	The dimensions
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	width, height	Les dimensions
		 */
		CU_API Size( uint32_t width, uint32_t height )noexcept;
		/**
		 *\~english
		 *\brief		Copy Constructor
		 *\param[in]	obj	The object to copy
		 *\~french
		 *\brief		Constructeur par copie
		 *\param[in]	obj	L'objet à copier
		 */
		CU_API Size( Size const & obj )noexcept;
		/**
		 *\~english
		 *\brief		Move assignment Constructor
		 *\param[in]	obj	The object to move
		 *\~french
		 *\brief		Constructeur par déplacement
		 *\param[in]	obj	L'objet à déplacer
		 */
		CU_API Size( Size && obj )noexcept;
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		CU_API ~Size()noexcept;
		/**
		 *\~english
		 *\brief		Copy assignment operator
		 *\param[in]	obj	The object to copy
		 *\return		A reference to this object
		 *\~french
		 *\brief		Opérateur d'affectation par copie
		 *\param[in]	obj	L'objet à copier
		 *\return		Une référence sur cet objet
		 */
		CU_API Size & operator=( Size const & obj )noexcept;
		/**
		 *\~english
		 *\brief		Move assignment operator
		 *\param[in]	obj	The object to move
		 *\return		A reference to this object
		 *\~french
		 *\brief		Opérateur d'affectation par déplacement
		 *\param[in]	obj	L'objet à déplacer
		 *\return		Une référence sur cet objet
		 */
		CU_API Size & operator=( Size && obj )noexcept;
		/**
		 *\~english
		 *\brief		sets the size values
		 *\param[in]	width, height	The dimensions
		 *\~french
		 *\brief		Définit la taille
		 *\param[in]	width, height	Les dimensions
		 */
		CU_API void set( uint32_t width, uint32_t height )noexcept;
		/**
		 *\~english
		 *\brief		Retrieves the width
		 *\return		The width
		 *\~french
		 *\brief		Récupère la largeur
		 *\return		La largeur
		 */
		inline uint32_t getWidth()const noexcept
		{
			return m_data.size.x;
		}
		/**
		 *\~english
		 *\brief		Retrieves the width
		 *\return		The width
		 *\~french
		 *\brief		Récupère la largeur
		 *\return		La largeur
		 */
		inline uint32_t & getWidth()noexcept
		{
			return m_data.size.x;
		}
		/**
		 *\~english
		 *\brief		Retrieves the height
		 *\return		The height
		 *\~french
		 *\brief		Récupère la hauteur
		 *\return		La hauteur
		 */
		inline uint32_t getHeight()const noexcept
		{
			return m_data.size.y;
		}
		/**
		 *\~english
		 *\brief		Retrieves the height
		 *\return		The height
		 *\~french
		 *\brief		Récupère la hauteur
		 *\return		La hauteur
		 */
		inline uint32_t & getHeight()noexcept
		{
			return m_data.size.y;
		}
		/**
		 *\~english
		 *\brief		Modifies the size
		 *\remarks		If width+cx < 0 (or height+cy < 0) then width=0 (respectively height=0)
		 *\param[in]	cx, cy	The size modifications
		 *\~french
		 *\brief		Modifie la taille
		 *\remarks		Si width+cx < 0 (ou height+cy < 0) alors width=0 (respectivement height=0)
		 *\param[in]	cx, cy	Les valeurs de modification
		 */
		CU_API void grow( int32_t cx, int32_t cy )noexcept;

		inline Size & operator<<=( uint32_t rhs )noexcept
		{
			m_data.size.x <<= rhs;
			m_data.size.y <<= rhs;
			return *this;
		}

		inline Size & operator>>=( uint32_t rhs )noexcept
		{
			m_data.size.x >>= rhs;
			m_data.size.y >>= rhs;
			return *this;
		}

		auto operator->()const noexcept
		{
			return &m_data.size;
		}

		auto operator->()noexcept
		{
			return &m_data.size;
		}

		using BaseType::ptr;
		using BaseType::constPtr;
	};
	/**
	 *\~english
	 *\brief		Equality operator
	 *\param[in]	a, b	The sizes to compare
	 *\return		\p true if sizes have same dimensions
	 *\~french
	 *\brief		Opérateur d'égalité
	 *\param[in]	a, b	Les tailles à comparer
	 *\return		\p true si les tailles ont les mêmes dimensions
	 */
	CU_API bool operator==( Size const & a, Size const & b )noexcept;
	/**
	 *\~english
	 *\brief		Difference operator
	 *\param[in]	a, b	The sizes to compare
	 *\return		\p false if sizes have same dimensions
	 *\~french
	 *\brief		Opérateur de différence
	 *\param[in]	a, b	Les tailles à comparer
	 *\return		\p false si les tailles ont les mêmes dimensions
	 */
	CU_API bool operator!=( Size const & a, Size const & b )noexcept;

	inline Size operator<<( Size const & lhs, uint32_t rhs )noexcept
	{
		Size tmp{ lhs };
		tmp <<= rhs;
		return tmp;
	}

	inline Size operator>>( Size const & lhs, uint32_t rhs )noexcept
	{
		Size tmp{ lhs };
		tmp >>= rhs;
		return tmp;
	}
}

#endif
