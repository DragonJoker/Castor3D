/*
See LICENSE file in root folder
*/
#ifndef ___CSE_TextBillboardList_H___
#define ___CSE_TextBillboardList_H___

#include <Castor3D/Scene/BillboardList.hpp>

#include <CastorUtils/Data/TextWriter.hpp>

namespace c3d
{
	template<>
	class TextWriter< BillboardList >
		: public TextWriterT< BillboardList >
	{
	public:
		explicit TextWriter( String const & tabs );
		bool operator()( BillboardList const & obj
			, StringStream & file )override;
	};
}

#endif
