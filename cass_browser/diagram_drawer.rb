
def draw_lattice_diagram(lattice, diagram, diagram_filename, image_list)

  painter = draw_lattice_diagram_content(lattice, diagram, diagram_filename, image_list)
  painter.draw(image_list)
end

def draw_lattice_diagram_with_metrics(lattice, diagram, diagram_filename, image_list, stats, lex) 

  stats_text = 
    lex.indexes.to_a.map { |index| stats.fields[lex.word(index)] }.join(',')
  painter = draw_lattice_diagram_content(lattice, diagram, diagram_filename, image_list)
  painter.stroke('transparent')
  painter.font_family('Times')
  painter.pointsize(24)
  
  painter.font_style(Magick::NormalStyle)
  puts "stats_text=#{stats_text}"
  painter.text(20,40,'"' + stats_text + '"')
  painter.draw(image_list)
end

def draw_lattice_diagram_content(lattice, diagram, diagram_filename, image_list)
  for x in lattice do
    p = diagram.points[x]
    r = Rect.new(p.x, p.y, 0, 0) if r == nil
    r = r.includePoint(diagram.points[x])
  end
  
  r = Rect.new(0,0,0,0) if r == nil
  r = r.expand(50)

  name = '"' + diagram_filename.sub(/^.*?[\/#]/,'').gsub('.diagram','') + '"'
  image_list.new_image(r.width, r.height)
  painter = Magick::Draw.new
  
  painter.stroke('transparent')
  painter.font_family('times')
  painter.pointsize(24)
  
  painter.font_style(Magick::NormalStyle)
  painter.text(20,20,name)
  
  painter.fill_opacity(1)
  for x in lattice do
    p = diagram.points[x]
    x = p.x - r.left
    y = p.y - r.top
    painter.circle(x, y, x+5, y)
  end
  
  painter.stroke('black')
  painter.stroke_width(2)
  painter.stroke_opacity(1)
  for x in lattice do
    for y in lattice.lower_covers[x] do
      px = diagram.points[x]
      py = diagram.points[y]
      painter.line(px.x - r.left, px.y - r.top, py.x - r.left, py.y - r.top)
    end
  end

  return painter

end

def draw_lattice_diagram_test(lattice, diagram, diagram_filename, image_list)

  image_list.new_image(500, 500)
  painter = Magick::Draw.new
  
  painter.stroke('transparent')
  painter.font_family('times')
  painter.pointsize(24)
  
  painter.font_style(Magick::NormalStyle)
  painter.text(20,40,'HelloWorld')
  
  painter.draw(image_list)
end
