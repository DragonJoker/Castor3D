#!/usr/bin/python

import wx
import castor
import castor_wx

class MainFrame(wx.Frame):
	def __init__(self, title):
		super(MainFrame, self).__init__(None, title=title, size=(800, 600))
		self.Move((800, 250))
		self.Show()
		colour = castor.utils.Colour
		colour.r = 127
		colour.g = 0
		colour.b = 0
		colour.a = 255
		self.SetBackgroundColour(wx.Colour(colour.r,colour.g,colour.b))
		castor_wx.makeWindowHandle(wx.Frame(self))


if __name__ == '__main__':
	app = wx.App()
	MainFrame('Python CastorViewer')
	app.MainLoop()
