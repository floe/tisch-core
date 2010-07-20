// compile with: javac -cp /path/to/libtisch.jar example.java
//     run with: java  -cp /path/to/libtisch.jar:. example

import libtisch.*;

class MyTile extends Tile {

	public MyTile( int w, int h, int x, int y, double a ) {
		super(w,h,x,y,a);
	}
	
	public void tap( Vector pos, int id ) {
		System.out.println("tile::tap( " + pos.getX() + " " +pos.getY() + " " + id +" )");
	}

};

public class example {

	public static void main( String args[] ) {

		int use_mouse = 1;
		Window win = new Window(800,600,"libTISCH Java Example",use_mouse);
		win.add( new MyTile(100,100,0,0,0.5) );

		win.update();
		win.run();
	}
}

