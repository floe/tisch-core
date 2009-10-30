import pygame
import libtisch

from pygame.locals import *
from OpenGL.GL import *

from OpenGL.GL import *
from OpenGL.GLU import *

def initializeDisplay(w, h):
	pygame.display.set_mode((w,h), pygame.OPENGL|pygame.DOUBLEBUF)

	glClearColor(0.0, 0.0, 0.0, 1.0)
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT)

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, w, 0, h);
	glMatrixMode(GL_MODELVIEW);


def main():
	pygame.init()
	initializeDisplay(800, 600)

	glColor4f(1.0,1.0,1.0,1.0)

	master = libtisch.MasterContainer(800,600)
	foo = libtisch.Container(100,100,0,0,0.5)
	master.add(foo)

	done = False

	master.update()

	while not done:

		master.draw()
		
		glBegin(GL_TRIANGLES)
		glVertex2f(10,400)
		glVertex2f(400, 400)
		glVertex2f(400, 200)
		glEnd()
			
		pygame.display.flip()

		eventlist = pygame.event.get()
		for event in eventlist:
			if event.type == QUIT or event.type == KEYDOWN and event.key == K_ESCAPE:
				done = True

if __name__ == '__main__':
	main()

