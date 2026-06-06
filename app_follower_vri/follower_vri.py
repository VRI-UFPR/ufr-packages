# Projeto Pioneer
# Copyright (C) 2025  Visao Robotica e Imagem (VRI)
#  - Luize Duarte
# 
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#  
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#  
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.
#
# ============================================================================
#  Header
# ============================================================================

import ufr

# Define constants for directions and thresholds
PERSON_CLASS = 'person'
LEFT = 'left'
RIGHT = 'right'
FORWARD = 'forward'
STOP = 'stop'
CENTER_MIN = 3030
CENTER_MAX = 340
IMAGE_CENTER = 320

# ============================================================================
#  class simple_follower
# ============================================================================

class simple_follower:

    def __init__(self):
        # super().__init__('simple_follower')
        # subscription to model results
        # self.subscription = self.create_subscription(ModelResults, '/model_results', self.calc_movement, 10)
        pass

    def move_robot(self, speed, direction):
        # geometry_msgs/Twist Message
        # alterar o x do primeiro vetor e o z do segundo.
        print(f"Speed: {speed}, Direction: {direction}")

    def calc_movement(self, data):
        # checks if the object is a person, if not, stops the robot
        # the robot follows the first person detected in the image
        for r in data.model_results:
            if r.class_name == PERSON_CLASS:
                x = (r.top + r.bottom) / 2
                direction = self.get_direction(x)
                speed = self.calculate_speed(x)

                self.move_robot(speed, direction)
                return  # Stop after finding the first person
        
        # If no person is detected, stop the robot
        self.move_robot(0, STOP)

    def get_direction(self, x):
        if x < CENTER_MIN:
            return LEFT
        elif x > CENTER_MAX:
            return RIGHT
        return FORWARD

    def calculate_speed(self, x):
        return abs(x - IMAGE_CENTER) / IMAGE_CENTER
        # deve estar entre 0 20 ou 30

# ============================================================================
#  Main
# ============================================================================

follower = simple_follower()

model = ufr_

while True:
    