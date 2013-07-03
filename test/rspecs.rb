require "rspec"
require "comprehend"

describe Array, "#comprehend" do

  it "applies a block to an array" do
    [1,2,3].comprehend{ |i| i.to_s }.should == ["1","2","3"]
  end

  it "excludes nil results" do
    [1,2,3].comprehend{ |i| i.to_s if i<3 }.should == ["1","2"]
  end

  it "does not modify the array" do
    ary = [1,2,3]
    ary.comprehend{ |i| i.to_s if i<3 }
    ary.should == [1,2,3]
  end

end

describe Array, "#comprehend!" do

  it "comprehends an array in place" do
    ary = [1,2,3]
    ary.comprehend! { |i| i.to_s if i<3 }
    ary.should == ["1","2"]
  end

  it "returns the modified array if it's shorter" do
    [1,2,3].comprehend!{ |i| i.to_s if i<3 }.should == ["1","2"]
  end

  it "returns nil if no nils were created by the block" do
    [1,2,3].comprehend!{ |i| i.to_s }.should == nil
  end

  it "can resize a large array to a small one" do
    ary = [*0..1024]
    ary.comprehend!{|i| i.to_s if i<15}
    ary.size.should == 15
  end

  it "can resize elements of an array while transforming it" do
    ary = [*1..100]
    ary.comprehend!{ |i| i**3 unless i==50}
    ary.size.should == 99
    ary.last.should == 100**3
  end

end
