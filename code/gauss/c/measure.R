d <- read.csv("results.csv")

pdf("results.pdf", 8, 6)
d$ln = log10(d$n)
d$lt = log10(d$time)
l <- lm(d$lt ~ d$ln)
l
plot(d$ln, d$lt,
	main = "Laufzeit Gauss-Algorithmus, sequentiell",
	xlab = "log10(n)", ylab = "log10(Laufzeit)",
	pch = 3)
abline(l, col = "red")

u <- read.csv("results-uni.csv")

pdf("results-uni.pdf", 8, 6)
u$ln = log10(u$n)
u$lt = log10(u$time)
l <- lm(u$lt ~ u$ln)
l
plot(u$ln, u$lt,
	main = "Laufzeit Gauss-Algorithmus, sequentiell und unidirektional",
	xlab = "log10(n)", ylab = "log10(Laufzeit)",
	pch = 3)
lines(d$ln, d$lt, col="blue", lw = 2)
abline(l, col = "red", lw = 2)

