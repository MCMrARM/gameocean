#import "lbclientViewController.h"

#import "iOSApp.h"
#import "TouchHandler.h"

#define MAX_TOUCHES 10

@interface lbclientViewController () {
    UITouch* touches[MAX_TOUCHES];
}

@property (strong, nonatomic) EAGLContext *context;
@property iOSApp *app;
@property int lastWidth;
@property int lastHeight;

- (void)setupGL;
- (void)tearDownGL;
@end

@implementation lbclientViewController

- (void)viewDidLoad
{
    [super viewDidLoad];
    
    self.lastWidth = 0;
    self.lastHeight = 0;
    
    self.context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
    
    if (!self.context) {
        NSLog(@"Failed to create ES context");
    }
    
    GLKView *view = (GLKView *)self.view;
    view.context = self.context;
    view.drawableDepthFormat = GLKViewDrawableDepthFormat24;
    
    self.app = new iOSApp();
    self.app->init();
    self.app->pixelSize = [UIScreen mainScreen].scale * 2;
    
    for (int i = 0; i < MAX_TOUCHES; i++) {
        touches[i] = null;
    }
    
    [self setupGL];
}

- (void)dealloc
{
    [self tearDownGL];
    
    if ([EAGLContext currentContext] == self.context) {
        [EAGLContext setCurrentContext:nil];
    }
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    
    if ([self isViewLoaded] && ([[self view] window] == nil)) {
        self.view = nil;
        
        [self tearDownGL];
        
        if ([EAGLContext currentContext] == self.context) {
            [EAGLContext setCurrentContext:nil];
        }
        self.context = nil;
    }
}

- (void)setupGL
{
    [EAGLContext setCurrentContext:self.context];
    
    self.app->initOpenGL();
}

- (void)tearDownGL
{
    [EAGLContext setCurrentContext:self.context];
    
    //
}

- (void)touchesBegan:(NSSet *)utouches withEvent:(UIEvent *)event {
    int scale = [UIScreen mainScreen].scale;
    for (UITouch* touch in utouches) {
        for (int i = 0; i < MAX_TOUCHES; i++) {
            if (touches[i] == null) {
                touches[i] = touch;
                CGPoint loc = [touch locationInView:self.view];
                TouchHandler::press(i, loc.x * scale, loc.y * scale);
                Logger::main->trace("Touch", "Adding touch: id %i; pos: %f %f", i, loc.x, loc.y);
                break;
            }
        }
    }
}

- (void)touchesMoved:(NSSet *)utouches withEvent:(UIEvent *)event {
    int scale = [UIScreen mainScreen].scale;
    for (UITouch* touch in utouches) {
        for (int i = 0; i < MAX_TOUCHES; i++) {
            if (touches[i] == touch) {
                CGPoint loc = [touch locationInView:self.view];
                TouchHandler::move(i, loc.x * scale, loc.y * scale);
                break;
            }
        }
    }
}

- (void)touchesEnded:(NSSet *)utouches withEvent:(UIEvent *)event {
    int scale = [UIScreen mainScreen].scale;
    for (UITouch* touch in utouches) {
        for (int i = 0; i < MAX_TOUCHES; i++) {
            if (touches[i] == touch) {
                touches[i] = null;
                CGPoint loc = [touch locationInView:self.view];
                TouchHandler::release(i, loc.x * scale, loc.y * scale);
                break;
            }
        }
    }
}

- (void)touchesCancelled:(NSSet *)utouches withEvent:(UIEvent *)event {
    [self touchesCancelled:utouches withEvent:event];
}

#pragma mark - GLKView and GLKViewController delegate methods

- (void)update
{
    //
}

- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect
{
    int w = rect.size.width * [UIScreen mainScreen].scale;
    int h = rect.size.height * [UIScreen mainScreen].scale;
    if(self.lastWidth != w || self.lastHeight != h) {
        self.app->resize(w, h);
        self.lastWidth = w;
        self.lastHeight = h;
    }
    
    self.app->render();
}

@end