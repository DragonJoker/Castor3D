/*
See LICENSE file in root folder
*/
#ifndef ___CSE_TextBillboardList_H___
#define ___CSE_TextBillboardList_H___

#include <Castor3D/Scene/BillboardList.hpp>

#include <CastorUtils/Data/TextWriter.hpp>

namespace castor
{
	template<>
	class TextWriter< castor3d::BillboardList >
		: public TextWriterT< castor3d::BillboardList >
	{
	public:
		explicit TextWriter( String const & tabs );
		bool operator()( castor3d::BillboardList const & obj
			, castor::StringStream & file )override;
	};
}

#endif
