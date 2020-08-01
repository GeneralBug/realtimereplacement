Interactive 3D Assignment 1
Harrison Ellem, s3718372

Notes
 - = and - change segment_count, while + and _ change the wave thing (hold shift)
 - Tesselation  below 16 breaks the barrels, as they use x-coordinates to determine their y-values. 
 Could be fixed by widening the range, but that would made the floating less accurate, so I've left it as is.
 - No idea how to fill in sine wave, probably integration.
 - I'm not sure what is meant by magnitude; I've capped the y-component of the velocity, but x is still infinite.
 - Also, I wasn't sure about the "stop animation" button, so it just stops rendering the scene until you push it again.
 