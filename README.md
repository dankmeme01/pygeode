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
```

needed files to compile are `include`, `Lib.zip`, and `python312.lib`, put em in `py` subfolder.

todos:

* hooking the same function multiple times is intentionally broken
* fix gd::string and allow functions that return/accept it
* add bindings for common structs (color3B, ccpoint, etc.)
* codegen a python `bind` module that wraps around `_internal.call_original` for all functions in the broma
* somehow do the same for cocos functions. have fun figuring out how
* better struct member access instead of MBO
* allow converting `_Ptr` into a pointer in `PyRuntime`
* load scripts from resources or some other folder at startup