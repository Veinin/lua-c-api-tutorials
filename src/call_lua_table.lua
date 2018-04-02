function tablehandler(t)
	local returnedt = {numfields = 1}

	for i, v in pairs(t) do
		returnedt.numfields = returnedt.numfields + 1
		returnedt[tostring(i)] = tostring(v)
	end

	io.write("this is comming from table handler. table num fields : ", returnedt.numfields, "\n")
	return returnedt
end