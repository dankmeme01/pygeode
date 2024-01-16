from pathlib import Path
import sys
import re

HEADER = """
// This file was automatically generated by generate_hookdetours.py
#include "hookdetours_generated.hpp"

namespace hookdetours {"""

HEADER_HEADER = """
// This file was automatically generated by generate_hookdetours.py
#pragma once
#include "hookdetours_base.hpp"

namespace hookdetours {
"""

FOOTER = """
}
"""

START_FNVARS = """
    namespace fnvars {
"""

END_FNVARS = """
    }
"""

ADD_HOOK_SIG = "geode::Hook* addHook(geode::Mod* mod, const std::string_view name, PyObject* userDetour)"
CALL_ORIGINAL_SIG = "PyObject* callOriginal(PyObject* args)"

CALL_ORIGINAL_BEGIN = f"""
    {CALL_ORIGINAL_SIG} {{
        auto& rt = PyRuntime::get();
        auto name = rt.unpackFromTuple<std::string>(args, 0);
"""

def mangle_name(name: str):
    return name.replace("::", "_")

def split_name(name: str) -> tuple[str, str]:
    return tuple(name.split("::"))

function_list = {
    "MenuLayer::onMoreGames": ("0x277d90", False, "Membercall", "void", ("MenuLayer*", "cocos2d::CCObject*"))
}

# broma parser let's goo!!!!
broma_path = Path(sys.argv[1])

def line_has_function(line: str):
    line = line.strip()
    first_try = line.startswith("TodoReturn") or line.startswith("virtual") or line.startswith("void") or line.endswith(");")

    if first_try and line.endswith(";"): return True

    pattern = r".*\) = (win|mac) 0x[0-9a-fA-F]+(?:, (win|mac) 0x[0-9a-fA-F]+)?;"
    if re.match(pattern, line):
        return True

    return False

def get_func_name(line: str, class_name: str):
    if line.startswith("//"):
        line = line.strip("// \t")

    if line.startswith("virtual"):
        line = line[len("virtual "):]

    if line.startswith("static"):
        line = line[len("static "):]

    if line.startswith("callback"):
        line = line[len("callback "):]

    if line.startswith(f"~{class_name}"):
        return f"~{class_name}"
    elif line.startswith(class_name):
        return class_name

    name = line.partition(" ")[2].partition("(")[0].strip()
    if " " in name:
        name = name.partition(" ")[2]

    return name

def get_func_name_and_ret(line: str, class_name: str):
    func_name = get_func_name(line, class_name)

    if line.startswith("virtual"):
        line = line[len("virtual "):]

    if line.startswith("static"):
        line = line[len("static "):]

    if line.startswith("callback"):
        line = line[len("callback "):]

    return (func_name, line.partition(func_name)[0].strip())

def parse_types_from_arglist(arglist: str):
    arguments = [arg.strip() for arg in arglist.split(",")]
    argument_types = []
    for arg in arguments:
        if not arg:
            continue

        if arg.endswith(" const") or arg.endswith("*const"):
            arg = arg[:-6].strip()

        if ' ' not in arg:
            argument_types.append(arg)
            continue

        noptr = arg.strip("*")
        if noptr.endswith("const"):
            argument_types.append(arg)
        else:
            argument_types.append(arg.rpartition(" ")[0])

    return argument_types

def can_pack_arg(arg: str):
    # pointers are convertible to py 'int'
    if arg.endswith("*"): return True

    # integral types are ok
    if arg in ["bool", "int", "unsigned int", "char", "short", "unsigned char", "unsigned short", "long", "unsigned long", "long long", "unsigned long long"]: return True

    # so are floats
    if arg in ["float", "double"]: return True

    # stuff that's specially handled
    if arg in [
        # "gd::string",
        "std::string"
    ]: return True

    return False

current_class = ""
for line in broma_path.read_text().splitlines():
    line = line.strip()
    if not line: continue
    if line.startswith("class"):
        current_class = line.partition("class ")[2].partition(" ")[0].partition("{")[0]

    if not current_class:
        continue

    if line.startswith("//"):
        continue

    if not line_has_function(line):
        continue

    if not "win " in line:
        continue

    (func_name, return_type) = get_func_name_and_ret(line, current_class)
    if func_name == current_class or func_name.startswith("~"):
        continue

    is_void = return_type in ("void", "TodoReturn")
    if not is_void and not can_pack_arg(return_type):
        continue

    is_static = "static " in line
    full_name = f"{current_class}::{func_name}"
    offset = line.partition("win ")[2].partition(" ")[0].partition(",")[0].strip("; \t\n\r")
    full_arg_string = line.partition("(")[2].partition(")")[0]
    args_types = parse_types_from_arglist(full_arg_string)
    if not is_static:
        # self type
        args_types.insert(0, f"{current_class}*")

    is_ok = True
    for arg_type in args_types:
        if not can_pack_arg(arg_type):
            is_ok = False
            break

    if not is_ok:
        continue

    function_list[full_name] = (offset, is_static, "Optcall" if is_static else "Membercall", return_type, args_types)

