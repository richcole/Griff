require 'test/unit'
require 'lines'

class TC_MyTest < Test::Unit::TestCase
  # def setup
  # end
  
  # def teardown
  # end
  
  def test_examples
    assert_equal([0.0, 0.0], crossing(0, 0, 1, 1, 0, 0, 1, 2))
    assert_equal([1.0, 1.0], crossing(0, 0, 1, 1, 1, 1, 1, 2))
    assert_equal([1.0, 1.0], crossing(1, 1, 0, 0, 1, 1, 1, 2))
    assert_equal([1.0, 1.0], crossing(1, 1, 0, 0, 1, 2, 1, 1))
    assert_equal(nil, crossing(1, 1, 1, 1, 1, 2, 1, 1))
    assert_equal(nil, crossing(0, 0, 2, 2, 8, 0, 0, 8))
    assert_equal(nil, crossing(-150, 150, 200, 400, 0, 0, 0, 150))
    assert_equal(nil, crossing(200, 400, 200, 550, 0, 0, 350, 250))
  end
end
