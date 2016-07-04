dumper = require ("tbdumper")
parser = require("gsqlparser")

print("==== the gsqlparser.so functions list =====")
dumper.print(parser, 1)
print("==== end gsqlparser.so funcstion list =====\n")

p = parser.NewParser(parser.EVendor.mysql)
print("----- new a Pasrer(vendor=1) ---- ")
dumper.print(p)
print("----- end a Pasrer() funtions ----\n")

print("---- Parser.__metatable -->")
dumper.print(getmetatable(p), 2)

print("---- end Parser.__metatable <--\n")

query = "select * from tb1 where col1='A';"
-- assert(0 ~= (p:check_syntax("error" .. query)))
assert(0 == p:check_syntax(query))

dumper.print(p:tokenize(query))

-- print( #p:tokenize(query))

dumper.print(p:get_statement_count())
s = p:get_statement(0)
dumper.print(getmetatable(s), 1)
print("---- end statement metatable ---")
n = s:get_root()
-- dumper.print(getmetatable(n), 1)
