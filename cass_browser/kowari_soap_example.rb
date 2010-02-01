require 'soap/wsdlDriver'
require 'rexml/document'

class ITQLService

  def initialize
    @wsdl_url = 
      "http://localhost:8080/webservices/services/ItqlBeanService?wsdl"
    @soap = SOAP::WSDLDriverFactory.new(@wsdl_url).createDriver
  end

  def query(query_string)
    puts "Before result"
    result = @soap.executeQueryToString(query_string)
    puts "Class: " + result.class.to_s
    return REXML::Document.new(result)
  end

end

def testItqlService
  itql = ITQLService.new
  query = File.read('/home/rcole/kowari/example-1.itql')
  query.gsub!('localhost',`hostname`.chop())
  puts "Query: " + query
  result = itql.query(query)
  result.write($stdout, 2)
  
rescue SOAP::FaultError => fault
  puts "Fault: " + fault.faultstring.to_s
rescue Exception => exception
  puts "Exception Class: " + exception.class.to_s
  puts "Exception Description: " + exception.to_s
end

testItqlService
