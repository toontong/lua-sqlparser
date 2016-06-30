dumper = require ("tbdumper")
parser = require("gsqlparser")

print("==== the gsqlparser.so functions list =====")
dumper.print(parser)
print("==== end gsqlparser.so funcstion list =====\n")

p = parser.Parser(1)
print("----- new a Pasrer(vendor=1) ---- ")
dumper.print(p)
print("----- end a Pasrer() funtions ----\n")

print("---- Parser.__metatable -->")
dumper.print(getmetatable(p), 1)

print("---- end Parser.__metatable <--\n")

dumper.print(gsqlparser.Parser)

print(p:tostring())