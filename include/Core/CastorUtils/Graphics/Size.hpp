/*
See LICENSE file in root folder
*/
#ifndef ___CASTOR_SIZE_H___
#define ___CASTOR_SIZE_H___

#include "CastorUtils/Graphics/GraphicsModule.hpp"

#include "CastorUtils/Design/DataHolder.hpp"
#include "CastorUtils/Math/PointView.hpp"

namespace c3d
{
	union SizeData
	{
		struct Mbr
		{
			uint32_t x;
			uint32_t y;
		};
		Mbr size;
		Array< uint32_t, 2u > buffer;
	};

	class Size
		: private DataHolderT< SizeData >
		, public PointView< uint32_t, 2 >
	{
	private:
		using BaseType = PointView< uint32_t, 2 >;

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
		CU_API Size( Size const & obj );
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
		CU_INL_API ~Size()noexcept = default;
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
		uint32_t getWidth()const noexcept
		{
			return getData().size.x;
		}
		/**
		 *\~english
		 *\brief		Retrieves the width
		 *\return		The width
		 *\~french
		 *\brief		Récupère la largeur
		 *\return		La largeur
		 */
		uint32_t & getWidth()noexcept
		{
			return getData().size.x;
		}
		/**
		 *\~english
		 *\brief		Retrieves the height
		 *\return		The height
		 *\~french
		 *\brief		Récupère la hauteur
		 *\return		La hauteur
		 */
		uint32_t getHeight()const noexcept
		{
			return getData().size.y;
		}
		/**
		 *\~english
		 *\brief		Retrieves the height
		 *\return		The height
		 *\~french
		 *\brief		Récupère la hauteur
		 *\return		La hauteur
		 */
		uint32_t & getHeight()noexcept
		{
			return getData().size.y;
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

		Size & operator<<=( uint32_t rhs )noexcept
		{
			getData().size.x <<= rhs;
			getData().size.y <<= rhs;
			return *this;
		}

		Size & operator>>=( uint32_t rhs )noexcept
		{
			getData().size.x >>= rhs;
			getData().size.y >>= rhs;
			return *this;
		}

		auto operator->()const noexcept
		{
			return &getData().size;
		}

		auto operator->()noexcept
		{
			return &getData().size;
		}

		using BaseType::ptr;
		using BaseType::constPtr;

	private:
		friend bool operator==( Size const & lhs, Size const & rhs )noexcept
		{
			return lhs.getWidth() == rhs.getWidth() && lhs.getHeight() == rhs.getHeight();
		}

		friend Size operator<<( Size const & lhs, uint32_t rhs )noexcept
		{
			Size tmp{ lhs };
			tmp <<= rhs;
			return tmp;
		}

		friend Size operator>>( Size const & lhs, uint32_t rhs )noexcept
		{
			Size tmp{ lhs };
			tmp >>= rhs;
			return tmp;
		}
	};
}

#endif
