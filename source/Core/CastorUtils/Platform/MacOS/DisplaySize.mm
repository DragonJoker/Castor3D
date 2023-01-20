#pragma clang diagnostic ignored "-Wimport-preprocessor-directive-pedantic"
#pragma clang diagnostic ignored "-Wmissing-prototypes"
#pragma GCC diagnostic ignored "-Wmissing-declarations"

#import <CoreGraphics/CGDisplayConfiguration.h>

void getDisplaySize( uint32_t * ww, uint32_t * hh )
{
	uint32_t id = CGMainDisplayID();
	size_t w = CGDisplayPixelsWide( id );
	size_t h = CGDisplayPixelsHigh( id );
	*ww = uint32_t( w );
	*hh = uint32_t( h );
}
