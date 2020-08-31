#import <AppKit/AppKit.h>
#import <QuartzCore/CAMetalLayer.h>

void makeViewMetalCompatible(void* handle)
{
    NSView* view = (NSView*)handle;
    assert([view isKindOfClass:[NSView class]]);

    if (![view.layer isKindOfClass:[CAMetalLayer class]])
    {
        [view setLayer:[CAMetalLayer layer]];
        [view setWantsLayer:YES];
    }
}