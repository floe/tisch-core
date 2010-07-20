import sys
#sys.path += [ "." ]

from libtisch import *

class MyTile(Tile):
	def __init__(self,w,h,x,y,a):
		super(MyTile,self).__init__(w,h,x,y,a)
	def tap(self,pos,fid):
		print "tile::tap(" + str(pos.x) + " " + str(pos.y) + " " + str(fid) + " )"

def main():

	use_mouse = 1
	window = Window(800,600,"libTISCH Python example",use_mouse)

	foo = MyTile(100,100,0,0,0.5)
	window.add(foo)

	window.update()
	window.run()

if __name__ == '__main__':
	main()

