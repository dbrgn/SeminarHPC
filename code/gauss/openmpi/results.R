d <- read.csv("results.csv")
d$ln = log10(d$n)
d$lt = log10(d$time)
d$lthr = log10(d$threads)

u <- read.csv("../c/results.csv")
u$ln = log10(u$n)
u$lt = log10(u$time)

pdf("results.pdf", 8, 6)
plot(d[d$threads == 32,]$n, d[d$threads == 32,]$time, type = "l", col = "blue",
	log = "xy",
	main = "Gauss-Algorithmus, OpenMPI-Implementierung",
	xlab = "n", ylab = "Laufzeit [s]",
	xlim = c(20,5000), ylim = c(0.0001,1000))
lines(d[d$threads == 16,]$n, d[d$threads == 16,]$time)
lines(d[d$threads == 12,]$n, d[d$threads == 12,]$time)
lines(d[d$threads == 8,]$n, d[d$threads == 8,]$time)
lines(d[d$threads == 4,]$n, d[d$threads == 4,]$time)
lines(d[d$threads == 2,]$n, d[d$threads == 2,]$time)
lines(d[d$threads == 1,]$n, d[d$threads == 1,]$time)
points(u$n, u$time, col = "red", pch = 20)
grid()

pdf("threads.pdf")
plot(d[d$n == 5000,]$threads, d[d$n == 5000,]$time, type = "l", log = "xy",
	main = "Gauss-Algorithmus, OpenMPI-Implementierung",
	xlab = "Prozesse", ylab = "Laufzeit [s]",
	ylim = c(0.02,500) )
grid()
l = lm(d[d$n == 5000,]$lt ~ d[d$n == 5000,]$lthr);
abline(l, col = "blue")

lines(d[d$n == 4000,]$threads, d[d$n == 4000,]$time,)
l = lm(d[d$n == 4000,]$lt ~ d[d$n == 4000,]$lthr);
abline(l, col = "blue")

lines(d[d$n == 3600,]$threads, d[d$n == 3600,]$time,)
l = lm(d[d$n == 3600,]$lt ~ d[d$n == 3600,]$lthr);
abline(l, col = "blue")

lines(d[d$n == 3000,]$threads, d[d$n == 3000,]$time,)
l = lm(d[d$n == 3000,]$lt ~ d[d$n == 3000,]$lthr);
abline(l, col = "blue")

lines(d[d$n == 2000,]$threads, d[d$n == 2000,]$time,)
l = lm(d[d$n == 2000,]$lt ~ d[d$n == 2000,]$lthr);
abline(l, col = "blue")

lines(d[d$n == 1000,]$threads, d[d$n == 1000,]$time,)
l = lm(d[d$n == 1000,]$lt ~ d[d$n == 1000,]$lthr);
abline(l, col = "blue")

lines(d[d$n == 500,]$threads, d[d$n == 500,]$time,)
l = lm(d[d$n == 500,]$lt ~ d[d$n == 500,]$lthr);
abline(l, col = "blue")
