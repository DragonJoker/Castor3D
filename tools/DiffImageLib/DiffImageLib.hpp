/* See LICENSE file in root folder */
#ifndef ___DIL_DiffImage_HPP___
#define ___DIL_DiffImage_HPP___

#include <CastorUtils/Data/Path.hpp>
#include <CastorUtils/Graphics/Image.hpp>
#include <CastorUtils/Graphics/ImageLoader.hpp>
#include <CastorUtils/Graphics/ImageWriter.hpp>

namespace diffimg
{
	struct Options
	{
		castor::PathArray outputs;
		castor::Path input;
		double acceptableThreshold = 0.1;
		double negligibleThreshold = 0.001;
	};

	enum class DiffResult
	{
		eUnprocessed,
		eNegligible,
		eAcceptable,
		eUnacceptable,
		eCount,
	};

	struct Config
	{
		Config( Options const & options );

		castor::ImageUPtr reference;
		std::array< castor::Path, size_t( DiffResult::eCount ) > dirs;
		castor::ImageLoader loader;
		castor::ImageWriter writer;
	};

	DiffResult compareImages( Options const & options
		, Config const & config
		, castor::Path const & compFile );
}

#endif
