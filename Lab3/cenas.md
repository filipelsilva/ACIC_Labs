# Project Notes

* Ignore the top half of the intersection
* Period is 20 seconds (10 green, 10 red; with the last one second being yellow)
* For the synchronization: first send our time to west and east, then the rest
  of the algorithm
* `send` function: might need to distinguish between I2C and in-Arduino
  communication (when Arduino controls two junctions)
* Start with one junction per Arduino, then move to two

# Questions

[ ] need to detect failure of the red light -> use analogRead ?
[ ] how to standardize the I2C communication ?
