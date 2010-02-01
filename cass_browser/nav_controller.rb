 
# Copyright 2004, Richard Cole
#
# This file is part of Griff.
# 
# Griff is free software; you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free
# Software Foundation; either version 2 of the License, or (at your
# option) any later version.
# 
# Griff is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
# for more details.
# 
# You should have received a copy of the GNU General Public License
# along with Griff; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

require 'itql_nav_space'


class NavController < Qt::Object

  slots 'focusOnIntent()', 'focusOnExtent()'
  slots 'foldAttribute()', 'unfoldAttribute()'
  slots 'foldObject()', 'unfoldObject()'
  slots 'focusOnAttribute()', 'focusOnObject()'
  slots 'primeCanvas()'

  attr_reader :space

  def foldObject
    @space.fold_object(@object)
    primeCanvas
  end

  def unfoldObject
    @space.unfold_object(@object)
    primeCanvas
  end

  def foldAttribute
    @space.fold_attribute(@object)
    primeCanvas
  end

  def unfoldAttribute
    @space.unfold_attribute(@attribute)
    primeCanvas
  end

  def primeCanvas
    result = @space.context
    @canvas.load_context(result['context'], result['object_interp'], result['attribute_interp'])
  end

  def focusOnIntent
    @space.unfold_attribute(@concept.intent)
    primeCanvas
  end

  def focusOnAttribute
    @space.focus_on_attribute(@attribute)
    primeCanvas
  end

  def focusOnObject
    @space.focus_on_object(@object)
    primeCanvas
  end

  def focusOnExtent
    puts "Focus on extent."
  end

  def initialize canvas
    super
    @canvas = canvas
    @space = ItqlNavSpace.new
  end

  def concept_menu event, concept
    @concept = concept
    menu = PopupMenu.new
    submenu = PopupMenu.new
    submenu.insertItem("Intent", self, SLOT("focusOnIntent()"))
    submenu.insertItem("Extent", self, SLOT("focusOnExtent()"))
    menu.insertItem("Focus On...", submenu)
    menu.exec(event.globalPos)
  end

  def attribute_menu event, attribute
    @attribute = attribute
    menu = PopupMenu.new
    menu.insertItem("Fold", self, SLOT("foldAttribute()"))
    menu.insertItem("UnFold", self, SLOT("unfoldAttribute()"))
    menu.insertItem("Focus On", self, SLOT("focusOnAttribute()"))
    menu.exec(event.globalPos)
  end

  def object_menu event, object
    @object = object
    menu = PopupMenu.new
    menu.insertItem("Fold", self, SLOT("foldObject()"))
    menu.insertItem("UnFold", self, SLOT("unfoldObject()"))
    menu.insertItem("Focus On", self, SLOT("focusOnObject()"))
    menu.exec(event.globalPos)
  end

end

