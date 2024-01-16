from __future__ import annotations
import _gd
import _internal
from basic import *

# Similar to _Ptr, but handles cocos reference counting to make sure it doesn't point to an invalid object
class _CocosPtr(_Ptr):
    def __init__(self, ptr: Addr):
        super().__init__(ptr)
        _gd.CCObject_retain(ptr)

    def __del__(self):
        _gd.CCObject_release(self._ptr)

class _StructPtr(_CocosPtr):
    def _mbo_string(self, offset: int) -> str:
        return _internal.mbo_string(self._ptr, offset)

    def _mbo_int(self, offset: int) -> int:
        return _internal.mbo_int(self._ptr, offset)

class FLAlertLayer(_CocosPtr):
    @staticmethod
    def create(title: str, desc: str, btn: str = "Ok") -> FLAlertLayer:
        return FLAlertLayer.from_address(_gd.FLAlertLayer_create(title, desc, btn))

    def show(self):
        _gd.FLAlertLayer_show(self._ptr)

class GJGameLevel(_StructPtr):
    @property
    def name(self):
        return self._mbo_string(0x118)
