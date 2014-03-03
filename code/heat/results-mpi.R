d <- read.csv("results-mpi.csv")

pdf("results-heat-mpi.pdf", 8, 6)
plot(d[d$threads == 1,]$n, d[d$threads == 1,]$time, log = "xy", type = "l",
	main = "Laufzeit Lösung der Wärmeleitungsgleichung",
	xlab = "n", ylab = "Laufzeit [s]")

points(d[d$threads == 2,]$n, d[d$threads == 2,]$time, pch = 20)
points(d[d$threads == 3,]$n, d[d$threads == 3,]$time, pch = 21)
points(d[d$threads == 4,]$n, d[d$threads == 4,]$time, pch = 22)
points(d[d$threads == 6,]$n, d[d$threads == 6,]$time, pch = 23)
points(d[d$threads == 9,]$n, d[d$threads == 9,]$time, pch = 24)

pdf("results-heat-threads.pdf", 8, 6)
plot(d[d$n == 3686400,]$threads, d[d$n == 3686400,]$time, log = "xy",
	main = "Laufzeit gegen Prozesszahl",
	xlab = "Anzahl Prozesse", ylab = "Laufzeit")
