// global events in main window
public void scale( double factor ) {
  println( "master::scale: " + factor );
}

public void rotate( double angle ) {
  println( "master::rotate: " + angle );
}

public void move( Vector delta ) {
  println( "master::move: " + delta.getX() + " " + delta.getY() );
}

public void down( Vector pos, int id ) {
  println( "master::tap: " + pos.getX() + " " + pos.getY() + " " + id );
}

public void up( int id ) {
  println( "master::release: " + id );
}


// tile subclass example
public class MyTile extends Tile {

  public MyTile( int w, int h, int x, int y, double a ) {
    super(w,h,x,y,a);
  }
  
  public void tap( Vector pos, int id ) {
    println("tile::tap( " + pos.getX() + " " +pos.getY() + " " + id +" )");
  }

  public void draw() {
    enter();
    paint();
    triangle( -20,-20, -20, 20, 20,20 );
    leave();
  }
};


// widgets: must be global to avoid garbage collection
Label l;
Dial d;
Slider s;


// main setup
void setup() {
  libtisch_setup();
  size( 800, 600, OPENGL );
}

boolean init_done = false;

// graphics setup - warning: must be called from draw once
void gfxinit() {

  if (init_done) return;
  init_done = true;

  libtisch_init();

  // movable container + texture example  
  /*RGBATexture tex = new RGBATexture("Box.png");
  Tile foo = new Container(100,100,0,0,PI/6, tex, 0xFF );
  master.add(foo);*/

  MyTile t = new MyTile( 100,100,0,0, PI/6 ); master.add(t);
  s = new Slider(100,30,100,100); master.add(s);
  d = new Dial(100,-100,-100); master.add(d);
  l = new Label("Label",300,30,100,-100); master.add(l);
}


// main draw method
void draw() {

  gfxinit();

  l.set( "Dial: " + d.get() + " Slider: " + s.getpos() );
  
  libtisch_draw();  
  //text("word", 20, -20);
  triangle( 20,-20, 20,-60, 60,-60 );
}

