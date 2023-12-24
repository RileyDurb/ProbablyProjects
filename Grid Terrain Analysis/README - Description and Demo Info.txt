Project: Terrain Analysis
The featured project here is various functions that analyze a 2D grid, and then color it to show that information
For example, one of the functions calculates the general level of visibility of each square, and colors a square
a shade of orange that is darker the more visible it is from other tiles. Then, the idea here is that it can be used
to inform AI decisions, and other things, if utilized.

Main code file:
P3_TerrainAnalysis.cpp

Included is a project demo, utilizing a provided engine, and feel free to run the Grid terrain Analysis executable
if you'd like to see it.

Controls:
It's mainly playing around with buttons as you'd like, but here is a more detailed description:

- On the right side are buttons for the different analysis types. Feel free to play around, and some, like propagation,
require clicking the map as well.
- There is a button to switch maps on the upper right
- On the left there are sliders that adjust different factors about the analysis
- This demo also contains 2 other projects, an example situation made from behaviour trees, and A* pathfinding, which can be accessed
by pressing f1 and f2, respectively. f3 returns to the terrain analysis project, and code for the other projects is available in the source folder.