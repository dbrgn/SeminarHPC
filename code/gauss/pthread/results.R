d <- read.csv("results.csv")
d$ln = log10(d$n)
d$lt = log10(d$time)

u <- read.csv("../c/results.csv")
u$ln = log10(u$n)
u$lt = log10(u$time)

pdf("results.pdf", 8, 6)
plot(d[d$threads == 32,]$n, d[d$threads == 32,]$time, type = "l", log = "xy",
	main = "Laufzeit Gauss Pthread-Implementation",
	xlab = "n", ylab = "Laufzeit" )
lines(d[d$threads == 16,]$n, d[d$threads == 16,]$time)
lines(d[d$threads == 12,]$n, d[d$threads == 12,]$time)
lines(d[d$threads == 8,]$n, d[d$threads == 8,]$time)
lines(d[d$threads == 4,]$n, d[d$threads == 4,]$time)
lines(d[d$threads == 2,]$n, d[d$threads == 2,]$time)
lines(d[d$threads == 1,]$n, d[d$threads == 1,]$time)
points(u$n, u$t, col = "red", pch = 20)
