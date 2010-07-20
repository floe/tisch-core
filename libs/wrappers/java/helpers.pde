import javax.media.opengl.*;
import processing.opengl.*;

import libtisch.Loader;
import libtisch.Vector;

import libtisch.RGBATexture;
import libtisch.Mouse;
import libtisch.MasterContainer;

import libtisch.Container;
import libtisch.Label;
import libtisch.Tile;
import libtisch.Button;
import libtisch.Dial;
import libtisch.Slider;

import libtisch.Gesture;
import libtisch.FeatureBase;
import libtisch.BlobCount;
import libtisch.BlobPos;
import libtisch.BlobID;
import libtisch.Motion;
import libtisch.Rotation;
import libtisch.Scale;

// own master container class to catch events and hand them off to global methods
public class ProcessingMasterContainer extends MasterContainer {
  public ProcessingMasterContainer( int w, int h ) { super( w, h ); }
  public void action( Gesture g ) {
     String name = g.name();
     if (name.equals("move")) {
       FeatureBase fb0 = g.get(0).get();
       Motion m = Motion.dynamic_cast(fb0);
       move( m.result() );
     } else
     if (name.equals("scale")) {
       FeatureBase fb0 = g.get(0).get();
       Scale s = Scale.dynamic_cast(fb0);
       scale( s.result() );
     } else
     if (name.equals("rotate")) {
       FeatureBase fb0 = g.get(0).get();
       Rotation r = Rotation.dynamic_cast(fb0);
       rotate( r.result() );
     } else
     if (name.equals("tap")) {
       FeatureBase fb0 = g.get(0).get(); BlobID  bi = BlobID.dynamic_cast(fb0);
       FeatureBase fb1 = g.get(1).get(); BlobPos bp = BlobPos.dynamic_cast(fb1);
       down( bp.result(), bi.result() );
     } else
     if (name.equals("remove")) {
       FeatureBase fb0 = g.get(0).get();
       BlobID bi = BlobID.dynamic_cast(fb0);
       up( bi.result() );
     }
  }
};

// global objects
ProcessingMasterContainer master;
Mouse mymouse;
int do_update = 2;
boolean do_mouse = false;

// mouse handlers
void mouseMoved()    { if (do_mouse) mymouse.passive( 1, mouseX, height-mouseY ); }
void mouseDragged()  { if (do_mouse) mymouse.motion( 1, mouseX, height-mouseY ); }
void mousePressed()  { if (do_mouse) mymouse.button( 1, 1, 0, mouseX, height-mouseY ); }
void mouseReleased() { if (do_mouse) mymouse.button( 1, 1, 1, mouseX, height-mouseY ); }

void keyPressed() {
  if (key == 'q') { master.signOff(); exit(); }
  if (key == 'm') { do_mouse = !do_mouse; }
}

// setup routine - call this in setup()
void libtisch_setup() {
  Loader ltload = new Loader( this );
}

// gfx init - call this in draw() _once_
void libtisch_init() {
  
  master = new ProcessingMasterContainer( width, height );
  master.texture( null );
  //master.color( 0.0, 0.0, 0.0, 1.0 );
  master.setShadow( false );

  mymouse = new Mouse();
  mymouse.entry( 1, 1 );

  // nasty hack for primitive mouse wheel support
  addMouseWheelListener(new java.awt.event.MouseWheelListener() {
    public void mouseWheelMoved(java.awt.event.MouseWheelEvent evt) {
      int step = evt.getWheelRotation();
      mymouse.button( 1, (7+step)/2, 1, mouseX, height-mouseY );
    }
  });
}

// draw routine - call this in draw() every time
void libtisch_draw() {
  
  PGraphicsOpenGL pgl = (PGraphicsOpenGL) g;  // g may change
  GL gl = pgl.beginGL();  // always use the GL object returned by beginGL

  ortho( 0, width, 0, height, -1000000, 1000000 );
  background(128);
  resetMatrix();
  gl.glMatrixMode(GL.GL_MODELVIEW);
  gl.glLoadIdentity();

  if (do_mouse) mymouse.send_blobs();
  if (do_update != 0) { master.update(); do_update--; }
  
  master.draw();
  master.process();

  pgl.endGL();
}

