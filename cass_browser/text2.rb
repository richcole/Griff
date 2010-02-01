require 'RMagick'

  image_list = Magick::ImageList.new
  image_list.new_image(500, 500)
  painter = Magick::Draw.new
  
  painter.stroke('transparent')
  painter.font_family('times')
  painter.pointsize(24)
  
  painter.font_style(Magick::NormalStyle)
  painter.text(20,40,'Hello World')
  
  painter.draw(image_list)
  image_list.write('text.png')
