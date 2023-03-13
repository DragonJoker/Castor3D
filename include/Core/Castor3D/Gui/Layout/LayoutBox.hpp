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

		/**
		*\brief
		*	Constructor
		*/
		C3D_API explicit LayoutBox( LayoutControl & container );

		void setHorizontal( bool v )
		{
			m_horizontal = v;
			markDirty();
		}

		//@}

	private:
		/**
		*\copydoc	castor3d::Layout::doUpdate
		*/
		void doUpdate()override;

		uint32_t doComputeSeparator( uint32_t component );
		std::pair< int32_t, uint32_t > doGetPosSize( LayoutCtrl const & control
			, uint32_t limit
			, uint32_t component );

	private:
		bool m_horizontal{};
	};
}

#endif
