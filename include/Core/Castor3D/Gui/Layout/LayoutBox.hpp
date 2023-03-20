/*
See LICENSE file in root folder
*/
#ifndef ___C3D_LayoutBox_H___
#define ___C3D_LayoutBox_H___

#include "Castor3D/Gui/Layout/Layout.hpp"

namespace castor3d
{
	class LayoutBox
		: public Layout
	{
	public:
		/**@name General */
		//@{

		/** Constructor
		*\param[in] container
		*	The control affected by this layout.
		*/
		C3D_API explicit LayoutBox( LayoutControl & container );

		/** Sets the layout direction.
		*\param[in]	horizontal	\p true for an horizontal layout.
		*/
		void setHorizontal( bool v )noexcept
		{
			m_horizontal = v;
			markDirty();
		}

		bool isHorizontal()const noexcept
		{
			return m_horizontal;
		}

		//@}

	private:
		/** @copydoc	castor3d::Layout::doUpdate
		*/
		void doUpdate()override;

		uint32_t doComputeSeparator( uint32_t component );
		std::pair< int32_t, uint32_t > doGetFixedPosSize( Item const & item
			, uint32_t limit
			, uint32_t component );

	private:
		bool m_horizontal{};
	};
}

#endif
