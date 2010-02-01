require 'qtsarl'

Qt.debug_level = Qt::DebugLevel::High

context = Qt::SarlContext.new
context.insert(1,1)
context.insert(1,2)
context.insert(1,3)

diagram = Qt::SarlDiagram.new(context)

st = Qt::SarlStringTable.new
st.set(1, "one")
st.set(2, "two")
st.set(3, "three")

a = Qt::Application.new(ARGV)
puts "Calling intializer"
hello = Qt::SarlLatticeView.new(diagram, st, st, 'Hello World!', nil)
hello.resize(100, 30)
a.setMainWidget(hello)
hello.show()
a.exec()

