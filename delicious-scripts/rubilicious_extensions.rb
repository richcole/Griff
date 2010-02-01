require 'rubygems'
require_gem 'Rubilicious'

class Rubilicious
  def full_posts(tag)
    args = [(tag ? "tag=#{tag.uri_escape}" : nil)]
    get('posts/all?' << args.compact.join('&amp;'), 'post').map do |e|
      e['time'] = Time::from_iso8601(e['time'])
      e['tag'] = e['tag'].split(/\s/)
      e
    end
  end

  def post_by_url(url)
    args = [(url ? "url=#{url.uri_escape}" : nil)]
    get('posts/get?' << args.compact.join('&amp;'), 'post').map do |e|
      e['time'] = Time::from_iso8601(e['time'])
      e['tag'] = e['tag'].split(/\s/)
      e
    end
  end

  def all_posts(tag=nil)
    args = [(tag ? "tag=#{tag.uri_escape}" : nil)]
    get('posts/all?' << args.compact.join('&amp;'), 'post').map do |e|
      e['time'] = Time::from_iso8601(e['time'])
      e['tag'] = e['tag'].split(/\s/)
      e
    end
  end

end

