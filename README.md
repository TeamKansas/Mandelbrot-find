# Mandelbrot-find 
How it works:
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
This program does not compress any of the data, so the output is quite a large file.


Mandelbrot Set description:
If you do not know what the Mandelbrot set is or how it works, here is a brief description.
The Mandelbrot set is a set of points (painted black on most programs including this one), 
that never leave the circle of radius 2 centered at the origin when operated on infinite times.
The operation is a complex operation: 

    Z -> Z^2 + C

Where C is the original number (point) and Z is the new number. To determine if a point is in the
set, you do this operation infinite times. If it ever leaves the circle, it is not in the set, and 
is colored according to how many times it took to leave the circle (iterations). Thus, the more 
colors there are, the more interesting it is, right? So the higher variety of iterations within a 
specific view of the set, the more interesting it is.
This program obviously does not iterate infinite times, but a finite default of 3000 iterations. 
This produces a good approximation of the set. In real coordinates the iteration looks like this:

    (zX, zY) -> (zX^2 - zY^2 + cX, 2 * zX*zY + cY)

where (zX, zY) is the iterated point and (cX, cY) is the original point.
