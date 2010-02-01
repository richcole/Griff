require 'cass_browser_main_window'

include Qt

app = Application.new(ARGV)
main_window = CassBrowserMainWindow.new(nil, "Canvas")
app.mainWidget = main_window
main_window.show()

main_window.load_file("b3.cxt")

app.exec()
