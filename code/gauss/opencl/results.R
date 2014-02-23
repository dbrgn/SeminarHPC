d <- read.csv("results.csv")

u <- read.csv("../c/results.csv")
cn = u$n
ct = u$time

pdf("results.pdf", 8, 6)

nvidiat = d[d$platform == "nvidia" & d$vectorlength == 16,]$time
nvidian = d[d$platform == "nvidia" & d$vectorlength == 16,]$n
amdt = d[d$platform == "amd" & d$vectorlength == 16,]$time
amdn = d[d$platform == "amd" & d$vectorlength == 16,]$n
intelt = d[d$platform == "intel" & d$vectorlength == 16,]$time
inteln = d[d$platform == "intel" & d$vectorlength == 16,]$n

plot(cn, ct, log = "xy", 
	main = "Laufzeit in Abhängigkeit von der Problemgrösse",
	xlab = "Problemgrösse n", ylab = "Laufzeit",
	sub = "Nvidia OpenCL (rot), Intel OpenCL (blau), AMD OpenCL (grün)",
	ylim = c(0.00004, 1000))

lines(nvidian, nvidiat, type = "l", col = "red")
lines(amdn, amdt, type = "l", col = "blue")
lines(inteln, intelt, type = "l", col = "green")

vectordependency <- function(platformname) {
	filename = sprintf("vector-%s.pdf", platformname)
	pdf(filename, 8, 6)
	t = d[(d$platform == platformname),]$time
	t2 = d[(d$platform == platformname) & (d$vectorlength == 4),]$time
	v = d[(d$platform == platformname),]$vectorlength
	n = d[(d$platform == platformname),]$n
	subtitle = sprintf("Laufzeit auf %s für verschiedene Vektorlängen",
		platformname)
	plot(v, t/t2, log = "x",
		xlab = "Vektorlänge", ylab = "Laufzeit (standardisiert)",
		main = "Standardisierte Laufzeit",
		sub = subtitle)
}

vectordependency("amd")
vectordependency("intel")
vectordependency("nvidia")

