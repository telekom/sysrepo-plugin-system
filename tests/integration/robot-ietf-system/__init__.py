from robot.api.deco import keyword

import sys
import os


ROBOT_AUTO_KEYWORDS = False


@keyword('Import Root Robot Path')
def import_root_robot_path() -> None:
    sys.path.insert(0, os.path.dirname(__file__))

