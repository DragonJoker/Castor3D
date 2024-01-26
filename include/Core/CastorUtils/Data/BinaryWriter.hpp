/*
See LICENSE file in root folder
*/
#ifndef ___CASTOR_BINARY_WRITER_H___
#define ___CASTOR_BINARY_WRITER_H___

#include "CastorUtils/Data/Writer.hpp"
#include "CastorUtils/Data/BinaryFile.hpp"

namespace castor
{
	template< class T >
	class BinaryWriter
		: public FileWriter< T, FileType::eBinary >
	{
	protected:
		using Type = typename FileWriter< T, FileType::eBinary >::Type;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		BinaryWriter() = default;
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		~BinaryWriter()noexcept override = default;
	};
}

#endif
