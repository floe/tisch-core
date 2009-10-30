using System;
using System.Threading;
using System.Runtime.InteropServices; 
using Tao.OpenGl;
using Tao.FreeGlut;
using TischDemo;
using TischSharp;

namespace TischDemo
{
    class Program : MasterContainer
    {
		Container c;

        int WHEEL_UP = 3;
        int WHEEL_DOWN = 4;

        int framenum = 0;
		
		//Fake blob info
		SharpBlob[] blobArr = new SharpBlob[3];
		int[] valArr = new int[3];
		
		public Program(int w, int h) : base(w, h, "127.0.0.1")
		{
			texture(null); //black bg

			
			//Test widgets
            c = new Container(1332, 732, 0, 0, 0.0, null, 0xFF);
            add(c);
            update();
            c.add( new Button(60, 30, -150, 90));
            draw();
			
			buildFakeBlobData();

            //Fake events
            InitGlutWireEvents();
		}

		static void Main(string[] args)
        {
			InitGl();
            new Program(640, 480);
            Glut.glutMainLoop();
        }
		
		//Der glue damit die blobdaten verschickt werden können
		//Alternativ: GestureDaemon
        [DllImport("libmouseglue")]
        public static extern void send_blob(int num, double b_angle, double b_scale, int val, int x, int y);

		[DllImport("libmouseglue")]
		public static extern int send_frame(int num);
		
		
				
		
		private void buildFakeBlobData()
		{
			for (int i = 0; i<3; i++) {
				valArr[i] = 1;
				blobArr[i] = new SharpBlob();
				blobArr[i].pos.x = 0;
				blobArr[i].pos.y = 0;
			}
		}

        public static void InitGl()
        {
			//GL(UT) setup
            Vector v = new Vector(); //Win32 hack glutInit()
            Glut.glutInitDisplayMode(Glut.GLUT_DOUBLE | Glut.GLUT_RGB);
            Glut.glutInitWindowSize(640, 480);
            //Glut.glutInitWindowPosition(50, 50);
            Glut.glutCreateWindow("TD");            
            //Glut.glutGetWindow();
            
			Gl.glShadeModel( Gl.GL_FLAT );
            Gl.glEnable( Gl.GL_DEPTH_TEST );
            Gl.glDepthFunc( Gl.GL_LEQUAL );
            Gl.glViewport(0, 0, 640, 480);
            Gl.glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            Gl.glClear(Gl.GL_COLOR_BUFFER_BIT | Gl.GL_DEPTH_BUFFER_BIT);
            Gl.glMatrixMode(Gl.GL_PROJECTION);
            Gl.glLoadIdentity();
            Glu.gluOrtho2D(0, 640, 0, 480);
            Gl.glBlendFunc( Gl.GL_SRC_ALPHA, Gl.GL_ONE_MINUS_SRC_ALPHA );
            Gl.glEnable( Gl.GL_BLEND );
            Gl.glMatrixMode(Gl.GL_MODELVIEW);
            Gl.glLoadIdentity();
            Gl.glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
        }

        private void InitGlutWireEvents()
        {
            Glut.glutKeyboardFunc(new Glut.KeyboardCallback(KeyboardEvent));
            Glut.glutDisplayFunc(new Glut.DisplayCallback(Display));
            Glut.glutMouseFunc(new Glut.MouseCallback(MouseEvent));
            Glut.glutPassiveMotionFunc(new Glut.PassiveMotionCallback(MotionEvent));
			Glut.glutMotionFunc(new Glut.MotionCallback(MotionEvent));
        	Glut.glutIdleFunc(new Glut.IdleCallback(IdleEvent));
		}

		private void IdleEvent()
		{
			if (process() == 0) {
				SendBlobs();
				Glut.glutPostRedisplay();
			}
		}
        
        private void MotionEvent(int x, int y)
        {
            blobArr[0].pos.x = x;
			blobArr[0].pos.y = 480-y;
			SendBlobs();
        }

        private void Display()
        {
            update();
			draw();
            Glut.glutSwapBuffers();
        }

        private void KeyboardEvent(byte key, int x, int y)
        {
			System.Console.WriteLine("Key");
			//Fake fingers
			if (key == '1') {
				blobArr[1].pos.x = x;
				blobArr[1].pos.y = 480-y;
				valArr[1] = (valArr[1]%2)+1;
			}
			if (key == '2') {
				blobArr[2].pos.x = x;
				blobArr[2].pos.y = 480-y;
                valArr[2] = (valArr[2]%2)+1;
			}
            if (key == 'f') Glut.glutFullScreen();
            if (key == 'q') Environment.Exit(1);
			System.Console.WriteLine("KeyE");
        }
        
		//libtisch demo mouse event code
        private void MouseEvent(int button, int state, int x, int y)
        {
			System.Console.WriteLine("Mouse");
            if(Glut.glutGetModifiers() == 2)
            {
                if ((button == WHEEL_UP) || (button == WHEEL_DOWN))
                {
                    //b_scale = b_scale*1.1;
                    //if (button == WHEEL_DOWN) b_scale = b_scale*0.9;
                    //blob.axis1 = blob.axis1*scale;
                    //blob.axis2 = blob.axis2*scale;
                    //send_blob(-1, angle, b_scale, b_val, b_x, b_y, framenum++);
                    return;
                }
            }

            if ((button == WHEEL_UP) || (button == WHEEL_DOWN))
            {
                double angle = (2.0/180.0)*3.14; //TODO: M_PI
                if (button == WHEEL_DOWN) 
					angle = angle-((2.0/180.0)*3.14);
				else
					angle = angle+((2.0/180.0)*3.14);
                //blob.axis1.rotate( angle );
                //blob.axis2.rotate( angle );
                System.Console.WriteLine("Rotate!");
				System.Console.WriteLine(angle);
//                send_blob(-1, angle, b_scale, b_val, b_x, b_y, framenum++);
                return;
            }

            if (state == Glut.GLUT_DOWN) {
                System.Console.WriteLine("MouseDown!");
                valArr[0] = 2;
            } else if (state == Glut.GLUT_UP  ) {
                System.Console.WriteLine("MouseUp!");
                valArr[0] = 1;
            }
            System.Console.WriteLine("MouseE");
            MotionEvent(x,y);
            
        }
		/*	
		public override void action(Gesture g)
		{
			if (g.name() == "tap") {
				FeatureBase fb = (FeatureBase)g[0].get();
				BlobID m = fb as BlobID;
				if (m != null)
					System.Console.WriteLine(m.name());
				else
					System.Console.WriteLine("BlobID null");
			}
		}
		*/
		private void SendBlobs()
        {

            for (int i = 0; i<3; i++) {
                if (valArr[i] >= 2) {
                    blobArr[i].id = i;
                    blobArr[i].pid = i+1;
                    GestureDaemon.SendFingerBlob(blobArr[i]);
                }
            }
            SendFrame();
        }

        private void SendFrame()
        {
            GestureDaemon.SendFrame(framenum++);
        }
		

    } //End Class Program
} //End Namespace

