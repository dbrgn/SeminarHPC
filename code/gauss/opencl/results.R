# read the data of OpenCL measurements
d <- read.csv("results.csv")

# read the comparison data from the unaccelerated implementation
u <- read.csv("../c/results.csv")

# create a comparison graph for all openCL oimplementations
pdf("results.pdf", 8, 6)

nvidiat = d[d$platform == "nvidia" & d$vectorlength == 16,]$time
nvidian = d[d$platform == "nvidia" & d$vectorlength == 16,]$n

amdt    = d[d$platform == "amd"    & d$vectorlength == 16,]$time
amdn    = d[d$platform == "amd"    & d$vectorlength == 16,]$n

intelt  = d[d$platform == "intel"  & d$vectorlength == 16,]$time
inteln  = d[d$platform == "intel"  & d$vectorlength == 16,]$n

plot(u$n, u$time, log = "xy", 
	main = "Laufzeit in Abhängigkeit von der Problemgrösse",
	xlab = "Problemgrösse n", ylab = "Laufzeit [s]",
	sub = "Nvidia OpenCL (rot), Intel OpenCL (blau), AMD OpenCL (grün)",
	ylim = c(0.00004, 1000))
grid()
lines(nvidian, nvidiat, type = "l", col = "red")
lines(amdn,    amdt,    type = "l", col = "blue")
lines(inteln,  intelt,  type = "l", col = "green")

# create a grpahi showing vector size dependency

vectordependency <- function(platformname) {
	filename = sprintf("vector-%s.pdf", platformname)
	pdf(filename, 8, 6)
	t = d[(d$platform == platformname) & (d$n >= 500)
		& (d$n <= 1000),]$time
	t2 = d[(d$platform == platformname) & (d$vectorlength == 4)
		& (d$n >= 500) & (d$n <= 1000),]$time
	v = d[(d$platform == platformname) & (d$n >= 500)
		& (d$n <= 1000),]$vectorlength
	n = d[(d$platform == platformname) & (d$n >= 500)
		& (d$n <= 1000),]$n
	subtitle = sprintf("Laufzeit auf %s für verschiedene Vektorlängen",
		platformname)
	plot(v, t / t2, log = "x",
		xlab = "Vektorlänge", ylab = "Laufzeit (standardisiert)",
		main = "Standardisierte Laufzeit",
		sub = subtitle)
	grid()
}

vectordependency("amd")
vectordependency("intel")
vectordependency("nvidia")

