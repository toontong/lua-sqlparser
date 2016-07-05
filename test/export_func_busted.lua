package.cpath = package.cpath.. ";../?.so"
require("busted")


describe("Base export functions and Enums", function()
        it("functions", function() 
            sqlparser = require "sqlparser"

            assert.are.equal(2 ,  sqlparser.EVendor.mysql)
            p = sqlparser.NewParser(sqlparser.EVendor.mysql)
            assert.are.equal(type(sqlparser.EStmtType), "table")
            assert.are.equal(type(sqlparser.ESetType), "table")
            assert.are.equal(type(sqlparser.ECreateType), "table")
            assert.are.equal(type(sqlparser.EExecType), "table")
            assert.are.equal(type(sqlparser.EVendor), "table")
            assert.are.equal(type(sqlparser.ENodeType), "table")
        
            query = "SELECT a, b FROM table_1 WHERE c > 20"
            assert.True(0 == p:check_syntax(query))
            assert.True(1 == p:get_statement_count())
            st = p:get_statement(0)
            assert.True(nil ~= st)
            assert.True(sqlparser.EStmtType.select == st:get_stmttype())
            node = st:get_root()
            assert.True(nil ~= node)
            assert.True(nil ~= getmetatable(node))

            p2 = sqlparser.NewParser(sqlparser.EVendor.mysql)
            assert.are.equal(getmetatable(p), getmetatable(p2))
            assert.True(0 == p2:check_syntax(query))
            st2 = p2:get_statement(0)
            assert.are.equal(getmetatable(st), getmetatable(st2))
            n2 = st2:get_root()
            assert.are_not.equal(getmetatable(node), getmetatable(n2))
            -- print("position of root: ",n2:get_position()) 
            -- print("position of from: ",n2.fromTableList:get_position()) 
            -- print("position of where: ",n2.whereCondition:get_position()) 
            -- print("position of result: ",n2.resultColumnList:get_position()) 

            assert.are.equal(sqlparser.ENodeType.selectStatement, n2.node_type)
            assert.are.equal("table_1", n2.fromTableList:get_text())
            assert.are.equal("WHERE c > 20", n2.whereCondition:get_text())
            assert.are.equal("a, b", n2.resultColumnList:get_text())

            for k,v in pairs(getmetatable(n2)) do
            --        print(v, k, type(v))
            end
            --print("\t ---- statement type ---- ")
            for k,v in pairs(getmetatable(st)) do
            --        print(v, k, type(v))
            end

        end)
end)
