d <- read.csv("results.csv")

pdf("results-heat.pdf", 8, 6)
plot(d[d$threads == 1,]$n, d[d$threads == 1,]$time, log = "xy",
	col = "red",
	main = "Laufzeit Lösung der Wärmeleitungsgleichung",
	xlab = "n", ylab = "Laufzeit [s]")

points(d[d$threads == 1,]$n, d[d$threads == 1,]$time, col = "red")
lines(d[d$threads == 1,]$n, d[d$threads == 1,]$time, col = "red")

points(d[d$threads == 2,]$n, d[d$threads == 2,]$time, col = "blue")
lines(d[d$threads == 2,]$n, d[d$threads == 2,]$time, col = "blue")

points(d[d$threads == 3,]$n, d[d$threads == 3,]$time, col = "green")
lines(d[d$threads == 3,]$n, d[d$threads == 3,]$time, col = "green")

points(d[d$threads == 4,]$n, d[d$threads == 4,]$time)
lines(d[d$threads == 4,]$n, d[d$threads == 4,]$time)
