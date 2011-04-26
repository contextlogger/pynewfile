# This produces a nice viewfinder, but the usual Camera application controls are not present.

import appuifw
import camera
import graphics
import e32

myLock = e32.Ao_lock()

def quit():
  camera.stop_finder()
  camera.release()
  myLock.signal()

# Just updates the viewfinder. We do not actually provide a way to take a photo here.
def cb(im):
  appuifw.app.body.blit(im)

appuifw.app.exit_key_handler = quit
appuifw.app.body = appuifw.Canvas()
camera.start_finder(cb)

myLock.wait()
