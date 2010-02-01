require 'webrick'
require 'yaml'

class Simple < WEBrick::HTTPServlet::AbstractServlet
  
  def do_GET(request, response)
    response.status = 200
    response['Content-Type'] = "text/html"
    response.body = get_page(request)
  end
  
  def get_page(request)
    r = ""
    r << "<html><title>List of Pages</title></html>\n"
    pages = YAML::load(File.read("tags.txt"))
    for p in pages do
      url = p['href']
      r << "<a href=\"#{url}\">#{url}</a><br>\n"
      r << "Tags: #{p['tag'].join(" ")}<br><br>\n\n"
    end
    r << "</body></html>"
    return r
  end
  
end

server = WEBrick::HTTPServer.new(:Port => 2000)
server.mount "/simple", Simple
server.mount "/pages", Simple

trap("INT"){ server.shutdown }

server.start
