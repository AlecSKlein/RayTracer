RayTrace Pt 2

Alec Klein & Ben Ammer

Ray Trace part 2 implements all existing code from part 1 adding textures, shadows, and reflection.

Textures are looked up in the LeafNode for each object, and the texture location is found in both their S and T
coordinates by using the point of intersection with the ray and the object. The shader in Scenegraph uses the hit
record to set the color based on the texture coordinates found in intersect.

Shadows are done by implementing the intersect function on rays tracing from the intersect location to the light. If
and object is found to be inbetween the light and the intersection point it is not shaded, and will be set to black.

Reflections are done in the raycast function. If an object is found to be reflective it will use the normals found
in the hit record to calculate the reflection of the oncoming ray. If the ray intersects with another object it will
add the fraction of its reflection defined in the xml file to the color of the intersected object. This function works
recursively, but we have set a limit so it never reflects infinitely.

The scene we set up contained a fully reflective box backdrop, two spheres (one with a texture and one with a portion of
it reflective), and one box as a floor that is not reflective, but contains a texture. There are three lights, one
between the backdrop and the spheres, one above the spheres, off center of the floor towards the spheres, and one
across from the backdrop, to allow the back side of the spheres to be seen. This creates 0 absolute dark shadows.