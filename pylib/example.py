from geode import *
from gd import *
import geode
import sys

mod = Mod.get()
version = mod.get_version()

log.info(f"Mod version: {version}")

@geode.modify
class MenuLayer:
    @geode.modify_func
    def init(self):
        log.info("init!")

        geode.call_original(MenuLayer.init, self)
        notif = Notification.create(
            "what the hell",
            NotificationIcon.Warning,
            5.0
        )
        notif.show()

        return True

@geode.modify
class PlayLayer:
    @geode.modify_func
    def init(self: geode.Addr, level: geode.Addr, arg1: bool, arg2: bool):
        # call the original PlayLayer::init with the same arguments
        if not geode.call_original(PlayLayer.init, self, level, arg1, arg2):
            return False

        # 'level' is right now just a memory address, convert it into a `GJGameLevel*` to access fields
        level_struct = GJGameLevel.from_address(level)

        notif = Notification.create(
            f"you opened the level {level_struct.name}",
            NotificationIcon.Success,
            3.0
        )
        notif.show()
        return True

# mod.add_hook("MenuLayer::onNewgrounds", on_newgrounds, geode.OriginalPlacement.Discard)
