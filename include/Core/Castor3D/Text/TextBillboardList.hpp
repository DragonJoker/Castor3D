/*
See LICENSE file in root folder
*/
#ifndef ___C3D_TextBillboardList_H___
#define ___C3D_TextBillboardList_H___

#include "Castor3D/Scene/BillboardList.hpp"

#include <CastorUtils/Data/TextWriter.hpp>

namespace castor
{
	template<>
	class TextWriter< castor3d::BillboardList >
		: public TextWriterT< castor3d::BillboardList >
	{
	public:
		C3D_API explicit TextWriter( String const & tabs );
		C3D_API bool operator()( castor3d::BillboardList const & obj
			, castor::StringStream & file )override;
	};
}

#endif
