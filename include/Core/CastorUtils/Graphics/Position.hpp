/*
See LICENSE file in root folder
*/
#ifndef ___CASTOR_POSITION_H___
#define ___CASTOR_POSITION_H___

#include "CastorUtils/Graphics/GraphicsModule.hpp"

#include "CastorUtils/Design/DataHolder.hpp"
#include "CastorUtils/Math/Coords.hpp"

namespace castor
{
	union PositionData
	{
		struct Pos
		{
			int32_t x;
			int32_t y;
		};
		Pos position;
		std::array< int32_t, 2u > buffer;
	};

	class Position
		: private DataHolderT< PositionData >
		, public Coords< int32_t, 2 >
	{
	private:
		using BaseType = Coords< int32_t, 2 >;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	x, y	The position
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	x, y	La position
		 */
		CU_API Position( int32_t x = 0, int32_t y = 0 );
		/**
		 *\~english
		 *\brief		Copy Constructor
		 *\param[in]	obj	The object to copy
		 *\~french
		 *\brief		Constructeur par copie
		 *\param[in]	obj	L'objet à copier
		 */
		CU_API Position( Position const & obj );
		/**
		 *\~english
		 *\brief		Move assignment Constructor
		 *\param[in]	obj	The object to move
		 *\~french
		 *\brief		Constructeur par déplacement
		 *\param[in]	obj	L'objet à déplacer
		 */
		CU_API Position( Position && obj )noexcept;
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		CU_API ~Position()noexcept = default;
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
		CU_API Position & operator=( Position const & obj );
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
		CU_API Position & operator=( Position && obj )noexcept;
		/**
		 *\~english
		 *\brief		sets the position values
		 *\param[in]	x, y	The position
		 *\~french
		 *\brief		Définit la position
		 *\param[in]	x, y	La position
		 */
		CU_API void set( int32_t x, int32_t y );
		/**
		 *\~english
		 *\brief		Offsets the position values
		 *\param[in]	x, y	The position offsets
		 *\~french
		 *\brief		Déplace la position
		 *\param[in]	x, y	Les valeurs de déplacement
		 */
		CU_API void offset( int32_t x, int32_t y );
		/**
		 *\~english
		 *\brief		Retrieves the left coordinate
		 *\return		The rectangle's left coordinate
		 *\~french
		 *\brief		Récupère la coordonnée gauche
		 *\return		La coordonnée gauche du rectangle
		 */
		int32_t y()const
		{
			return getData().position.y;
		}
		/**
		 *\~english
		 *\brief		Retrieves the left coordinate
		 *\return		The rectangle's left coordinate
		 *\~french
		 *\brief		Récupère la coordonnée gauche
		 *\return		La coordonnée gauche du rectangle
		 */
		int32_t & y()
		{
			return getData().position.y;
		}
		/**
		 *\~english
		 *\brief		Retrieves the right coordinate
		 *\return		The rectangle's right coordinate
		 *\~french
		 *\brief		Récupère la coordonnée droite
		 *\return		La coordonnée droite du rectangle
		 */
		int32_t x()const
		{
			return getData().position.x;
		}
		/**
		 *\~english
		 *\brief		Retrieves the right coordinate
		 *\return		The rectangle's right coordinate
		 *\~french
		 *\brief		Récupère la coordonnée droite
		 *\return		La coordonnée droite du rectangle
		 */
		int32_t & x()
		{
			return getData().position.x;
		}

		using BaseType::ptr;
		using BaseType::constPtr;
	};
	/**
	 *\~english
	 *\brief		Equality operator
	 *\param[in]	a, b	The positions to compare
	 *\return		\p true if positions have same coordinates
	 *\~french
	 *\brief		Opérateur d'égalité
	 *\param[in]	a, b	Les positions à comparer
	 *\return		\p true si les positions ont les mêmes coordonnées
	 */
	CU_API bool operator==( Position const & a, Position const & b );
	/**
	 *\~english
	 *\brief		Difference operator
	 *\param[in]	a, b	The positions to compare
	 *\return		\p false if positions have same coordinates
	 *\~french
	 *\brief		Opérateur de différence
	 *\param[in]	a, b	Les positions à comparer
	 *\return		\p false si les positions ont les mêmes coordonnées
	 */
	CU_API bool operator!=( Position const & a, Position const & b );
}

#endif
