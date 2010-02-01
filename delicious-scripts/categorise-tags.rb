require 'net/http'
require 'cgi'
require 'rexml/document'

class DelServer

  include Net
  include REXML 

  attr_accessor :server

  def initialize
    @proxy_addr = "proxy"
    @proxy_port = 80
    @proxy = HTTP::Proxy(@proxy_addr, @proxy_port)
    @auth = File.read("#{ENV["HOME"]}/.delicious/auth").split(":")
    puts "auth=#{@auth.join(":")}"
  end

  def get(uri)
    uri = URI.parse(uri)
    resp = nil
    @proxy.start(uri.host) do
      |http|
      puts "Get: uri=#{uri}, path=#{uri.path} query=#{uri.query} " + 
        "frag=#{uri.fragment}"
      req = Net::HTTP::Get.new(uri.path)
      req.basic_auth(@auth[0], @auth[1])
      puts "Request: #{req} user=#{@auth[0]}, pass=#{@auth[1]}"
      puts "  authorization: #{req['authorization']}"
      resp = http.request(req)
    end
    if resp != nil then
      if resp.code == 500 then
        return resp.body
      else
        puts "Reponse: #{resp.message}"
      end
    else
      puts "Empty response"
    end
    return nil
  end

  def get_doc(uri)
    page = get(uri)
    puts "Page: #{page}"
    return page ? Document.new(page) : nil
  end

  def get_tags
    get_doc("http://del.icio.us/api/tags/get?")
  end

end

server = DelServer.new
puts server.get_tags.to_s
