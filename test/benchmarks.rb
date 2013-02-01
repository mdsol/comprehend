require "comprehend"
require "benchmark"

puts "Expensive operation (find the sum of all factors)"
x = [*2..1e4]
Benchmark.bmbm do |reporter|
  reporter.report("comprehend") {x.comprehend { |i| (2...i).select{ |f| i%f==0 }.inject(:+) } }
  reporter.report("compact map") {x.map{ |i| (2...i).select{ |f| i%f==0 }.inject(:+) }.compact}
  reporter.report("inject") {x.inject([]) {|memo,i| (sum = (2...i).select{ |f| i%f==0 }.inject(:+) ) ? memo << sum : memo}}
  reporter.report("select then map") {x.select{ |i| (2...i).select{ |f| i%f==0 }.inject(:+) }.map{ |i| (2...i).select{ |f| i%f==0 }.inject(:+) }}
end

puts "Large array, cheap operation"
x = [*2..5e7]
Benchmark.bmbm do |reporter|
  reporter.report("comprehend") {x.comprehend {|i| i.to_s if i<500}}
  reporter.report("compact map") {x.map{|i| i.to_s if i<500}.compact}
  reporter.report("inject") {x.inject([]) {|memo,i| i<500 ? memo << i.to_s : memo}}
  reporter.report("select then map") {x.select{|i| i.to_s if i<500}.map{|i| i.to_s if i<500}}
end
