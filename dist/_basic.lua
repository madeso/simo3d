-- table.pack isn't abailable in 5.1
function custom_pack(...)
  return { n = select("#", ...), ... }
end

function print_wrapper(...)
  local args = custom_pack(...)
  local str = ""
  local first = true
  for i=1,args.n do
    if first then
      first = false
    else
      str = str .. " "
    end
    str = str .. tostring(args[i])
  end
  logline(str)
end
print = print_wrapper

print("Hello world")
