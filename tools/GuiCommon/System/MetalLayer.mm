#import <AppKit/AppKit.h>
#import <QuartzCore/CAMetalLayer.h>

#pragma clang diagnostic ignored "-Wmissing-prototypes"
#pragma GCC diagnostic ignored "-Wmissing-declarations"

void makeViewMetalCompatible( void * handle )
{
    NSView* view = reinterpret_cast< NSView * >( handle );
    assert( [view isKindOfClass:[NSView class]] );

    if ( ![view.layer isKindOfClass:[CAMetalLayer class]] )
    {
        [view setLayer:[CAMetalLayer layer]];
        [view setWantsLayer:YES];
    }
}
