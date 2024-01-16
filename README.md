# pygeode

This is where she makes a mod.

embeds python inside Geometry Dash, allowing you to make minimal mods (hooking, showing popups, patching, creating notifications). WIP. example usage:

```py
from geode import *
from gd import *
import geode

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
```

needed files to compile are `include`, `Lib.zip`, and `python312.lib`, put em in `py` subfolder.
