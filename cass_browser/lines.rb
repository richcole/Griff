
def crossing(x1, y1, x2, y2, a1, b1, a2, b2)

  x1 = x1.to_f
  y1 = y1.to_f
  x2 = x2.to_f
  y2 = y2.to_f
  a1 = a1.to_f
  b1 = b1.to_f
  a2 = a2.to_f
  b2 = b2.to_f

  if x1 > x2 then
    t = x1
    x1 = x2
    x2 = t
    t = y1
    y1 = y2
    y2 = t
  end

  if a1 > a2 then
    t = a1
    a1 = a2
    a2 = t
    t = b1
    b1 = b2
    b2 = t
  end

  if x2 == x1 then
    return nil if x1 < a1 || x1 > a2
    return nil if a2 == a1
    t = x1 - a1
    m2 = (b2 - b1) / (a2 - a1)
    intercept = b1 + (m2 * t)
    return nil if y2 > y1 && (intercept > y2 || intercept < y1)
    return nil if y2 < y1 && (intercept < y2 || intercept > y1)
    return [x1, intercept]
  end
    
  if a1 == a2 then
    return nil if a1 < x1 || a1 > x2
    return nil if x2 == x1
    t = a1 - x1
    m1 = (y2 - y1) / (x2 - x1)
    intercept = y1 + (m1 * t)
    return nil if b2 > b1 && (intercept > b2 || intercept < b1)
    return nil if b2 < b1 && (intercept < b2 || intercept > b1)
    return [a1, intercept]
  end
    
  m1 = (y2 - y1) / (x2 - x1)
  m2 = (b2 - b1) / (a2 - a1)
  return nil if m1 == m2
  t = ((y1 - b1) - m2*(x1 - a1)) / (m2 - m1)
  s = (x1 - a1) + t

  if t < 0 || s < 0 || x1 + t > x2 || a1 + s > a2 then
    return nil
  else
    return [x1 + t, y1 + (m1 * t)]
  end
end

def p_to_s(p)
  return "(" + p[0].to_s + "," + p[1].to_s + ")"
end




