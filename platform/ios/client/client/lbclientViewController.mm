#import "lbclientViewController.h"

#import "iOSApp.h"

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

@interface lbclientViewController () {
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