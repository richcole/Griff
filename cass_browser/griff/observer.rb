require 'set'

module Griff 

  module Observable
    def add_observer(observer)
      if @observers == nil then
        @observers = Set.new
      end
      @observers.add(observer)
    end

    def update
      if @observers then
        for ob in @observers do
          ob.notify(self)
        end
      end
    end
  end

end
