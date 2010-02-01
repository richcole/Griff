
def txt_to_html(lines)
  state = :outside
  result = []
  code = ""
  for line in lines do
    line.gsub!(/[ \n\r\f\t]$/,'')
#    $stderr.puts "state=#{state}, line=\"#{line}\"" 
    if state == :inside_code then
      case line
      when '---'
        state = :outside
        result.push({ 
          :type => :pre, :content => [], :text => code, :indent => 0 
        })
      else
        if code.length > 0 then
          code << "\n" << line.gsub('<','&lt;').gsub('>','&gt;')
        else
          code << line.gsub('<','&lt;').gsub('>','&gt;')
        end
      end
    elsif state == :inside_html then
      case line
      when '}}}'
        state = :outside
        result.push({ 
          :type => :html, :content => [], :text => code, :indent => 0 
        })
      else
        if code.length > 0 then
          code << "\n" << line
        else
          code << line
        end
      end
    else
      case line 
      when /\A((\s*>)+)(.*)/
        text = $1 + $3
        quote = $1
        indent = quote.length / 2
        if result.length > 0 && result[-1][:type] == :quote then
          result[-1][:text] << "\n"  << text
        else
          result.push({:type => :quote, :indent => indent, :text => text})
        end
      when '{{{'
        state = :inside_html
        code = ""
      when '---'
        state = :inside_code
        code = ""
      when /\A\s*$/
        if result.length > 0 then
          if result[-1] then
            if result[-1][:content] &&  result[-1][:content][-1] then
              indent = result[-1][:content][-1][:indent]
            else
              indent = result[-1][:indent]+2
            end
          else
            indent = 0
          end
          result.push({ :type => :sep, :indent => indent })
        end
      when /\A(\s*)\*\s+(.*)/
        indent = $1.length
        result.push({ :type => :li, :content => 
            [{:type => :p, :text => $2, :indent => indent+2 }], 
          :indent => indent
        })
      when /\A(\s*)-\s+(.*)/
        indent = $1.length
        result.push({ :type => :li, :content => 
            [{:type => :p, :text => $2, :indent => indent+2 }], 
          :indent => indent
        })
      when /\A(\s*)\+\s+(.*)/
        indent = $1.length
        result.push({ :type => :li, :content => 
            [{:type => :p, :text => $2, :indent => indent+2 }], 
          :indent => indent
        })
      when /\A(\s*)\+\+(\++)\s+(.*)/
        indent = $1.length
        level  = $2.length
        text   = $3
        result.push({ 
          :type => :h, :level => level, :text => text, :indent => indent 
        })
      when /(\s*)(.*)/
        indent = $1.length
        text = $2
        stowed = false
        if result.length > 1 then
          if result[-1][:type] == :p && indent == result[-1][:indent] then
            result[-1][:text] << " " << text
            stowed = true
          elsif result[-1][:content] != nil then
            c = result[-1][:content] 
            if c[-1] && c[-1][:type] == :p && indent == c[-1][:indent] then
              c[-1][:text] << " " << text
              stowed = true
            end
          end
        end
        if ! stowed then
          e = {
            :type => :p, :text => $2, :indent => indent 
          }
          result.push(e)
        end
      end
    end
  end
  return result
end

def update_seps(lst)
  result = []
  prev = nil
  for x in lst do
    if prev != nil then
      if x[:type] == :sep then
        if prev[:type] == :p then
          x[:indent] = prev[:indent]
          result.push(x)
          prev = x
        end
      end
    end
    if x[:type] != :sep then
      result.push(x)
      prev = x
    end
  end

  return result
end

def h_to_s(h)
  s = "{"
  first = true
  for x, y in h do
    if ! first then
      s << ", "
    else
      first = false
    end
    s << x.to_s << "=>" << y.to_s
  end
  s << "}"
  return s
end

def reorder(lst)
  stack = []
  result = []
  for i in 0..(lst.length-1) do
    finished = false
    while ! finished do
      if false then # lst[i][:type] == :sep
        finished = true
      elsif stack.length > 0 then
        if stack[-1][:indent] < lst[i][:indent] then
          if stack[-1][:content] != nil then
            stack[-1][:content].push(lst[i])
            stack.push(lst[i])
          elsif
            stack.push(lst[i])
            result.push(lst[i])
          end
          finished = true
        else 
          while stack.length > 0 && stack[-1][:indent] >= lst[i][:indent] do
            stack.pop
          end
          finished = false
        end
      else
        result.push(lst[i])
        stack.push(lst[i])
        finished = true
      end
    end
  end
  return result
end

def enclose_lists(lst)
  state = :outside
  result = []
  for i in 0..(lst.length-1)
    x = lst[i]
    if x[:content] then
      x[:content] = enclose_lists(x[:content])
    end
    case state
    when :outside
      case x[:type]
      when :li
        ul = {:type => :ul, :content => [x], :indent=> x[:indent]}
        result.push(ul)
        state = :inside
      else
        result.push(x)
      end
    when :inside
      case x[:type]
      when :li
        ul[:content].push(x)
      when :sep
        ul[:content].push(x)
      else
        result.push(x)
        state = :outside
      end
    end
  end
  for x in result
  end
  return result
