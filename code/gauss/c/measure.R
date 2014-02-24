d <- read.csv("results.csv")

pdf("results.pdf", 8, 6)
d$ln = log10(d$n)
d$lt = log10(d$time)
l <- lm(d$lt ~ d$ln)
l
plot(d$n, d$t, log = "xy",
	main = "Laufzeit Gauss-Algorithmus, sequentiell",
	xlab = "n", ylab = "Laufzeit [s]",
	pch = 3)
grid()
abline(l, col = "red")

u <- read.csv("results-uni.csv")

pdf("results-uni.pdf", 8, 6)
u$ln = log10(u$n)
u$lt = log10(u$time)
l <- lm(u$lt ~ u$ln)
l
plot(u$n, u$t, log = "xy",
	main = "Laufzeit Gauss-Algorithmus, sequentiell und unidirektional",
	xlab = "n", ylab = "Laufzeit [s]",
	pch = 3)
grid()
lines(d$n, d$t, col="blue", lw = 2)
abline(l, col = "red", lw = 2)

