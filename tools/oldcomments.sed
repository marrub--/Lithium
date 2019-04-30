$! {  N
      N
      s/\/\/-+\|\n//
      t m1
      b mE
:m1;  s/  / MARKER/
      h
      s/.+MARKER//
      s/ /-/g
      x
      s/MARKER.+//
      x
      H
      x
      s/ \n-/ --/
      t m2
      b mE
:m2;  N
      N
      s/\/\/\n//g
:mE;  }
