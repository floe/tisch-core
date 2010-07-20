package libtisch;
import processing.core.*;

public class Loader {

  PApplet parent;

  public Loader( PApplet parent ) {
		/*System.loadLibrary("tisch_java");
		System.out.println("libTISCH wrapper loaded.");*/
		this.parent = parent;
		parent.registerDispose(this);
  }

  public void dispose() {
    // anything in here will be called automatically when 
    // the parent applet shuts down. for instance, this might
    // shut down a thread used by this library.
    // note that this currently has issues, see bug #183
    // http://dev.processing.org/bugs/show_bug.cgi?id=183
  }
}

