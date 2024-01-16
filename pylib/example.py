from geode import *
from gd import *
import geode
import sys

mod = Mod.get()
version = mod.get_version()

log.info(f"Mod version: {version}")

@geode.modify
class PlayLayer:
    @geode.modify_func
    def init(self: geode.Addr, level: geode.Addr, arg1: bool, arg2: bool):
        # call the original PlayLayer::init with the same arguments
        if not geode.call_original(PlayLayer.init, self, level, arg1, arg2):
            return False

        # 'level' is right now just a memory address, convert it into a `GJGameLevel*` to access fields
        level_struct = GJGameLevel.from_address(level)
        level_name = level_struct.name

        notif = Notification.create(
            f"you opened the level {level_name}",
            NotificationIcon.Success,
            3.0
        )
        notif.show()

        # instant complete hack
        geode.call_function("PlayLayer::levelComplete", self)

        return True

# mod.add_hook("MenuLayer::onNewgrounds", on_newgrounds, geode.OriginalPlacement.Discard)
