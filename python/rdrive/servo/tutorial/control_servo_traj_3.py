import logging
import os
import sys

sys.path.append(os.path.join(os.path.dirname(__file__), "../.."))

import rdrive as rr
from rdrive.servo.tutorial import *

logging.basicConfig()
logger = logging.getLogger(os.path.basename(__file__))
logger.setLevel(logging.INFO)

if __name__ == '__main__':
    logger.info("Initializing ServoApi")
    api = rr.ServoApi()

    logger.info("Initializing interface {}".format(INTERFACE_NAME))
    interface = api.init_interface(INTERFACE_NAME)

    logger.info("Initializing servo id {}".format(SERVO_1_ID))
    servo1 = interface.init_servo(SERVO_1_ID)

    logger.info("Initializing servo id {}".format(SERVO_2_ID))
    servo2 = interface.init_servo(SERVO_2_ID)

    logger.info("Initializing servo id {}".format(SERVO_3_ID))
    servo3 = interface.init_servo(SERVO_3_ID)

    logger.info("Appending points")
    servo1.add_motion_point(100., 0., 6000)
    servo1.add_motion_point(-100., 0., 6000)
    servo1.add_motion_point(0, 0, 6000)
    servo2.add_motion_point(100., 0., 6000)
    servo2.add_motion_point(-100., 0., 6000)
    servo2.add_motion_point(0, 0, 6000)
    servo3.add_motion_point(100., 0., 6000)
    servo3.add_motion_point(-100., 0., 6000)
    servo3.add_motion_point(0, 0, 6000)
    logger.info("Starting motion")
    interface.start_motion(0)

    logger.info("Waiting till motion ends")
    api.sleep_ms(20000)

