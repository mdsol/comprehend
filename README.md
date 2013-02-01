# comprehend

```ruby
[1,2,3].comprehend{ |i| i.to_s if i<3 } == ["1","2"]

[{:type => :dog, :sound => "woof"},{:type => :tree}, {:type => :cat, :sound => "meow"}].comprehend do |o|
  o[:sound]
end == ["woof", "meow"]
```

Comprehend is a tiny gem that defines `Array#comprehend`
and `Array#comprehend!`.  Comprehend accepts 0 arguments
and requires a block.  It invokes the block for each element
of self and returns an array of all non-nil results.

`comprehend(&block)` is equivalent to `map(&block).compact`, but is slightly faster
because it only iterates once over the array.  For blocks that do not modify the array
in place, the result is also equivalent to `select(&block).map(&block)`.

# Usage

```ruby
require "comprehend"
```

Tested on ruby-mri 1.9.3 and 2.0.0, not compatible with lower versions.

# Why "comprehend"?

Invocations of comprehend will often look like Python list comprehensions.
Compare the first example above to

```python
[str(i) for i in [1,2,3] if i<3] == ['1', '2']
```

# License
Copyright (C) 2013 Medidata Solutions Inc.
 
Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:
 
The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.
 
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