dest = Path(sys.argv[0]).parent / "src" / "hookdetours_generated.cpp"
with open(dest, 'w') as f:
    f.write(HEADER)
    f.write(START_FNVARS)

    add_hook_str = f"    {ADD_HOOK_SIG} {{\n"
    call_original_str = CALL_ORIGINAL_BEGIN

    for name, (offset, is_static, convention, return_type, arglist) in function_list.items():
        is_void = return_type in ("void", "TodoReturn")

        add_hook_str += f"        CHECK_HOOK(\"{name}\", {offset}, tulip::hook::TulipConvention::{convention}, {mangle_name(name)});\n"

        # write the function itself
        expanded_params = ", ".join([f"{arg_type} p{i}" for i, arg_type in enumerate(arglist)])
        expanded_arg_types = ", ".join([f"{arg_type}" for arg_type in arglist])

        # gd::string gives an error "cannot compile this forwarded non-trivially copyable parameter yet"
        # i frankly don't know what that really means, but this fixes the issue.
        # however this will probably just break that function.
        expanded_params = expanded_params.replace("gd::string", "gd::string&&")
        expanded_arg_types = expanded_arg_types.replace("gd::string", "gd::string&&")

        class_name, func_name = split_name(name)
        expanded_arg_names = ", ".join([f"std::move(p{i})" for i in range(len(arglist))])
        expanded_arg_names_skip1 = ", ".join([f"std::move(p{i})" for i in range(1, len(arglist))])
        expanded_arg_types_skip1 = expanded_arg_types.partition(",")[2].strip()
        if is_static:
            expanded_orig_call = f"{class_name}::{func_name}({expanded_arg_names})"
        else:
            expanded_orig_call = f"p0->{func_name}({expanded_arg_names_skip1})"

        # call original append
        if is_void or can_pack_arg(return_type):
            if not is_static and not is_void:
                call_original_str += f"        CALL_ORIG_HOOK_NSNV(\"{name}\", {class_name}, {return_type}, {func_name}, {expanded_arg_types_skip1});\n"
            elif not is_static and is_void:
                call_original_str += f"        CALL_ORIG_HOOK_NSV(\"{name}\", {class_name}, {func_name}, {expanded_arg_types_skip1});\n"

        if is_void:
            f.write(f"""
auto {mangle_name(name)}(PyObject* userDetour) {{
    static PyObject* storedDetour = nullptr;
    COMMON_UD(userDetour);
    return +[]({expanded_params}) {{
        PyObject* args = PyRuntime::get().packTuple({expanded_arg_names});
        auto res = PyObject_CallObject(storedDetour, args);
        Py_DECREF(args);
        handleException(res);
    }};
}}
""")
        else:
            f.write(f"""
auto {mangle_name(name)}(PyObject* userDetour) {{
    static PyObject* storedDetour = nullptr;
    COMMON_UD(userDetour);
    return +[]({expanded_params}) -> {return_type} {{
        {return_type} retval;
        PyObject* args = PyRuntime::get().packTuple({expanded_arg_names});
        auto res = PyObject_CallObject(storedDetour, args);
        Py_DECREF(args);
        handleException(res);
        return PyRuntime::get().fromPyObject<{return_type}>(res);
    }};
}}
""")


    f.write(END_FNVARS)
    # addHook
    add_hook_str += "\n        hookFail(name);\n    }\n"
    f.write(add_hook_str)

    # callOriginal
    call_original_str += "\n        return nullptr;\n    }\n"
    f.write(call_original_str)
    f.write(FOOTER)

# generate the header
dest = Path(sys.argv[0]).parent / "src" / "hookdetours_generated.hpp"
with open(dest, 'w') as f:
    f.write(HEADER_HEADER)
    f.write(START_FNVARS)

    for name, (offset, is_static, convention, return_type, arglist) in function_list.items():
        f.write(f"        auto {mangle_name(name)}(PyObject* userDetour, int origPlacement);\n")

    f.write(END_FNVARS)
    f.write(f"{ADD_HOOK_SIG};")
    f.write(FOOTER)