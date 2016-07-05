## General SQL parser lua module
    code from https://github.com/TwoLaid/python-sqlparser/
    gsp(General SQL Parser，http://www.sqlparser.com)
    gsp为收费软件，试用版本只能解释1k个字符长的sql，并限制90天试用。

## Dev help
  - base on lua 5.1
  - unit-test by busted; install by luarocks
  - python download-gsp-sqlparer-c-lib.py  to download gsp_c_lib.tar.gz
  - cmake .
  - make
  - just test on Linux x64

Usage
----
```lua
sqlparser = require("sqlparser")

query = "SELECT a, b FROM table_1 WHERE c > 20"
parser = sqlparser.NewParser(sqlparser.EVendor.mysql)

assert(parser.check_syntax(query) == 0)

-- Get first statement from the query
stmt = parser.get_statement(0)

assert(stmt:get_stmttype() == sqlparser.EStmtType.sstselect)

node = stmt:get_root()

for k,v in pairs(getmetatable(node)) do
    print(v, k, type(v))
end

```


