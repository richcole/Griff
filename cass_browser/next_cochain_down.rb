require 'formal_context'

def intersection(a, b)
  if a == nil then
    b.dup
  else
    a.and(b)
  end
end

def implied_attributes(context, intent)
  implied = nil
  for g in context.objects do
    g_dash = context.object_intent(g)
    if intent.subseteq?(g_dash) then
      implied = intersection(implied, g_dash)
    end
  end
  if implied == nil then
    implied = context.attributes.dup
  end
  return implied
end

def next_cochain_down(context, intent)

  h = Hash.new
  for g in context.objects do
    g_dash = context.object_intent(g)
    if intent.subseteq?(g_dash) then
      g_dash_minus_intent = g_dash.minus(intent)
      for m in g_dash_minus_intent do
        h[m] = intersection(h[m], g_dash_minus_intent)
      end
    end
  end
    
  lower_covers = make_default_set()
  for m, phi_m in h do
    is_m_ok = true
    for n in phi_m do
      phi_n = h[n]
      if phi_n == nil || ! phi_n.include?(m) then
        is_m_ok = false
        break
      end
    end
    if is_m_ok then
      lower_covers.add(phi_m)
    end
  end
  
  return lower_covers
  
end
