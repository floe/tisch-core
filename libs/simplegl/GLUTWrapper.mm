/*************************************************************************\
*    Part of the TISCH framework - see http://tisch.sourceforge.net/      *
*   Copyright (c) 2006 - 2011 by Florian Echtler <floe@butterbrot.org>    *
*   Licensed under GNU Lesser General Public License (LGPL) 3 or later    *
\*************************************************************************/

#include "GLUTWrapper.h"
#include <iostream>
#include <iomanip>

#import <UIKit/UIKit.h>
#import <OpenGLES/EAGL.h>
#import <QuartzCore/QuartzCore.h>
#import <OpenGLES/EAGLDrawable.h>


// GLU functions

static void gluMultMatrixVecd( const GLdouble matrix[16], const GLdouble in[4], GLdouble out[4] ) {
	for (int i = 0; i < 4; i++) {
		out[i] = 
			in[0] * matrix[0*4+i] +
			in[1] * matrix[1*4+i] +
			in[2] * matrix[2*4+i] +
			in[3] * matrix[3*4+i];
	}
}

GLint gluProject(
	GLdouble objx, GLdouble objy, GLdouble objz, 
	const GLdouble modelMatrix[16], 
	const GLdouble projMatrix[16],
	const GLint viewport[4],
	GLdouble *winx, GLdouble *winy, GLdouble *winz
) {

	GLdouble in[4];
	GLdouble out[4];

	in[0] = objx;
	in[1] = objy;
	in[2] = objz;
	in[3] = 1.0;

	gluMultMatrixVecd(modelMatrix, in, out);
	gluMultMatrixVecd(projMatrix, out, in);
	if (in[3] == 0.0) return GL_FALSE;

	in[0] /= in[3];
	in[1] /= in[3];
	in[2] /= in[3];

	// map x, y and z to range 0-1 
	in[0] = in[0] * 0.5 + 0.5;
	in[1] = in[1] * 0.5 + 0.5;
	in[2] = in[2] * 0.5 + 0.5;

	// map x,y to viewport
	in[0] = in[0] * viewport[2] + viewport[0];
	in[1] = in[1] * viewport[3] + viewport[1];

	*winx = in[0];
	*winy = in[1];
	*winz = in[2];

	return GL_TRUE;
}


// GLUT function pointers

void (*g_idlefunc)(void) = 0;
void (*g_displayfunc)(void) = 0;
void (*g_reshapefunc)(int,int) = 0;

void (*g_extentryfunc)(int,int) = 0;
void (*g_extmotionfunc)(int,int,int) = 0;
void (*g_extbuttonfunc)(int,int,int,int,int) = 0;
void (*g_extpassivefunc)(int,int,int) = 0;


int g_redisplay = 0;
EAGLContext* g_context = 0;
NSAutoreleasePool* g_pool = 0;


// UIView-derived class for wrapping an EAGLContext

@interface GLESView: UIView {
	
	@private

		GLint width, height;
		
		GLuint renderbuffer, framebuffer, depthbuffer;
		
		NSTimer* idletimer;
}

- (id)initGLES;

- (BOOL)createFramebuffer;
- (void)destroyFramebuffer;

- (void)startIdleLoop;
- (void)stopIdleLoop;
- (void)drawView;

@end


@implementation GLESView

+ (Class)layerClass {
	return [CAEAGLLayer class];
}

-(id)initWithFrame:(CGRect)frame
{
	if ((self = [super initWithFrame:frame]))
		self = [self initGLES];
	return self;
}

- (id)initWithCoder:(NSCoder*)coder
{
	if ((self = [super initWithCoder:coder]))
		self = [self initGLES];
	return self;
}

- (id)initGLES {

	[self setMultipleTouchEnabled:YES];

	CAEAGLLayer* eaglLayer = (CAEAGLLayer*)self.layer;
	
	eaglLayer.opaque = YES;
	eaglLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:
		[NSNumber numberWithBool:FALSE],
		kEAGLDrawablePropertyRetainedBacking,
		kEAGLColorFormatRGBA8,
		kEAGLDrawablePropertyColorFormat,
		nil
	];

	if (!g_context || ![EAGLContext setCurrentContext:g_context] || ![self createFramebuffer]) {
		[self release];
		return nil;
	}

	g_reshapefunc( width, height );

	return self;
}


- (void)drawView {
	if ([EAGLContext currentContext] != g_context)
		[EAGLContext setCurrentContext:g_context];
	
	glBindFramebufferOES( GL_FRAMEBUFFER_OES, framebuffer );

	if (g_idlefunc) g_idlefunc();
	if (g_displayfunc && g_redisplay) {
		g_displayfunc();
		g_redisplay = 0;
	}
	
	glBindRenderbufferOES( GL_RENDERBUFFER_OES, renderbuffer );
	[g_context presentRenderbuffer:GL_RENDERBUFFER_OES];

	[self startIdleLoop];
}

- (void)layoutSubviews {
	[EAGLContext setCurrentContext:g_context];
	[self destroyFramebuffer];
	[self createFramebuffer];
	[self drawView];
}

