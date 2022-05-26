# Path-Finder
My first attempt at a path finder

## A*?
This is not A*. I wrote this before I was aware of A* and so it is fully my own impementation as poor as it is, it is mine.

## How it works
This program simply assigns each tile around the current location a "cost" which is just the distance from the "goal", then picks the cheapest square to 
move to. The largest problem with appoach is the chances of becoming stuck in a "Bay". In attempt to surpass this problem every iteration that returns to 
same tile increases the cost of the tile to try and have the tile not returned to. This is a far from optimal solution and I would like to come back and 
rewrite this into A* some day.
