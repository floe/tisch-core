// compile with: gmcs -warn:4 -r:/path/to/libtisch.dll example.cs 
 
using System;

class MyTile: Tile {
	public MyTile(int _w, int _h, int _x, int _y, double angle): base(_w,_h,_x,_y,angle) { }
	public override void tap(Vector pos, int id) {
		Console.WriteLine(String.Format("tile::tap( {0} {1} {2} )",pos.x,pos.y,id));
	}
}

class Example {

	static void Main() {

		int use_mouse = 1;

		Window win = new Window(800,600,"libTISCH C# Example",use_mouse);
		win.add(new MyTile(100,100,0,0,0.5));

		win.update();
		win.run();
	}
}

