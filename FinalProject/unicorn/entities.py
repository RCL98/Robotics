from math import cos, sin, radians
import pprint, re, textwrap

class Entity:
	def get_gcode(self,context):
		#raise NotImplementedError()
		return "NIE"

class Line(Entity):
	def __str__(self):
		return "Line from [%.2f, %.2f] to [%.2f, %.2f]" % (self.start[0], self.start[1], self.end[0], self.end[1])
	def get_gcode(self,context):
		"Emit gcode for drawing line"
		context.codes.append("(" + str(self) + ")")
		context.go_to_point(self.start[0],self.start[1])
		context.draw_to_point(self.end[0],self.end[1])
		context.codes.append("")

class Circle(Entity):
	def __str__(self):
		return "Circle at [%.2f,%.2f], radius %.2f" % (self.center[0], self.center[1], self.radius)
	def get_gcode(self,context):
		"Emit gcode for drawing arc"
		start = (self.center[0] - self.radius, self.center[1])
		arc_code = "G3 I%.2f J0 F%.2f" % (self.radius, context.xy_feedrate)

		context.codes.append("(" + str(self) + ")")
		context.go_to_point(start[0],start[1])
		context.start()
		context.codes.append(arc_code)
		context.stop()
		context.codes.append("")

class Arc(Entity):
	def __str__(self):
		return "Arc at [%.2f, %.2f], radius %.2f, from %.2f to %.2f" % (self.center[0], self.center[1], self.radius, self.start_angle, self.end_angle)

	def find_point(self,proportion):
		"Find point at the given proportion along the arc."
		delta = self.end_angle - self.start_angle
		angle = self.start_angle + delta*proportion
		
		return (self.center[0] + self.radius*cos(angle), self.center[1] + self.radius*sin(angle))

	def get_gcode(self,context):
		"Emit gcode for drawing arc"
		start = self.find_point(0)
		end = self.find_point(1)
		delta = self.end_angle - self.start_angle

		if (delta < 0):
			arc_code = "G3"
		else:
			arc_code = "G3"
		arc_code = arc_code + " X%.2f Y%.2f I%.2f J%.2f F%.2f" % (end[0], end[1], self.center[0] - start[0], self.center[1] - start[1], context.xy_feedrate)

		context.codes.append("(" + str(self) + ")")
		context.go_to_point(start[0],start[1])
		context.last = end
		context.start()
		context.codes.append(arc_code)
		context.stop()
		context.codes.append("")
        
class Ellipse(Entity):
        #NOT YET IMPLEMENTED
	def __str__(self):
		return "Ellipse at [%.2f, %.2f], major [%.2f, %.2f], minor/major %.2f" + " start %.2f end %.2f" % \
		(self.center[0], self.center[1], self.major[0], self.major[1], self.minor_to_major, self.start_param, self.end_param)

hex_to_decimal = {
    "0": 0,
    "1": 1,
    "2": 2,
    "3": 3,
    "4": 4,
    "5": 5,
    "6": 6,
    "7": 7,
    "8": 8,
    "9": 9,
    "a": 10,
    "b": 11,
    "c": 12,
    "d": 13,
    "e": 14,
    "f": 15
}

hexBasicColorValues = [0, 128, 192, 255]

basicColorsMap = {
	(0, 0, 0): "black",
	(255, 255, 255): "white",
	(255, 0, 0): "red",
	(0, 255, 0): "green",
	(0, 0, 255): "blue",
	(255, 255, 0): "yellow",
	(0, 255, 255): "cyan",
	(255, 0, 255): "magenta",
	(192, 192, 192): "gray",
	(128, 0, 0): "maroon",	
	(128, 0, 128): "purple"
}


def closest(lst, K):  
    return lst[min(range(len(lst)), key = lambda i: abs(lst[i]-K))] 

def colorMap(hexValue):
	hexValue = hexValue[1:]
	values = textwrap.wrap(hexValue,2)
	red = 16* hex_to_decimal[values[0][0]] + hex_to_decimal[values[0][1]]
	green = 16 * hex_to_decimal[values[1][0]] + hex_to_decimal[values[1][1]]
	blue = 16* hex_to_decimal[values[2][0]] + hex_to_decimal[values[2][1]]
	red = closest(hexBasicColorValues, red)
	green = closest(hexBasicColorValues, green)
	blue = closest(hexBasicColorValues, blue)
	color = basicColorsMap.get((red, green, blue))
	if color is None:
		return "black"
	return color
	
basicColorsToCodeMap = {
	"black": "10",
	"white": "20",
	"red": "30",
	"green": "40",
	"blue": "50",
	"yellow": "60",
	"cyan": "70",
	"magenta": "80",
	"gray": "90",
	"maroon": "100",	
	"purple": "110"
}

class PolyLine(Entity):
	def __str__(self):
		return "Polyline consisting of %d segments." % len(self.segments)

	def get_gcode(self,context):
		"Emit gcode for drawing polyline"
		if hasattr(self, 'segments'):
			for points in self.segments:
				start = points[0]
				context.codes.append("(" + str(self) + ")")
				context.go_to_point(start[0],start[1])
				subs = re.findall("#[0-9A-Fa-f]*",self.pathStyle)
				if len(subs) == 2:
					color = colorMap(subs[1])
				elif len(subs) == 1:
					color = colorMap(subs[0])
				else: 
					color = "black"
				colorCommand = "C " + basicColorsToCodeMap.get(color) + " (" + color + ")" 
				context.codes.append(colorCommand)
				context.start()
				for point in points[1:]:
					context.draw_to_point(point[0],point[1])
					context.last = point
				context.stop()
				context.codes.append("")

