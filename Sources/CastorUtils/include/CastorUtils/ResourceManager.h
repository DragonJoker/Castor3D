#ifndef ___CSU_ResourceManager___
#define ___CSU_ResourceManager___

#include "Module_Utils.h"
#include "Resource.h"
#include "Manager.h"
#include "AutoSingleton.h"

/*!
Manages all the images loaded in the engine.
\author Sylvain DOREMUS
\date 14/02/2010
*/
class ImageManager : public General::Templates::Manager<Image>, public General::Theory::AutoSingleton<ImageManager>
{
public:
	/**
	 * Constructor
	 */
	ImageManager(){}
	/**
	 * Destructor
	 */
	~ImageManager(){}
	/**
	* Creates an image with the given path and the given anem
	*@param p_name : [in] The resource name
	*@param p_path : [in] The image path
	*@return The created image
	*/
	Image * CreateImage( const String & p_name, const String & p_path);
};

#endif