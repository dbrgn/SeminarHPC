d <- read.csv("results.csv")
d$ln = log10(d$n)
d$lt = log10(d$time)

pdf("results.pdf", 8, 6)
plot(d$ln, d$lt, type = "l", col = "blue",
	main = "Gauss-Algorithmus, OpenMPI-Implementierung",
	xlab = "log10(n)", ylab = "log10(Laufzeit)")
