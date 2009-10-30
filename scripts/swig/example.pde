import javax.media.opengl.*;
import processing.opengl.*;
import libtisch.*;

MasterContainer master;
Container foo;
int send = 1;

void setup() {
  size(640, 360, OPENGL);
  Loader ltload = new Loader(this);
  master = new MasterContainer(640,360);
  foo = new Container(100,100,0,0,PI/6);
  Container bar = new Container(10,10,20,20,0);
  RGBATexture tex = new RGBATexture("/home/echtler/color9.png");
  Tile baz = new Tile( 50,50,50,50,0.0, tex );
  master.add(foo);
  foo.add(bar);
  foo.add(baz);
}

void draw() {

  PGraphicsOpenGL pgl = (PGraphicsOpenGL) g;  // g may change
  GL gl = pgl.beginGL();  // always use the GL object returned by beginGL

  ortho( 0, 640, 0, 360, -1000000, 1000000 );
  background(128);
  resetMatrix();
  gl.glMatrixMode(GL.GL_MODELVIEW);
  gl.glLoadIdentity();

  if (send != 0) { 
    master.update();
    send = 0;
  }

  master.draw();
  
  pgl.endGL();
}

