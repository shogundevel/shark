import pyshark.runtime as __shark_rt__
import os
__shark_rt__.add_import_path(os.path.split(__file__)[0])
__local_0__ = __shark_rt__.import_module('shark.activity')
activity = __local_0__.activity
__local_1__ = __shark_rt__.import_module('shark.asset')
load_texture = __local_1__.load_texture
event = __shark_rt__.import_module('shark.event')
main = None
w = load_texture("w.png")
b = load_texture("b.png")
class main_activity(activity):
  def launch(self):
    self.rotation = 0
    self.scale = 1
    self.up = False
    self.down = False
    self.left = False
    self.right = False
  def event(self, type, x, y):
    if type == event.E_PRESS_UP:
      self.up = True
    elif type == event.E_PRESS_DOWN:
      self.down = True
    elif type == event.E_PRESS_LEFT:
      self.left = True
    elif type == event.E_PRESS_RIGHT:
      self.right = True
    elif type == event.E_REL_UP:
      self.up = False
    elif type == event.E_REL_DOWN:
      self.down = False
    elif type == event.E_REL_LEFT:
      self.left = False
    elif type == event.E_REL_RIGHT:
      self.right = False
  def update(self):
    if self.up:
      self.scale += 0.02
    if self.down:
      self.scale -= 0.02
    if self.left:
      self.rotation += 5
    if self.right:
      self.rotation -= 5
  def render(self):
    self.draw_ex(w, 160, 96, 0, 0, 0, 5, 3)
    self.draw_ex(b, 160, 96, 0, 0, self.rotation, self.scale, self.scale)
if main:
  import sys
  main(sys.argv)
