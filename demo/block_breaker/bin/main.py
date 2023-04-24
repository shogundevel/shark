import pyshark.runtime as __shark_rt__
import os
__shark_rt__.add_import_path(os.path.split(__file__)[0])
__local_0__ = __shark_rt__.import_module('shark.activity')
activity = __local_0__.activity
asset = __shark_rt__.import_module('shark.asset')
event = __shark_rt__.import_module('shark.event')
persistent = __shark_rt__.import_module('shark.persistent')
math = __shark_rt__.import_module('system.math')
__local_1__ = __shark_rt__.import_module('system.util')
copy = __local_1__.copy
__local_2__ = __shark_rt__.import_module('system.string')
format = __local_2__.format
stoi = __local_2__.stoi
itos = __local_2__.itos
__local_3__ = __shark_rt__.import_module('system.io')
puts = __local_3__.puts
printf = __local_3__.printf
main = None
black_bar = asset.load_texture("black_bar.png")
black_ball = asset.load_texture("black_ball.png")
square = asset.load_texture("square.png")
bg = asset.load_texture("bg.png")
font = asset.load_font("courier_new.ttf", 12, 0x0)
main_screen_text = "Press any button to start a new game."
W = 16
H = 22
class main_activity(activity):
  def launch(self):
    self.left_down = False
    self.right_down = False
    self.down_down = False
    self.event_time = 0
    self.high_score = 0
    self.score = 0
    __local_0__ = persistent.get_save_file("r")
    if __local_0__ != None:
      self.high_score = stoi(__local_0__.read(256))
      __local_0__.close()
    self.in_game = False
    self.enter_game()
  def new_game(self):
    if self.score > self.high_score:
      self.high_score = self.score
      __local_0__ = persistent.get_save_file("w")
      __local_0__.puts(itos(self.high_score))
      __local_0__.close()
    self.in_game = False
  def enter_game(self):
    self.in_game = True
    __local_0__ = []
    for __local_1__ in range(W):
      __local_0__.append(False)
    self.board = []
    for __local_2__ in range(H):
      self.board.append(__local_0__)
    self.score = 0
    self.bar_x = 136
    self.ball_x = 152
    self.ball_y = 168
    self.speed_x = 1
    self.speed_y = -1
    self.new_line()
    self.new_line()
  def new_line(self):
    for __local_0__ in self.board[H - 1]:
      if __local_0__:
        self.new_game()
    __local_1__ = []
    for __local_2__ in range(W):
      __local_1__.append(True)
    if self.ball_y <= 8:
      __local_1__[math.floor((self.ball_x - 96) / 8)] = False
      self.score += 1
    for __local_3__ in range(H - 1):
      self.board[H - __local_3__ - 1] = self.board[H - __local_3__ - 2]
    self.board[0] = __local_1__
  def event(self, type, x, y):
    if not self.in_game:
      self.enter_game()
    else:
      if type == event.E_PRESS_LEFT:
        self.left_down = True
      elif type == event.E_PRESS_RIGHT:
        self.right_down = True
      elif type == event.E_REL_LEFT:
        self.left_down = False
      elif type == event.E_REL_RIGHT:
        self.right_down = False
  def update(self):
    if self.in_game:
      if self.left_down:
        self.bar_x = math.max(self.bar_x - 8, 96)
      if self.right_down:
        self.bar_x = math.min(self.bar_x + 8, 176)
      self.ball_x += self.speed_x * 8
      self.ball_y += self.speed_y * 8
      if self.ball_x < 96 or self.ball_x > 216:
        self.ball_x -= self.speed_x * 8
        self.speed_x = -self.speed_x
      if self.ball_y < 0:
        self.ball_y -= self.speed_y * 8
        self.speed_y = -self.speed_y
      elif self.ball_y == 176 and self.ball_x >= self.bar_x and self.ball_x < self.bar_x + 48:
        if self.left_down:
          self.speed_x = -1
        elif self.right_down:
          self.speed_x = 1
        self.ball_y -= self.speed_y * 8
        self.speed_y = -self.speed_y
      elif self.ball_y > 192:
        self.new_game()
      while True:
        if (self.ball_x >= 104 and self.ball_y < 176 and self.speed_x == -1 and self.board[math.floor(self.ball_y / 8)][math.floor((self.ball_x - 96) / 8) - 1]):
          self.board[math.floor(self.ball_y / 8)][math.floor((self.ball_x - 96) / 8) - 1] = False
          self.score += 1
          if self.score % 16 == 0:
            self.new_line()
          self.speed_x = -self.speed_x
        elif (self.ball_x < 216 and self.ball_y < 176 and self.speed_x == 1 and self.board[math.floor(self.ball_y / 8)][math.floor((self.ball_x - 96) / 8) + 1]):
          self.board[math.floor(self.ball_y / 8)][math.floor((self.ball_x - 96) / 8) + 1] = False
          self.score += 1
          if self.score % 16 == 0:
            self.new_line()
          self.speed_x = -self.speed_x
        elif (self.ball_y >= 8 and self.ball_y < 176 and self.speed_y == -1 and self.board[math.floor(self.ball_y / 8) - 1][math.floor((self.ball_x - 96) / 8)]):
          self.board[math.floor(self.ball_y / 8) - 1][math.floor((self.ball_x - 96) / 8)] = False
          self.score += 1
          if self.score % 16 == 0:
            self.new_line()
          self.speed_y = -self.speed_y
        elif (self.ball_y < 168 and self.speed_y == 1 and self.board[math.ceil(self.ball_y / 8) + 1][math.floor((self.ball_x - 96) / 8)]):
          self.board[math.ceil(self.ball_y / 8) + 1][math.floor((self.ball_x - 96) / 8)] = False
          self.score += 1
          if self.score % 16 == 0:
            self.new_line()
          self.speed_y = -self.speed_y
        elif (self.ball_x >= 104 and self.ball_y >= 8 and self.ball_y < 176 and self.speed_x == -1 and self.speed_y == -1 and self.board[math.floor(self.ball_y / 8) - 1][math.floor((self.ball_x - 96) / 8) - 1]):
          self.board[math.floor(self.ball_y / 8) - 1][math.floor((self.ball_x - 96) / 8) - 1] = False
          self.score += 1
          if self.score % 16 == 0:
            self.new_line()
          self.speed_x = -self.speed_x
          self.speed_y = -self.speed_y
          break
        elif (self.ball_x < 216 and self.ball_y >= 8 and self.ball_y < 176 and self.speed_x == 1 and self.speed_y == -1 and self.board[math.floor(self.ball_y / 8) - 1][math.floor((self.ball_x - 96) / 8) + 1]):
          self.board[math.floor(self.ball_y / 8) - 1][math.floor((self.ball_x - 96) / 8) + 1] = False
          self.score += 1
          if self.score % 16 == 0:
            self.new_line()
          self.speed_x = -self.speed_x
          self.speed_y = -self.speed_y
          break
        elif (self.ball_x >= 104 and self.ball_y < 168 and self.speed_x == -1 and self.speed_y == 1 and self.board[math.floor(self.ball_y / 8) + 1][math.floor((self.ball_x - 96) / 8) - 1]):
          self.board[math.floor(self.ball_y / 8) + 1][math.floor((self.ball_x - 96) / 8) - 1] = False
          self.score += 1
          if self.score % 16 == 0:
            self.new_line()
          self.speed_x = -self.speed_x
          self.speed_y = -self.speed_y
          break
        elif (self.ball_x < 216 and self.ball_y < 168 and self.speed_x == 1 and self.speed_y == 1 and self.board[math.floor(self.ball_y / 8) + 1][math.floor((self.ball_x - 96) / 8) + 1]):
          self.board[math.floor(self.ball_y / 8) + 1][math.floor((self.ball_x - 96) / 8) + 1] = False
          self.score += 1
          if self.score % 16 == 0:
            self.new_line()
          self.speed_x = -self.speed_x
          self.speed_y = -self.speed_y
          break
        else:
          break
  def render(self):
    self.draw(bg, 0, 0)
    if self.in_game:
      for __local_0__ in range(len(self.board)):
        __local_1__ = self.board[__local_0__]
        for __local_2__ in range(len(__local_1__)):
          if __local_1__[__local_2__]:
            self.draw(square, 96 + __local_2__ * 8, 0 + __local_0__ * 8)
      self.draw_text(itos(self.score), font, 260, 90)
      self.draw(black_bar, self.bar_x, 176)
      self.draw(black_ball, self.ball_x, self.ball_y)
    else:
      self.draw_text(main_screen_text, font, 40, 90)
      self.draw_text(format("score: %, high score %", [self.score, self.high_score]), font, 80, 106)
if main:
  import sys
  main(sys.argv)
