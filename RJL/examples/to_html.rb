require "fastesc"

lines = $stdin.readlines()
state = :plain
list_indent = 0
para_indent = -1
in_source = false
stack = []

def print_text(x)
  x.html_escape
  while x =~ /[^'](@class\s+(\w+))[^']/ do
    class_text = $1
    class_name = $2
    x.sub!($1, "<h2> Class: #{class_name}</h2>")
  end
  while x =~ /(@def\s+((\w+)\([^\)]+\)))/ do
    class_text = $1
    class_name = $2
    x.sub!($1, "<h3> Def: #{class_name}</h2>")
  end
  while x =~ /(@throws\s+(\w+))/ do
    class_text = $1
    class_name = $2
    x.sub!($1, "<h3> Throws: #{class_name}</h2>")
  end
  while x =~ /(^|[^"<>A-Za-z0-9'.])([A-Z]+[a-z0-9]+([A-Z]+[a-z0-9]+)+)([^"<>A-Za-z0-9'.]|$)/ do
    wiki_word = $2.dup
    wiki_url = wiki_word.dup
    wiki_url[0,1] = wiki_url[0,1].downcase
    while wiki_url =~ /([a-z0-9])([A-Z])/ do
      wiki_url.sub!($1 + $2, $1 + "_" + $2.downcase)
    end
    x.sub!(wiki_word,"<a href=\"#{wiki_url}.html\">#{wiki_word}</a>")
  end
  puts(x)
end

def print_source(x)
  puts(x.html_escape)
end

def print_elem(x)
  $stdout.write(x)
end

def pop_stack(s)
  while ! s.empty? do
    print_text s.pop
  end
end

puts '
<html>
  <head>
    <link 
       id="MainStyle" type="text/css" rel="stylesheet" 
       href="sample.css">
    </link>
  <body>
'

for line in lines do

  case state
  when :plain
    case line 
    when /^[ \t]*#\s*(\++)\s+(.*)/
      in_source = false
      pop_stack(stack)
      para_indent = -1
      print_elem "<h#{$1.size}>"
      print_text "#{$2}"
      print_elem "</h" + $1.size.to_s + ">"
    when /^[ \t]*#[ \t]*---[ \t]*$/
      in_source = false
      pop_stack(stack)
      para_indent = -1
      print_elem "<div id=\"code\">"
      state = :code
    when /^[ \t]*#(   \s*)([+*-])(.*)/
      in_source = false
      list_space = $1
      item_content = $3
      list_type = $2
      if (list_space.size / 3) > list_indent then
        if list_type === "*" then
          print_elem "<ul>"
          stack.push("</ul>")
        else
          print_elem "<ol>"
          stack.push("</ol>") 
        end
        list_indent = list_space.size / 3
      else
        if (list_space.size / 3) < list_indent then
          print_elem stack.pop
        end
      end
      print_elem "<li>"
      print_text "#{item_content}"
    when /^[ \t]*#([ \t]*)(.*)$/
      in_source = false
      para_space = $1
      para_content = $2
      if para_space.size / 3 > para_indent then
        print_elem("<p indent=#{para_space.size/3}>")
        stack.push("</p>")
        para_indent = para_space.size/3
      else
        if para_space.size / 3 < para_indent then
          print_elem stack.pop
          print_elem "<p indent=#{para_space.size/3}>"
          stack.push("</p>")
        end
      end
      print_text para_content
    when /^[ \t]*#([ \t]*)$/
      in_source = false
      pop_stack(stack)
      para_indent = -1
      list_indent = 0
    else
      if ! in_source then
        pop_stack(stack)
      para_indent = -1
        print_elem "<div id=\"source\">"
        stack.push "</div>"
        in_source = true
      end
      print_source(line)
    end
  when :code
    case line
    when /^[ \t]*#[ \t]*---[ \t]*$/
      print_elem "</div>"
      state = :plain
      pop_stack(stack)
      para_indent = -1
    else
      print_text line.sub(/^#[ \t]*/,'')
    end
  end

end
      
pop_stack(stack)  
      
puts '
</body> </html>
'
