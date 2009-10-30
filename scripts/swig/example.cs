// Hello World! : hello.cs
 
using System;

class Hello {
	static void Main() {

		Vector bar = new Vector(1,2,3);
		Region foo = new Region();
		foo.Add(bar);

		Console.WriteLine ("Hello, World!");

		//foo.print();

	}
}
