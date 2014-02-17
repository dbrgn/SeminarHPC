d <- read.csv("results.csv")
d$ln = log10(d$n)
d$lt = log10(d$time)
pdf("results.pdf", 8, 6)
plot(d$ln, d$lt, type = "l",
	col = "blue",
	main = "Laufzeit Gauss-Algorithmus OpenMP",
	xlab = "log10(n)", ylab = "log10(Laufzeit)")
u <- read.csv("../c/results.csv");
u$ln = log10(u$n)
u$lt = log10(u$time)
points(u$ln, u$lt, col = "red", pch = 20)