end

def spaces(indent)
  r = ""
  for i in 1..indent do
    r << " "
  end
  return r
end

def print_markup(file, markup, indent)
  for x in markup do
#    $stderr << "\n" + spaces(indent) + "type=" + x[:type].to_s + " "
    case x[:type]
    when :sep
#      file << "#{spaces(indent)}<p>"
#      file << "\n"
      nil
    when :quote
      file << "<pre>" << x[:text] << '</pre>'
    when :p
      file << "#{spaces(indent)}<p>#{x[:text]}</p>"
      file << "\n"
    when :pre
      file << "#{spaces(indent)}<pre>\n#{x[:text]}\n</pre>"
      file << "\n"
    when :html
      file << "#{x[:text]}\n"
    when :h
#      $stderr << (" text=" + x[:text].to_s)
      file << "#{spaces(indent)}<h#{x[:level]}>#{x[:text]}</h#{x[:level]}>"
      file << "\n"
    when :li
      file << "#{spaces(indent)}<#{x[:type]} indent=#{x[:indent]}>"
      print_markup(file, x[:content], indent+2)
      file << "#{spaces(indent)}</#{x[:type]}>"
      file << "\n"
    else
      file << "#{spaces(indent)}<#{x[:type]} indent=#{x[:indent]}>"
      file << "\n"
      print_markup(file, x[:content], indent+2)
      file << "#{spaces(indent)}</#{x[:type]}>"
      file << "\n"
    end
  end
end

def subtext(txt)
  txt.gsub!(/([^\[])(http:\/\/[^ ]*)/,'\1<a href="\2">\1</a>')
  txt.gsub!(/<<([^>|]+)\|([^>]+)>>/,'<image src="\1" align="\2" style="margin: 10px">')
  txt.gsub!(/<<([^>|]+)>>/,'<a href="\1"><image src="\1" style="width: 36em" align="center"></a>')
  txt.gsub!(/\[([^\]]*)\]\[([^\]]*)\]/,'<a href="\2">\1</a>')
  txt.gsub!(/\*([^*]+)\*/,'<i>\1</i>')
  return txt
end

def suballtext(lst)
  for x in lst
    if x[:type] == :p then
      x[:text] = subtext(x[:text])
    end
    if x[:content] then
      x[:content] = suballtext(x[:content])
    end
  end
end

def markup_file(filename)
  File.open(filename) { 
    |file|
    markup = txt_to_html(file.readlines())
#    markup = update_seps(markup)
    markup = reorder(markup)
    markup = enclose_lists(markup)
    suballtext(markup)
    File.open(filename.gsub(/.txt$/,'.html'), 'w') { 
      |file|
      str = ""
      print_markup(str, markup, 0)
      preamble = File.read('preamble.html')
      postamble = File.read('postamble.html')
      title = `grep +++ #{filename} | head -1 | sed -e 's/^\++//'`.chomp
      preamble.gsub!('%PAGETITLE%', title)
      preamble.gsub!('%INDEXPAGE%', File.read('contents.html'))
      preamble.gsub!('%CONTENT%', str)
      file.write(preamble)
      file.write(postamble)
    }
  }
end

def minimal_markup(filename)
  File.open(filename) { 
    |file|
    markup = txt_to_html(file.readlines())
#    markup = update_seps(markup)
    markup = reorder(markup)
    markup = enclose_lists(markup)
    suballtext(markup)
    File.open(filename.gsub(/.txt$/,'.html'), 'w') { 
      |file|
      print_markup(file, markup, 0)
    }
  }
end

if ARGV.size == 0 then
  minimal_markup('contents.txt')
end

exclude = [
  'contents.txt',
  'index_list.txt'
]

File.open('index_list.txt', 'w') {
  |index_list|
  index_list << "+++ List of all Pages\n\n"
  Dir.glob("*.txt").each { |filename| 
    
    if ARGV.size >= 1 then
      do_file = (filename == ARGV[0])
    else
      do_file = true
    end
#    $stderr.puts "do_file=#{do_file}, ARGV.size=#{ARGV.size}, #{ARGV[0]}"

    if ! exclude.include?(filename) then
      if do_file then 
        markup_file(filename) 
      end
      htmlname = filename.gsub(/.txt$/,'.html')
      for title in `grep +++ #{filename}`.split("\n")
        title.chomp!
        title.sub!("+++", "")
        title.gsub!('+', "  ");
        title.gsub!(/\A(\s*)/,'\1 - [')
        index_list << "#{title}][#{htmlname}]"
        index_list << "\n"
      end
    end
  }
}

if ARGV.size == 0 then
  markup_file('index_list.txt')
end


        