- (BOOL)createFramebuffer {
	
	glGenFramebuffersOES(  1, &framebuffer  );
	glGenRenderbuffersOES( 1, &renderbuffer );
	
	glBindFramebufferOES(  GL_FRAMEBUFFER_OES,  framebuffer  );
	glBindRenderbufferOES( GL_RENDERBUFFER_OES, renderbuffer );

	[g_context renderbufferStorage:GL_RENDERBUFFER_OES fromDrawable:(id<EAGLDrawable>)self.layer];
	glFramebufferRenderbufferOES( GL_FRAMEBUFFER_OES, GL_COLOR_ATTACHMENT0_OES, GL_RENDERBUFFER_OES, renderbuffer );
	
	glGetRenderbufferParameterivOES( GL_RENDERBUFFER_OES, GL_RENDERBUFFER_WIDTH_OES,  &width  );
	glGetRenderbufferParameterivOES( GL_RENDERBUFFER_OES, GL_RENDERBUFFER_HEIGHT_OES, &height );
	
	glGenRenderbuffersOES( 1, &depthbuffer );
	glBindRenderbufferOES( GL_RENDERBUFFER_OES, depthbuffer );
	glRenderbufferStorageOES( GL_RENDERBUFFER_OES, GL_DEPTH_COMPONENT16_OES, width, height );
	glFramebufferRenderbufferOES( GL_FRAMEBUFFER_OES, GL_DEPTH_ATTACHMENT_OES, GL_RENDERBUFFER_OES, depthbuffer );

	if (glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES) != GL_FRAMEBUFFER_COMPLETE_OES) {
		NSLog( @"failed to make complete framebuffer object %x", glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES) );
		return NO;
	}
	
	return YES;
}

- (void)destroyFramebuffer {
	glDeleteFramebuffersOES(  1, &framebuffer  ); framebuffer  = 0;
	glDeleteRenderbuffersOES( 1, &renderbuffer ); renderbuffer = 0;
	glDeleteRenderbuffersOES( 1, &depthbuffer  ); depthbuffer  = 0;
}


- (void)startIdleLoop {
	// cap at 30 Hz, otherwise input events will starve
	NSTimeInterval animationInterval = 1.0 / 30.0;
	idletimer = [NSTimer scheduledTimerWithTimeInterval:animationInterval target:self selector:@selector(drawView) userInfo:nil repeats:NO];
}

- (void)stopIdleLoop {
	idletimer = nil;
}


- (void)touchesBegan:(NSSet*)touches withEvent:(UIEvent*)event {
	for (UITouch* touch in touches) {
		CGPoint foo = [touch locationInView:self];
		int id = (int)touch;
		if (g_extentryfunc)  g_extentryfunc ( id, GLUT_ENTERED );
		if (g_extbuttonfunc) g_extbuttonfunc( id, foo.x, foo.y, 0, GLUT_DOWN );
	}	
}

- (void)touchesMoved:(NSSet*)touches withEvent:(UIEvent*)event {  
	for (UITouch* touch in touches) {
	 	CGPoint foo = [touch locationInView:self];
		int id = (int)touch;
		if (g_extmotionfunc) g_extmotionfunc( id, foo.x, foo.y );
	}
}

- (void)touchesEnded:(NSSet*)touches withEvent:(UIEvent*)event {
	for (UITouch* touch in touches) {
		CGPoint foo = [touch locationInView:self];
		int id = (int)touch;
		if (g_extbuttonfunc) g_extbuttonfunc( id, foo.x, foo.y, 0, GLUT_UP );
		if (g_extentryfunc)  g_extentryfunc ( id, GLUT_LEFT );
	}
}


- (void)dealloc {

	[self stopIdleLoop];

	if ([EAGLContext currentContext] == g_context)
		[EAGLContext setCurrentContext:nil];

	[g_context release];	
	[super dealloc];
}

@end



@interface SimpleCVAppDelegate: NSObject <UIApplicationDelegate> 
@end

@implementation SimpleCVAppDelegate

- (void)applicationDidFinishLaunching:(UIApplication*)application {

	[application setStatusBarHidden:YES];
	CGRect rect = [[UIScreen mainScreen] bounds];

	UIWindow* window = [[UIWindow alloc] initWithFrame:rect];
	GLESView* view = [[GLESView alloc] initWithFrame:rect];

	[window addSubview:view];
	[view startIdleLoop];
	[view release];
	[window makeKeyAndVisible];
}

- (void)applicationWillTerminate:(UIApplication*)application {
}

- (void)dealloc {
	[super dealloc];
}

@end




void glutInitDisplayMode( unsigned int mode ) { 
	if (!g_pool) g_pool = [[NSAutoreleasePool alloc] init];
	if (!g_context) g_context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES1];
	[EAGLContext setCurrentContext:g_context];
}

void glutInitWindowSize( int width, int height ) { }
int glutCreateWindow( const char *name ) { return 1; }

void glutIdleFunc(void (*func)(void)) { g_idlefunc = func; }
void glutDisplayFunc(void (*func)(void)) { g_displayfunc = func; }
void glutReshapeFunc(void (*func)(int width, int height)) { g_reshapefunc = func; }

void glutSpecialFunc(void (*func)(int key, int x, int y)) { }
void glutKeyboardFunc(void (*func)(unsigned char key, int x, int y)) { }

void glutEntryFunc(void (*func)(int state)) { }
void glutMouseFunc(void (*func)(int button, int state, int x, int y)) { }
void glutMotionFunc(void (*func)(int x, int y)) { }
void glutPassiveMotionFunc(void (*func)(int x, int y)) { }

void glutMultiEntryFunc( void (*func)(int num, int state) ) { g_extentryfunc = func; }
void glutMultiMotionFunc( void (*func)(int num, int x, int y) ) { g_extmotionfunc = func; }
void glutMultiButtonFunc( void (*func)(int num, int x, int y, int button, int state) ) { g_extbuttonfunc = func; }
void glutMultiPassiveFunc( void (*func)(int num, int x, int y) ) { g_extpassivefunc = func; }

void glutDestroyWindow(int win) { }
void glutPostRedisplay() { g_redisplay = 1; }
void glutSwapBuffers() { }
void glutFullScreen() { }

void glutMainLoop() {
	int argc = 1;
	char* argv[] = { "libsimplecv", 0 };
	UIApplicationMain(argc, argv, nil, @"SimpleCVAppDelegate");
	[g_pool release];
}

