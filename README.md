# Mandelbrot-find
This program finds an 'interesting' point on the Mandelbrot set and then creates an image of that spot. 
'Interesting' in this context is a spot that has a wide variety of iterations required to leave the Radius 
2 circle. In order to find spots with high variety, the program samples several points within the current 
viewpoint, called 'frame' in this program. Note that variables called frame denote the frame radius, and the 
center point is typically (x, y), (a, b) or (focus[0], focus[1]). After sampling several points (about 1600 
in this program), it calculates the minimum and the average. The difference between the average and minimum 
denotes how 'interesting' that frame is. The program will do this for several frames within the current frame
and select the most interesting frame of those evaluated. This works because most of the time there are going 
to be more points close to the minimum than close to the maximum, unless it is a spot in the set (painted black).
Now this works, but produces similar results every time because only the spots with the absolute most variety
it could find will be selected. To fix this, the program restricts each output with a random modulo between 10 
and 30, and this lets frames with a smaller variety of iterations to be selected occasionally. After this is 
done it changes the frame radius by 0.5 and repeats multiple times. The last frame selected is the frame 
displayed on the output .tiff file
