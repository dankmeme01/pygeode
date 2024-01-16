# pygeode

This is where she makes a mod.

embeds python inside Geometry Dash, allowing you to make minimal mods (hooking, showing popups, patching, creating notifications). WIP. example usage:

```py
import geode
@geode.modify
class MenuLayer:
    @geode.modify_func()
    def onNewgrounds(self, sender: geode.Addr):
        geode.log.info("on newgrounds hook called!")

        # call the original function to show a popup
        geode.call_original(MenuLayer.onNewgrounds, self, sender)

        # create a notification and show it to the user
        notif = geode.Notification.create(
            "what the hell",
            geode.NotificationIcon.Warning,
            5.0
        )
        notif.show()
```

needed files to compile are `include`, `Lib.zip`, and `python312.lib`, put em in `py` subfolder.
