d <- read.csv("results.csv")
d$ln = log10(d$n)
d$lt = log10(d$time)

u <- read.csv("../c/results.csv")
u$ln = log10(u$n)
u$lt = log10(u$time)

pdf("results.pdf", 8, 6)
plot(d[d$threads == 32,]$ln, d[d$threads == 32,]$lt, type = "l",
	main = "Laufzeit Gauss Pthread-Implementation",
	xlab = "log10(n)", ylab = "log10(Laufzeit)" )
lines(d[d$threads == 16,]$ln, d[d$threads == 16,]$lt)
lines(d[d$threads == 12,]$ln, d[d$threads == 12,]$lt)
lines(d[d$threads == 8,]$ln, d[d$threads == 8,]$lt)
lines(d[d$threads == 4,]$ln, d[d$threads == 4,]$lt)
lines(d[d$threads == 2,]$ln, d[d$threads == 2,]$lt)
lines(d[d$threads == 1,]$ln, d[d$threads == 1,]$lt)
points(u$ln, u$lt, col = "red", pch = 20)
