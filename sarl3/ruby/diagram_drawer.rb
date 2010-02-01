require 'rvg/rvg'
require 'sarl'
require 'cxt_file_format'
require 'geom'

include Magick
include Sarl
$diagram_scale = 30

def draw_lattice_diagram(diagram)

  lattice = diagram.lattice
  r = nil

  for c in lattice do
    p = diagram.get(c)
    r = Geom::Rect.new(p.x / $diagram_scale, p.y / $diagram_scale, 0, 0) if r == nil
    r = r.includePoint(Geom::Point.new(p.x / $diagram_scale, p.y / $diagram_scale))
  end
  
  r = Rect.new(0,0,0,0) if r == nil
  r = r.expand(15)

  RVG::dpi = 72
  puts "Rect=#{r}"
  rvg = RVG.new(r.width.pt, r.height.pt).viewbox(-r.left,-r.top,r.width,r.height) {
    |canvas|
    canvas.background_fill = 'white'

    for a in lattice do
      pa = diagram[a]
      canvas.g.translate(pa.x / $diagram_scale, pa.y / $diagram_scale).styles(:fill=>'yellow', :stroke=>'black', :stroke_width=>2) {
        |body|
        for b in lattice.lower_covers[a] do
          pb = diagram[b]
          body.line(pb.x / $diagram_scale - pa.x / $diagram_scale, pb.y / $diagram_scale - pa.y / $diagram_scale)
        end
      }
    end

    for a in lattice do
      p = diagram[a]
      puts "p=(#{p.x / $diagram_scale}, #{p.y / $diagram_scale})"
      canvas.g.translate(p.x / $diagram_scale, p.y / $diagram_scale).styles(:fill=>'yellow', :stroke=>'black', :stroke_width=>2) {
        |body|
        body.ellipse(5, 5)
      }
    end
  }

  return rvg
      
end

def draw_lattice_diagram_test()
  image_list = ImageList.new
  ic = interp_context_from_cxt('/home/rcole/Source/griff/flashview/contexts/freese2.cxt')
  lattice = Lattice.new(ic.context)
  diagram = Diagram.layout(lattice)
  rvg = draw_lattice_diagram(diagram) 
  rvg.draw.write("test.png")
end

def sort_diagrams(diagrams, metrics)
  maxes = []
  for m in metrics do
    puts "metrics: " + m.join(", ")
    for mi in m.range do
      maxes[mi] = max_of(maxes[mi], m[mi])
    end
  end
  
  diags = []
  for i in diagrams.range do
    d = diagrams[i]
    m = metrics[i]
    ismax = []
    score = 0
    for im in m.range do
      ismax[im] = (m[im] == maxes[im]) ? 1 : 0
      if ismax[im] == 1 then
        score += 1
      end
    end

    diags << [score, d]
  end

  return diags.sort { |x,y| - (x[0] <=> y[0]) } . map { |x| x[1] }
end

def draw_lattice_diagrams(path)

  File.open(path.gsub(/contexts/,'diagrams') + "/index.html", 'w') { 
    |file|
    file.puts "<html><body>"
    File.readlines(path + "/context_list.txt").each { 
      |cxtname|
      cxtname.chomp!
      if cxtname != "" then
      filename = path + "/" + cxtname + ".cxt"
      puts "** Processing: #{filename}"
      ic = interp_context_from_cxt(filename)
      lattice = Lattice.new(ic.context)
      if ( lattice.count <= 40 ) then
        metrics = []
        diagrams = Diagram.layout_array(lattice, metrics)
        diagrams = sort_diagrams(diagrams, metrics)
        name = "Unknown"
        if filename =~ /.*contexts\/+(.*)/ then
          name = $1
        end
        file.puts "<hr> #{name} <p>"
        for i in 0..min_of(diagrams.size - 1, 40) do
          rvg = draw_lattice_diagram(diagrams[i]) 
          img_name = filename.gsub(/contexts/,'diagrams').gsub(/.cxt/,"-#{i+1}.png")
          rvg.draw.write(img_name)
          file.puts "<img src=\"#{img_name}\">"
        end
        file.puts "</p>"
      end
      end
    }
    file.puts "</html></body>"
  }
end

draw_lattice_diagrams("/home/rcole/Source/griff/flashview/contexts")

