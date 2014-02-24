d <- read.csv("results.csv")
pdf("results.pdf", 8, 6)
plot(d$n, d$t, type = "l", log = "xy",
	col = "blue",
	main = "Laufzeit Gauss-Algorithmus OpenMP",
	xlab = "n", ylab = "Laufzeit [s]")
u <- read.csv("../c/results.csv");
points(u$n, u$t, col = "red", pch = 20)
grid()

