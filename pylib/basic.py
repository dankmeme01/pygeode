__all__ = [
    "Addr",
    "_Ptr"
]

Addr = int

# Base pointer helper class
class _Ptr:
    _ptr: Addr

    def __init__(self, ptr: Addr):
        self._ptr = ptr

    def __repr__(self):
        return f"<{type(self).__name__}* object pointing to {hex(self._ptr)}>"

    @classmethod
    def from_address(cls, ptr: Addr):
        return cls(ptr)
