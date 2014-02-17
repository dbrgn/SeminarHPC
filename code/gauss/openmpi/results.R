d <- read.csv("results.csv")
d$ln = log10(d$n)
d$lt = log10(d$time)
d$lthr = log10(d$threads)

u <- read.csv("../c/results.csv")
u$ln = log10(u$n)
u$lt = log10(u$time)

pdf("results.pdf", 8, 6)
plot(d[d$threads == 32,]$ln, d[d$threads == 32,]$lt, type = "l", col = "blue",
	main = "Gauss-Algorithmus, OpenMPI-Implementierung",
	xlab = "log10(n)", ylab = "log10(Laufzeit)",
	xlim = c(log10(20),log10(5000)), ylim = c(-4,2.6))
lines(d[d$threads == 16,]$ln, d[d$threads == 16,]$lt)
lines(d[d$threads == 12,]$ln, d[d$threads == 12,]$lt)
lines(d[d$threads == 8,]$ln, d[d$threads == 8,]$lt)
lines(d[d$threads == 4,]$ln, d[d$threads == 4,]$lt)
lines(d[d$threads == 2,]$ln, d[d$threads == 2,]$lt)
lines(d[d$threads == 1,]$ln, d[d$threads == 1,]$lt)
points(u$ln, u$lt, col = "red", pch = 20)

pdf("threads.pdf")
plot(d[d$n == 5000,]$lthr, d[d$n == 5000,]$lt, type = "l",
	main = "Gauss-Algorithmus, OpenMPI-Implementierung",
	xlab = "log10(Prozesse)", ylab = "log10(Laufzeit)",
	ylim = c(-1.5,2.6) )
l = lm(d[d$n == 5000,]$lt ~ d[d$n == 5000,]$lthr);
abline(l, col = "blue")

lines(d[d$n == 4000,]$lthr, d[d$n == 4000,]$lt,)
l = lm(d[d$n == 4000,]$lt ~ d[d$n == 4000,]$lthr);
abline(l, col = "blue")

lines(d[d$n == 3600,]$lthr, d[d$n == 3600,]$lt,)
l = lm(d[d$n == 3600,]$lt ~ d[d$n == 3600,]$lthr);
abline(l, col = "blue")

lines(d[d$n == 3000,]$lthr, d[d$n == 3000,]$lt,)
l = lm(d[d$n == 3000,]$lt ~ d[d$n == 3000,]$lthr);
abline(l, col = "blue")

lines(d[d$n == 2000,]$lthr, d[d$n == 2000,]$lt,)
l = lm(d[d$n == 2000,]$lt ~ d[d$n == 2000,]$lthr);
abline(l, col = "blue")

lines(d[d$n == 1000,]$lthr, d[d$n == 1000,]$lt,)
l = lm(d[d$n == 1000,]$lt ~ d[d$n == 1000,]$lthr);
abline(l, col = "blue")

lines(d[d$n == 500,]$lthr, d[d$n == 500,]$lt,)
l = lm(d[d$n == 500,]$lt ~ d[d$n == 500,]$lthr);
abline(l, col = "blue")
