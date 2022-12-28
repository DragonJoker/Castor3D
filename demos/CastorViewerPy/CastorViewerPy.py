import Tkinter
import 

class CastorViewer:
    def __init__( self, master ):
        button = Tkinter.Button( master, text="Load a scene" )
        button.pack()

if __name__ == '__main__':
    root = Tkinter.Tk()
    app = CastorViewer( root )
    root.mainloop()
