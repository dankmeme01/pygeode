import geode
import gd
import sys

mod = geode.Mod.get()
version = mod.get_version()

geode.log.info(f"Mod version: {version}")

@geode.modify
class MenuLayer:
    @geode.modify_func()
    def onNewgrounds(self, sender: geode.Addr):
        geode.log.info("on newgrounds hook called!")

        geode.call_original(MenuLayer.onNewgrounds, self, sender)
        notif = geode.Notification.create(
            "what the hell",
            geode.NotificationIcon.Warning,
            5.0
        )
        notif.show()

# @geode.modify
# class PlayLayer:
#     @geode.modify_func()
#     def init(self: geode.Addr, level: geode.Addr, arg1: bool, arg2: bool):
#         if not geode.call_original(PlayLayer.init, level, arg1, arg2):
#             return False

#         print(level, arg1, arg2)

#         return True

# mod.add_hook("MenuLayer::onNewgrounds", on_newgrounds, geode.OriginalPlacement.Discard)
