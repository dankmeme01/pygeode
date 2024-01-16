from __future__ import annotations
import _geode
import _internal
from tuliphook import *
from basic import *

from typing import Callable
from functools import wraps

base: Addr = _geode.get_base()
cocos_base: Addr = _geode.get_cocos_base()
nullptr: Addr = 0

class NotificationIcon:
    None_ = 0
    Loading = 1
    Success = 2
    Warning = 3
    Error = 4

# geode::Mod wrapper
class Mod(_Ptr):
    @staticmethod
    def get() -> Mod:
        return Mod.from_address(_geode.mod_get())

    def patch(self, addr: Addr, data: bytes | bytearray) -> Patch:
        if not isinstance(data, bytes):
            data = bytes(data)

        return Patch.from_address(_geode.mod_patch(self._ptr, addr, data))

    def add_hook(self, function: str, detour: Callable) -> Hook:
        return Hook.from_address(_geode.mod_add_hook(self._ptr, function, detour))

    def get_version(self) -> str:
        return _geode.mod_get_version(self._ptr)

# geode::Loader wrapper
class Loader(_Ptr):
    @staticmethod
    def get() -> Loader:
        return Loader.from_address(_geode.loader_get())

# geode::Notification wrapper
class Notification(_Ptr):
    @staticmethod
    def create(text: str, icon: int = NotificationIcon.None_, time: float = 3.0) -> Notification:
        return Notification.from_address(_geode.Notification_create(text, icon, time))

    def show(self):
        _geode.Notification_show(self._ptr)

# geode::Hook wrapper
class Hook(_Ptr): ...

# geode::Patch wrapper
class Patch(_Ptr): ...

# geode::log namespace wrapper
class log:
    @staticmethod
    def debug(msg: str):
        _geode.log_debug(msg)

    @staticmethod
    def info(msg: str):
        _geode.log_info(msg)

    @staticmethod
    def warn(msg: str):
        _geode.log_warn(msg)

    @staticmethod
    def error(msg: str):
        _geode.log_error(msg)

# $modify wrapper

def modify(class_):
    class_._modify_class_name = type(class_).__name__
    return class_

def modify_func(func):
    func_name = func.__qualname__.replace('.', '::')
    @wraps(func)
    def wrapper(*args, **kwargs):
        return func(*args, **kwargs)

    Mod.get().add_hook(func_name, wrapper)
    wrapper._modify_func_name = func_name
    return wrapper

# original call
def call_original(func, *args):
    name = func._modify_func_name
    return _internal.call_original(name, *args)
